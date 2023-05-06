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
 * File Name    : sf_touch_panel_chip_ft5x06.h
 * Description  : Implementation of touch panel chip ft5x06 interface.
 **********************************************************************************************************************/
#ifndef SF_TOUCH_PANEL_CHIP_FT5X06_H
#define SF_TOUCH_PANEL_CHIP_FT5X06_H

/*******************************************************************************************************************//**
 * @ingroup SF_Library
 * @defgroup SF_TOUCH_PANEL_CHIP_FT5X06 Touch Panel Framework Example for FT5X06
 * @brief RTOS-integrated touch panel chip ft5x06 example. Implementation of ft5x06 touch chip Driver.
 * It implements the following interfaces:
 *   - @ref SF_TOUCH_PANEL_CHIP_API
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "sf_touch_panel_chip_api.h"
#include "sf_external_irq_api.h"
#include "sf_i2c_api.h"
#include "r_ioport.h"

/* Common macro for SSP header files. There is also a corresponding SSP_FOOTER macro at the end of this file. */
SSP_HEADER

/**********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define SF_TOUCH_PANEL_CHIP_FT5X06_CODE_VERSION_MAJOR (2U)
#define SF_TOUCH_PANEL_CHIP_FT5X06_CODE_VERSION_MINOR (0U)

/**********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/
/** Instance control block. DO NOT INITIALIZE.  Initialization occurs when sf_touch_panel_chip_api_t::open is called */
typedef struct st_sf_touch_panel_chip_ft5x06_instance_ctrl
{
    ioport_port_pin_t                 pin;                     ///< Reset pin
    sf_i2c_instance_t const           * p_lower_lvl_framewrk;  ///< Pointer to lower level I2C framework.
    sf_external_irq_instance_t const  * p_lower_lvl_irq;       ///< Pointer to lower level external IRQ.
    sf_touch_panel_v2_payload_t       last_payload;            ///< Stores most recent payload for comparison.
} sf_touch_panel_chip_ft5x06_instance_ctrl_t;

/** Configuration for RTOS touch panel driver. */
typedef struct st_sf_touch_panel_chip_on_ft5x06_cfg
{
    ioport_port_pin_t                 pin;                     ///< Port pin connected to reset line on touch controller chip.
    sf_i2c_instance_t const           * p_lower_lvl_framewrk;  ///< Pointer to lower level I2C framework.
    sf_external_irq_instance_t const  * p_lower_lvl_irq;       ///< Pointer to lower level external IRQ.
} sf_touch_panel_chip_on_ft5x06_cfg_t;

/*******************************************************************************************************************//**
 * @} (end defgroup SF_TOUCH_PANEL_CHIP_FT5X06)
 **********************************************************************************************************************/

/* Common macro for SSP header files. There is also a corresponding SSP_HEADER macro at the top of this file. */
SSP_FOOTER

#endif /* SF_TOUCH_PANEL_CHIP_FT5X06_H */
