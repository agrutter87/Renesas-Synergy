/***********************************************************************************************************************
 * Copyright [2018-2021] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
 * 
 * This file is part of Renesas SynergyTM Software Package (SSP)
 *
 * The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
 * and/or its licensors ("Renesas") and subject to statutory and contractual protections.
 *
 * This file is subject to a Renesas SSP license agreement. Unless otherwise agreed in an SSP license agreement with
 * Renesas: 1) you may not use, copy, modify, distribute, display, or perform the contents; 2) you may not use any name
 * or mark of Renesas for advertising or publicity purposes or in connection with your use of the contents; 3) RENESAS
 * MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED
 * "AS IS" WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, AND NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR
 * CONSEQUENTIAL DAMAGES, INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF
 * CONTRACT OR TORT, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents
 * included in this file may be subject to different terms.
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : sf_touch_panel_chip_ft5x06.c
 * Description  : Touch panel framework chip specific implementation for the FT5X06 touch panel chip.
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "sf_touch_panel_chip_ft5x06.h"
#include "sf_touch_panel_v2.h"

/**********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

#define FT5X06_DOWN          0
#define FT5X06_UP            1
#define FT5X06_CONTACT       2

#define FT5X06_REG_TOUCH1_XH 0x03

#define extract_e(t) ((uint8_t) ((t).event))
#define extract_x(t) ((int16_t) (((t).x_msb << 8) | ((t).x_lsb)))
#define extract_y(t) ((int16_t) (((t).y_msb << 8) | ((t).y_lsb)))
#define I2C_TIMEOUT                (10U)

/**********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/
/** Driver-specific touch point register mapping */
typedef struct st_ft5x06_touch
{
    uint8_t  x_msb : 4;
    uint8_t        : 2;
    uint8_t  event : 2;
    uint8_t  x_lsb;

    uint8_t  y_msb : 4;
    uint8_t  id    : 4;
    uint8_t  y_lsb : 8;

    uint8_t  res1;
    uint8_t  res2;
} ft5x06_touch_t;

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
static inline sf_touch_panel_v2_event_t process_event (uint8_t raw_event);

static ssp_err_t ft5x06_open (sf_touch_panel_chip_ctrl_t * const p_ctrl,
                              sf_touch_panel_chip_cfg_t const * const p_cfg);

static ssp_err_t ft5x06_payload_get (sf_touch_panel_chip_ctrl_t * const    p_ctrl,
                                     sf_touch_panel_v2_payload_t * const p_payload);

static ssp_err_t ft5x06_reset (sf_touch_panel_chip_ctrl_t * const p_ctrl);

static ssp_err_t ft5x06_close (sf_touch_panel_chip_ctrl_t * const p_ctrl);

static ssp_err_t ft5x06_versionGet (ssp_version_t * const p_version);

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/
static const ssp_version_t g_version =
{
    .api_version_minor  = SF_TOUCH_PANEL_CHIP_API_VERSION_MINOR,
    .api_version_major  = SF_TOUCH_PANEL_CHIP_API_VERSION_MAJOR,
    .code_version_major = SF_TOUCH_PANEL_CHIP_FT5X06_CODE_VERSION_MAJOR,
    .code_version_minor = SF_TOUCH_PANEL_CHIP_FT5X06_CODE_VERSION_MINOR
};

const sf_touch_panel_chip_api_t g_sf_touch_panel_chip_ft5x06 =
{
    .open       = ft5x06_open,
    .payloadGet = ft5x06_payload_get,
    .reset      = ft5x06_reset,
    .close      = ft5x06_close,
    .versionGet = ft5x06_versionGet
};

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief  Translates event code raw data.
 *
 * @param[in]   raw_event                   Event code read from the touch controller.
 * @retval      sf_touch_panel_v2_event_t   The local representation of the event code the touch controller reported.
 **********************************************************************************************************************/
static inline sf_touch_panel_v2_event_t process_event (uint8_t raw_event)
{
    sf_touch_panel_v2_event_t event = SF_TOUCH_PANEL_V2_EVENT_INVALID;

    switch(raw_event)
    {
    case FT5X06_UP:
        event = SF_TOUCH_PANEL_V2_EVENT_UP;
        break;

    case FT5X06_DOWN:
        event = SF_TOUCH_PANEL_V2_EVENT_DOWN;
        break;

    case FT5X06_CONTACT:
        event = SF_TOUCH_PANEL_V2_EVENT_HOLD;
        break;
    default:
        break;
    }
    return event;
}

/*******************************************************************************************************************//**
 * @brief  Opens the lower level drivers. Implements sf_touch_panel_chip_api_t::open.
 *
 * @param[in,out]  p_api_ctrl   Pointer to control block from touch panel framework.
 * @param[in]      p_cfg    Pointer to configuration structure. All elements of the structure must be set by user.
 **********************************************************************************************************************/
static ssp_err_t ft5x06_open (sf_touch_panel_chip_ctrl_t * const p_api_ctrl,
                              sf_touch_panel_chip_cfg_t const * const p_cfg)
{
    sf_touch_panel_chip_ft5x06_instance_ctrl_t * p_ctrl = (sf_touch_panel_chip_ft5x06_instance_ctrl_t *) p_api_ctrl;

    /** Initialize previous event. */
    p_ctrl->last_payload.event_type = SF_TOUCH_PANEL_V2_EVENT_INVALID;
    p_ctrl->last_payload.x          = -1;
    p_ctrl->last_payload.y          = -1;

    sf_touch_panel_chip_on_ft5x06_cfg_t const * const p_cfg_extend = p_cfg->p_extend;
    p_ctrl->p_lower_lvl_framewrk     = p_cfg_extend->p_lower_lvl_framewrk;
    p_ctrl->p_lower_lvl_irq          = p_cfg_extend->p_lower_lvl_irq;
    p_ctrl->pin                      = p_cfg_extend->pin;
    ssp_err_t err                    = SSP_SUCCESS;

    err = p_cfg_extend->p_lower_lvl_framewrk->p_api->open(p_cfg_extend->p_lower_lvl_framewrk->p_ctrl,
                                                          p_cfg_extend->p_lower_lvl_framewrk->p_cfg);
    if (SSP_SUCCESS != err)
    {
        return err;
    }

    err = p_cfg_extend->p_lower_lvl_irq->p_api->open(p_cfg_extend->p_lower_lvl_irq->p_ctrl,
                                                     p_cfg_extend->p_lower_lvl_irq->p_cfg);
    if (SSP_SUCCESS != err)
    {
        return err;
    }

    return SSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief  Reads the touch event data from the touch controller. Implements sf_touch_panel_chip_api_t::payloadGet.
 *
 * @param[in,out]  p_api_ctrl       Pointer to control block from touch panel framework.
 * @param[out]     p_payload    Pointer to the payload data structure. Touch data provided should be processed to
 *                              logical pixel values.
 **********************************************************************************************************************/
static ssp_err_t ft5x06_payload_get (sf_touch_panel_chip_ctrl_t * const p_api_ctrl,
                                     sf_touch_panel_v2_payload_t * const p_payload)
{
    sf_touch_panel_chip_ft5x06_instance_ctrl_t * p_ctrl = (sf_touch_panel_chip_ft5x06_instance_ctrl_t *) p_api_ctrl;

    sf_i2c_ctrl_t                     * p_i2c_ctrl    = p_ctrl->p_lower_lvl_framewrk->p_ctrl;
    sf_external_irq_ctrl_t            * p_irq_ctrl    = p_ctrl->p_lower_lvl_irq->p_ctrl;
    sf_i2c_api_t             const    * p_i2c_api     = p_ctrl->p_lower_lvl_framewrk->p_api;
    sf_external_irq_api_t    const    * p_irq_api     = p_ctrl->p_lower_lvl_irq->p_api;
    ssp_err_t                         err             = SSP_SUCCESS;

    /** Wait for pin interrupt from touch controller. */
    err = p_irq_api->wait(p_irq_ctrl, TX_WAIT_FOREVER);
    if (SSP_SUCCESS != err)
    {
        return err;
    }

    /* Initialize all return parameters */
    p_payload->event_type = SF_TOUCH_PANEL_V2_EVENT_INVALID;
    p_payload->x          = 0;
    p_payload->y          = 0;

    /* Read the data about the touch point(s) */
    ft5x06_touch_t touch[1];
    uint8_t        reg = FT5X06_REG_TOUCH1_XH;

    err = p_i2c_api->write(p_i2c_ctrl, &reg, 1, true, I2C_TIMEOUT);
    if (SSP_SUCCESS != err)
    {
        return SSP_ERR_INTERNAL;
    }

    err = p_i2c_api->read(p_i2c_ctrl, (uint8_t *) &touch[0], (sizeof(touch) - 2), false, I2C_TIMEOUT);
    if (SSP_SUCCESS != err)
    {
        return SSP_ERR_INTERNAL;
    }

    /* Process the raw data for the touch point(s) into useful data */
    sf_touch_panel_v2_event_t new_event = process_event(extract_e(touch[0]));
    int16_t                new_x = extract_x(touch[0]);
    int16_t                new_y = extract_y(touch[0]);

    /* Figure out if the new event makes sense based on the previous event */
    switch (new_event)
    {
        case SF_TOUCH_PANEL_V2_EVENT_INVALID:
        case SF_TOUCH_PANEL_V2_EVENT_UP:
        case SF_TOUCH_PANEL_V2_EVENT_DOWN:
        case SF_TOUCH_PANEL_V2_EVENT_NONE:
            p_payload->event_type = new_event;
            break;

        case SF_TOUCH_PANEL_V2_EVENT_MOVE:
        case SF_TOUCH_PANEL_V2_EVENT_HOLD:
            if ((p_ctrl->last_payload.x != new_x) || (p_ctrl->last_payload.y != new_y))
            {
                p_payload->event_type = SF_TOUCH_PANEL_V2_EVENT_MOVE;
            }
            else
            {
                p_payload->event_type = SF_TOUCH_PANEL_V2_EVENT_HOLD;
            }
            break;
    }

    /* x and y are reversed for landscape screen mode */
    p_payload->x                    = new_y;
    p_payload->y                    = new_x;

    p_ctrl->last_payload.event_type = p_payload->event_type;
    p_ctrl->last_payload.x          = new_x;
    p_ctrl->last_payload.y          = new_y;

    return SSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief  Resets the touch chip. Implements sf_touch_panel_chip_api_t::reset.
 *
 * @param[in]  p_api_ctrl   Pointer to control block from touch panel framework.
 **********************************************************************************************************************/
static ssp_err_t ft5x06_reset (sf_touch_panel_chip_ctrl_t * const p_api_ctrl)
{
    /* Parameter checking done in touch panel framework. */
    sf_touch_panel_chip_ft5x06_instance_ctrl_t * p_ctrl = (sf_touch_panel_chip_ft5x06_instance_ctrl_t *) p_api_ctrl;

    /** Reset touch chip by setting GPIO reset pin low. */
    g_ioport_on_ioport.pinWrite(p_ctrl->pin, IOPORT_LEVEL_LOW);

    /** Wait 10 ms. */
    tx_thread_sleep(1);

    /** Release touch chip from reset */
    g_ioport_on_ioport.pinWrite(p_ctrl->pin, IOPORT_LEVEL_HIGH);

    /** Wait 10 ms. */
    tx_thread_sleep(1);

    return SSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief  Closes the lower level drivers. Implements sf_touch_panel_chip_api_t::close.
 *
 * @param[in,out]  p_api_ctrl   Pointer to control block from touch panel framework.
 **********************************************************************************************************************/
static ssp_err_t ft5x06_close (sf_touch_panel_chip_ctrl_t * const p_api_ctrl)
{

    sf_touch_panel_chip_ft5x06_instance_ctrl_t * p_ctrl = (sf_touch_panel_chip_ft5x06_instance_ctrl_t *) p_api_ctrl;
    sf_i2c_ctrl_t * const                        p_i2c_ctrl      = p_ctrl->p_lower_lvl_framewrk->p_ctrl;
    ssp_err_t                                    err             = SSP_SUCCESS;

    /** Close the lower level external IRQ driver. */
    err = p_ctrl->p_lower_lvl_irq->p_api->close(p_ctrl->p_lower_lvl_irq->p_ctrl);
    if (SSP_SUCCESS != err)
    {
        return err;
    }

    /** Resets touch chip by setting GPIO reset pin low. */
    g_ioport_on_ioport.pinWrite(p_ctrl->pin, IOPORT_LEVEL_LOW);
    /** Waits for a while (keep the reset signal low longer than 1ms) */
    tx_thread_sleep(2);

    /** Close the lower level I2C framework. */
    err = p_ctrl->p_lower_lvl_framewrk->p_api->close(p_i2c_ctrl);
    if (SSP_ERR_ABORTED == err)
    {
      /** If I2C Abort returned from previous close, one more try to close I2C driver. */
      err = p_ctrl->p_lower_lvl_framewrk->p_api->close(p_i2c_ctrl);
    }
    if (SSP_SUCCESS != err)
    {
        return err;
    }

    return SSP_SUCCESS;
}

/******************************************************************************************************************//**
 * @brief Implements sf_touch_panel_chip_api_t::versionGet.
 *
 * @param[out]  p_version  Code and API version used are stored here.
 *********************************************************************************************************************/
static ssp_err_t ft5x06_versionGet (ssp_version_t * const p_version)
{
#if SF_TOUCH_PANEL_V2_CFG_PARAM_CHECKING_ENABLE
    SSP_ASSERT(NULL != p_version);
#endif

    *p_version = g_version;

    return SSP_SUCCESS;
}
