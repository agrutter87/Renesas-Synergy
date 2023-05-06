/***********************************************************************************************************************
 * Copyright [2015-2021] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
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
 * File Name    : sf_audio_playback_hw_dac.c
 * Description  : DAC implementation of audio playback interface.
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/

#include <string.h>
#include "sf_audio_playback_hw_dac.h"
#include "sf_audio_playback_hw_dac_cfg.h"
#include "sf_audio_playback_hw_dac_private_api.h"
#if SF_AUDIO_PLAYBACK_HW_DAC_CFG_DMAC_SUPPORT_ENABLE
#include "r_dmac.h"
#endif

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
/** Macro for error logger. */
#ifndef SF_AUDIO_PLAYBACK_HW_DAC_ERROR_RETURN
/*LDRA_INSPECTED 77 S This macro does not work when surrounded by parentheses. */
#define SF_AUDIO_PLAYBACK_HW_DAC_ERROR_RETURN(a, err) SSP_ERROR_RETURN((a), (err), &g_module_name[0], &g_sf_audio_playback_hw_dac_version)
#endif

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global variables (to be accessed by other files)
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
#if SF_AUDIO_PLAYBACK_HW_DAC_CFG_PARAM_CHECKING_ENABLE
ssp_err_t sf_audio_playback_hw_dac_lower_level_cfg_check (sf_audio_playback_hw_cfg_t              const * const p_cfg);

ssp_err_t sf_audio_playback_hw_dac_param_check (sf_audio_playback_hw_dac_instance_ctrl_t      * const p_ctrl,
                                                sf_audio_playback_hw_cfg_t              const * const p_cfg);
#endif

#if SF_AUDIO_PLAYBACK_HW_DAC_CFG_DMAC_SUPPORT_ENABLE
static void sf_audio_playback_hw_ramped_up_check (sf_audio_playback_hw_dac_instance_ctrl_t * p_ctrl);
static void sf_audio_playback_hw_callback_dmac (transfer_callback_args_t * p_args);
#endif

static void sf_audio_playback_hw_callback_timer (timer_callback_args_t * p_args);

static void sf_audio_playback_hw_transfer_size_config(transfer_cfg_t const * p_transfer, sf_audio_playback_hw_dac_cfg_t const * p_cfg_extend);

/***********************************************************************************************************************
 * Global Variables
 **********************************************************************************************************************/

/** Function pointers for DAC implementation of audio playback API.   */
/*LDRA_NOANALYSIS LDRA_INSPECTED below not working. */
/*LDRA_INSPECTED 27 D This structure must be accessible in user code. It cannot be static. */
sf_audio_playback_hw_api_t g_sf_audio_playback_hw_on_sf_audio_playback_hw_dac =
{
    .open        = SF_AUDIO_PLAYBACK_HW_DAC_Open,
    .start       = SF_AUDIO_PLAYBACK_HW_DAC_Start,
    .stop        = SF_AUDIO_PLAYBACK_HW_DAC_Stop,
    .play        = SF_AUDIO_PLAYBACK_HW_DAC_Play,
    .dataTypeGet = SF_AUDIO_PLAYBACK_HW_DAC_DataTypeGet,
    .close       = SF_AUDIO_PLAYBACK_HW_DAC_Close,
    .versionGet  = SF_AUDIO_PLAYBACK_HW_DAC_VersionGet
};
/*LDRA_ANALYSIS */

/** Version data structure used by error logger macro. */
#if defined(__GNUC__)
/* This structure is affected by warnings from a GCC compiler bug.  This pragma suppresses the warnings in this 
 * structure only.*/
/*LDRA_INSPECTED 69 S */
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
static const ssp_version_t g_sf_audio_playback_hw_dac_version =
{
    .api_version_minor  = SF_AUDIO_PLAYBACK_HW_API_VERSION_MINOR,
    .api_version_major  = SF_AUDIO_PLAYBACK_HW_API_VERSION_MAJOR,
    .code_version_major = SF_AUDIO_PLAYBACK_HW_DAC_CODE_VERSION_MAJOR,
    .code_version_minor = SF_AUDIO_PLAYBACK_HW_DAC_CODE_VERSION_MINOR
};
#if defined(__GNUC__)
/* Restore warning settings for 'missing-field-initializers' to as specified on command line. */
/*LDRA_INSPECTED 69 S */
#pragma GCC diagnostic pop
#endif

/** Name of module used by error logger macro */
#if BSP_CFG_ERROR_LOG != 0
static const char          g_module_name[] = "sf_audio_playback_hw_dac";
#endif
static const int16_t g_sf_audio_playback_ramp_up[] =
{
    0x0040, 0x0080, 0x00C0, 0x0100, 0x0140, 0x0180, 0x01C0, 0x0200,
    0x0240, 0x0280, 0x02C0, 0x0300, 0x0340, 0x0380, 0x03C0, 0x0400,
    0x0440, 0x0480, 0x04C0, 0x0500, 0x0540, 0x0580, 0x05C0, 0x0600,
    0x0640, 0x0680, 0x06C0, 0x0700, 0x0740, 0x0780, 0x07C0, 0x0800,
};

/*******************************************************************************************************************//**
 * @addtogroup SF_AUDIO_PLAYBACK_DAC
 *
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Open the DAC audio driver, including the DAC HAL driver and helper timer and transfer HAL drivers.
 *
 * @retval SSP_SUCCESS              Configuration of lower level drivers completed successfully.
 * @retval SSP_ERR_ASSERTION        Null Pointer.
 * @return                          See @ref Common_Error_Codes or HAL driver for other possible return codes or causes.
 *                                  This function calls
 *                                  * dac_api_t::open
 *                                  * timer_api_t::open
 *                                  * transfer_api_t::open
 *                                  * timer_api_t::close
 *                                  * dac_api_t::close
 * @note This function is reentrant if the lower level driver functions are reentrant.
 **********************************************************************************************************************/
ssp_err_t SF_AUDIO_PLAYBACK_HW_DAC_Open (sf_audio_playback_hw_ctrl_t      * const p_api_ctrl,
                                     sf_audio_playback_hw_cfg_t const * const p_cfg)
{
    sf_audio_playback_hw_dac_instance_ctrl_t * p_ctrl = (sf_audio_playback_hw_dac_instance_ctrl_t *) p_api_ctrl;
    ssp_err_t err;

#if SF_AUDIO_PLAYBACK_HW_DAC_CFG_PARAM_CHECKING_ENABLE
    err = sf_audio_playback_hw_dac_param_check(p_ctrl, p_cfg);
    SF_AUDIO_PLAYBACK_HW_DAC_ERROR_RETURN(SSP_SUCCESS == err, err);
#endif /* if SF_AUDIO_PLAYBACK_HW_DAC_CFG_PARAM_CHECKING_ENABLE */

    sf_audio_playback_hw_dac_cfg_t const * p_cfg_extend = (sf_audio_playback_hw_dac_cfg_t const *) p_cfg->p_extend;

    /** Open Timer driver at selected frequency */
    timer_cfg_t const * p_timer = p_cfg_extend->p_lower_lvl_timer->p_cfg;
    timer_cfg_t       timer;

#if SF_AUDIO_PLAYBACK_HW_DAC_CFG_DMAC_SUPPORT_ENABLE
    if (&g_transfer_on_dmac != p_cfg_extend->p_lower_lvl_transfer->p_api)
    {
#endif
        /** If DTC is selected, register the audio callback with the timer ISR.  DTC calls activation source
         *  ISR when the transfer is complete. */
        timer = *p_cfg_extend->p_lower_lvl_timer->p_cfg;
        timer.p_callback = sf_audio_playback_hw_callback_timer;
        timer.p_context  = p_ctrl;
        p_timer          = &timer;
#if SF_AUDIO_PLAYBACK_HW_DAC_CFG_DMAC_SUPPORT_ENABLE
    }
#endif

    err = p_cfg_extend->p_lower_lvl_timer->p_api->open(p_cfg_extend->p_lower_lvl_timer->p_ctrl, p_timer);
    SF_AUDIO_PLAYBACK_HW_DAC_ERROR_RETURN(SSP_SUCCESS == err, err);

    /** Open DAC module */
    err = p_cfg_extend->p_lower_lvl_dac->p_api->open(p_cfg_extend->p_lower_lvl_dac->p_ctrl, p_cfg_extend->p_lower_lvl_dac->p_cfg);
    if (SSP_SUCCESS != err)
    {
        /* Close previously opened drivers. */
        p_cfg_extend->p_lower_lvl_timer->p_api->close(p_cfg_extend->p_lower_lvl_timer->p_ctrl);
    }
    SF_AUDIO_PLAYBACK_HW_DAC_ERROR_RETURN(SSP_SUCCESS == err, err);

    /** Open transfer module to transfer from buffer to DAC output register */
    transfer_cfg_t const * p_transfer = p_cfg_extend->p_lower_lvl_transfer->p_cfg;

    /** Configure transfer size of driver depending upon the underlying DAC resolution */
    sf_audio_playback_hw_transfer_size_config(p_transfer, p_cfg_extend);

#if SF_AUDIO_PLAYBACK_HW_DAC_CFG_DMAC_SUPPORT_ENABLE
    transfer_cfg_t       transfer;
    if (&g_transfer_on_dmac == p_cfg_extend->p_lower_lvl_transfer->p_api)
    {
        /** If DMAC is selected, register the audio callback with the DMAC ISR. */
        transfer = *p_cfg_extend->p_lower_lvl_transfer->p_cfg;
        transfer.p_callback    = sf_audio_playback_hw_callback_dmac;
        transfer.p_context     = p_ctrl;
        p_transfer        = &transfer;
    }
#endif
    /** Open transfer driver */
    err = p_cfg_extend->p_lower_lvl_transfer->p_api->open(p_cfg_extend->p_lower_lvl_transfer->p_ctrl, p_transfer);
    if (SSP_ERR_NOT_ENABLED == err)
    {
        /* Not enabled just means that the transfer source pointer is NULL or the transfer length is 0, but these will
         * be set when transfer_api_t::reset is called later. */
        err = SSP_SUCCESS;
    }

    if (SSP_SUCCESS != err)
    {
        /* Close previously opened drivers if any error occurred. */
        p_cfg_extend->p_lower_lvl_timer->p_api->close(p_cfg_extend->p_lower_lvl_timer->p_ctrl);
        p_cfg_extend->p_lower_lvl_dac->p_api->close(p_cfg_extend->p_lower_lvl_dac->p_ctrl);
    }
    SF_AUDIO_PLAYBACK_HW_DAC_ERROR_RETURN(SSP_SUCCESS == err, err);

    /** Store driver data. */
    p_ctrl->p_lower_lvl_dac = p_cfg_extend->p_lower_lvl_dac;
    p_ctrl->p_lower_lvl_timer = p_cfg_extend->p_lower_lvl_timer;
    p_ctrl->p_lower_lvl_transfer = p_cfg_extend->p_lower_lvl_transfer;
    p_ctrl->p_callback = p_cfg->p_callback;
    p_ctrl->p_context = p_cfg->p_context;
    p_ctrl->is_dac_ramped_up = false;

    /** Play linear ramp data to get DAC up to half the maximum output. */
    SF_AUDIO_PLAYBACK_HW_DAC_Play(p_ctrl, &g_sf_audio_playback_ramp_up[0], sizeof(g_sf_audio_playback_ramp_up) / sizeof(int16_t));
    SF_AUDIO_PLAYBACK_HW_DAC_Start(p_ctrl);

#if SF_AUDIO_PLAYBACK_HW_DAC_CFG_DMAC_SUPPORT_ENABLE
    sf_audio_playback_hw_ramped_up_check(p_ctrl);
#endif

    return SSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Start the DAC and timer HAL drivers.
 *
 * @retval SSP_SUCCESS              Audio playback hardware started successfully.
 * @retval SSP_ERR_ASSERTION        The parameter p_ctrl is NULL.
 * @return                          See @ref Common_Error_Codes or HAL driver for other possible return codes or causes.
 *                                  This function calls
 *                                  * dac_api_t::start
 *                                  * timer_api_t::start
 * @note This function is reentrant if the lower level driver functions are reentrant.
 **********************************************************************************************************************/
ssp_err_t SF_AUDIO_PLAYBACK_HW_DAC_Start(sf_audio_playback_hw_ctrl_t * const p_api_ctrl)
{
    sf_audio_playback_hw_dac_instance_ctrl_t * p_ctrl = (sf_audio_playback_hw_dac_instance_ctrl_t *) p_api_ctrl;

#if SF_AUDIO_PLAYBACK_HW_DAC_CFG_PARAM_CHECKING_ENABLE
    SSP_ASSERT(NULL != p_ctrl);
#endif

    /** Start DAC. */
    ssp_err_t err = p_ctrl->p_lower_lvl_dac->p_api->start(p_ctrl->p_lower_lvl_dac->p_ctrl);
    SF_AUDIO_PLAYBACK_HW_DAC_ERROR_RETURN(SSP_SUCCESS == err, err);

    /** Start timer. */
    err = p_ctrl->p_lower_lvl_timer->p_api->start(p_ctrl->p_lower_lvl_timer->p_ctrl);
    SF_AUDIO_PLAYBACK_HW_DAC_ERROR_RETURN(SSP_SUCCESS == err, err);

    return SSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Stop the DAC and timer HAL drivers.
 *
 * @retval SSP_SUCCESS              Audio playback hardware stopped successfully.
 * @retval SSP_ERR_ASSERTION        The parameter p_ctrl is NULL.
 * @return                          See @ref Common_Error_Codes or HAL driver for other possible return codes or causes.
 *                                  This function calls
 *                                  * timer_api_t::stop
 *                                  * dac_api_t::stop
 * @note This function is reentrant if the lower level driver functions are reentrant.
 **********************************************************************************************************************/
ssp_err_t SF_AUDIO_PLAYBACK_HW_DAC_Stop (sf_audio_playback_hw_ctrl_t * const p_api_ctrl)
{
    sf_audio_playback_hw_dac_instance_ctrl_t * p_ctrl = (sf_audio_playback_hw_dac_instance_ctrl_t *) p_api_ctrl;

#if SF_AUDIO_PLAYBACK_HW_DAC_CFG_PARAM_CHECKING_ENABLE
    SSP_ASSERT(NULL != p_ctrl);
#endif

    /** Stop timer. */
    ssp_err_t err = p_ctrl->p_lower_lvl_timer->p_api->stop(p_ctrl->p_lower_lvl_timer->p_ctrl);
    SF_AUDIO_PLAYBACK_HW_DAC_ERROR_RETURN(SSP_SUCCESS == err, err);

    /** Stop DAC. */
    err = p_ctrl->p_lower_lvl_dac->p_api->stop(p_ctrl->p_lower_lvl_dac->p_ctrl);
    SF_AUDIO_PLAYBACK_HW_DAC_ERROR_RETURN(SSP_SUCCESS == err, err);

    return SSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Play a single audio buffer by input samples to the DAC at the sampling frequency configured by the timer.
 *
 * @retval SSP_SUCCESS              Buffer playback began successfully.
 * @retval SSP_ERR_ASSERTION        The parameter p_ctrl or p_buffer is NULL or length is greater than 0x10000UL.
 * @return                          See @ref Common_Error_Codes or HAL driver for other possible return codes or causes.
 *                                  This function calls
 *                                  * transfer_api_t::reset
 * @note This function is reentrant if the lower level driver functions are reentrant.
 **********************************************************************************************************************/
ssp_err_t SF_AUDIO_PLAYBACK_HW_DAC_Play (sf_audio_playback_hw_ctrl_t * const p_api_ctrl,
                                     int16_t         const * const p_buffer,
                                     uint32_t                      length)
{
    sf_audio_playback_hw_dac_instance_ctrl_t * p_ctrl = (sf_audio_playback_hw_dac_instance_ctrl_t *) p_api_ctrl;

#if SF_AUDIO_PLAYBACK_HW_DAC_CFG_PARAM_CHECKING_ENABLE
    SSP_ASSERT(NULL != p_ctrl);
    SSP_ASSERT(NULL != p_buffer);
    SSP_ASSERT(length < 0x10000UL);
#endif

    /** Reset transfer. */
    ssp_err_t err = p_ctrl->p_lower_lvl_transfer->p_api->reset(p_ctrl->p_lower_lvl_transfer->p_ctrl,
                                                   p_buffer,
                                                   NULL,
                                                   (uint16_t) length);
    SF_AUDIO_PLAYBACK_HW_DAC_ERROR_RETURN(SSP_SUCCESS == err, err);

    return SSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Provides the expected data type in the pointer p_data_type.
 *
 * @retval SSP_SUCCESS              Data type stored in p_data_type.
 * @retval SSP_ERR_ASSERTION        The parameter p_ctrl or p_data_type is NULL.
 *
 * @note This function is reentrant if the lower level driver functions are reentrant.
 **********************************************************************************************************************/
ssp_err_t SF_AUDIO_PLAYBACK_HW_DAC_DataTypeGet (sf_audio_playback_hw_ctrl_t * const p_ctrl,
                                     sf_audio_playback_data_type_t   * const p_data_type)
{
#if SF_AUDIO_PLAYBACK_HW_DAC_CFG_PARAM_CHECKING_ENABLE
    SSP_ASSERT(NULL != p_ctrl);
    SSP_ASSERT(NULL != p_data_type);
#endif

    /** Store data type.  The Synergy DAC supports only 12-bit unsigned data. */
    dac_info_t  dac_info;
    dac_info.bit_width = (uint8_t) 0U;
    sf_audio_playback_hw_dac_instance_ctrl_t * p_api_ctrl = (sf_audio_playback_hw_dac_instance_ctrl_t *) p_ctrl;
    p_api_ctrl->p_lower_lvl_dac->p_api->infoGet(&dac_info);
    p_data_type->scale_bits_max = dac_info.bit_width; 
    p_data_type->is_signed = 0;

    return SSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Close open audio driver.
 *
 * @retval     SSP_SUCCESS              Successful close.
 * @retval     SSP_ERR_ASSERTION        The parameter p_ctrl is NULL.
 * @return                              See @ref Common_Error_Codes or HAL driver for other possible return codes or causes.
 *                                      This function calls
 *                                      * timer_api_t::close
 *                                      * dac_api_t::close
 *                                      * transfer_api_t::close
 * @note This function is reentrant for different channels.  It is not reentrant for the same channel.
 **********************************************************************************************************************/

ssp_err_t SF_AUDIO_PLAYBACK_HW_DAC_Close (sf_audio_playback_hw_ctrl_t * const p_api_ctrl)
{
    sf_audio_playback_hw_dac_instance_ctrl_t * p_ctrl = (sf_audio_playback_hw_dac_instance_ctrl_t *) p_api_ctrl;

#if SF_AUDIO_PLAYBACK_HW_DAC_CFG_PARAM_CHECKING_ENABLE
    SSP_ASSERT(NULL != p_ctrl);
#endif

    /** Close timer driver. */
    ssp_err_t err;
    err = p_ctrl->p_lower_lvl_timer->p_api->close(p_ctrl->p_lower_lvl_timer->p_ctrl);
    SF_AUDIO_PLAYBACK_HW_DAC_ERROR_RETURN(SSP_SUCCESS == err, err);

    /** Close DAC driver. */
    err = p_ctrl->p_lower_lvl_dac->p_api->close(p_ctrl->p_lower_lvl_dac->p_ctrl);
    SF_AUDIO_PLAYBACK_HW_DAC_ERROR_RETURN(SSP_SUCCESS == err, err);

    /** Close transfer driver. */
    err = p_ctrl->p_lower_lvl_transfer->p_api->close(p_ctrl->p_lower_lvl_transfer->p_ctrl);
    SF_AUDIO_PLAYBACK_HW_DAC_ERROR_RETURN(SSP_SUCCESS == err, err);

    /* Clear the DAC ramped up state */
    p_ctrl->is_dac_ramped_up = false;

    return SSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Stores the version of the firmware and API in provided pointer p_version.
 *
 * @retval SSP_ERR_ASSERTION        The parameter p_version is NULL.
 * @retval SSP_SUCCESS              Module version successfully stored in p_version.
 *
 * @note This function is reentrant.
 *
 **********************************************************************************************************************/
ssp_err_t  SF_AUDIO_PLAYBACK_HW_DAC_VersionGet (ssp_version_t * const p_version)
{
#if SF_AUDIO_PLAYBACK_HW_DAC_CFG_PARAM_CHECKING_ENABLE
    SSP_ASSERT(NULL != p_version);
#endif

    p_version->version_id = g_sf_audio_playback_hw_dac_version.version_id;

    return SSP_SUCCESS;
}

#if SF_AUDIO_PLAYBACK_HW_DAC_CFG_PARAM_CHECKING_ENABLE
/******************************************************************************************************************//**
 * This is the parameter checking subroutine for the SF_AUDIO_PLAYBACK_HW_DAC_Open API.
 *
 * @param[in]      p_cfg           Configuration structure for this instance.
 *
 * @retval  SSP_SUCCESS            No parameter checking violations identified.
 * @retval  SSP_ERR_ASSERTION      A lower level configuration is invalid.
 **********************************************************************************************************************/
ssp_err_t sf_audio_playback_hw_dac_lower_level_cfg_check (sf_audio_playback_hw_cfg_t              const * const p_cfg)
{
    sf_audio_playback_hw_dac_cfg_t const * p_cfg_extend = (sf_audio_playback_hw_dac_cfg_t const *) p_cfg->p_extend;

    SSP_ASSERT(TIMER_MODE_PERIODIC == p_cfg_extend->p_lower_lvl_timer->p_cfg->mode);
    SSP_ASSERT(TRANSFER_MODE_NORMAL == p_cfg_extend->p_lower_lvl_transfer->p_cfg->p_info->mode);
    SSP_ASSERT(TRANSFER_ADDR_MODE_INCREMENTED == p_cfg_extend->p_lower_lvl_transfer->p_cfg->p_info->src_addr_mode);
    SSP_ASSERT(TRANSFER_ADDR_MODE_FIXED == p_cfg_extend->p_lower_lvl_transfer->p_cfg->p_info->dest_addr_mode);
#if SF_AUDIO_PLAYBACK_HW_DAC_CFG_DMAC_SUPPORT_ENABLE
    if (&g_transfer_on_dmac != p_cfg_extend->p_lower_lvl_transfer->p_api)
    {
#endif
        SSP_ASSERT(TRANSFER_IRQ_END == p_cfg_extend->p_lower_lvl_transfer->p_cfg->p_info->irq);
#if SF_AUDIO_PLAYBACK_HW_DAC_CFG_DMAC_SUPPORT_ENABLE
    }
#endif
    SSP_ASSERT(DAC_DATA_FORMAT_FLUSH_RIGHT == p_cfg_extend->p_lower_lvl_dac->p_cfg->data_format);

    return SSP_SUCCESS;
}

/******************************************************************************************************************//**
 * This function performs parameter checking for SF_AUDIO_PLAYBACK_HW_DAC_Open.
 *
 * @param[in,out]  p_ctrl             Control block for this instance.
 * @param[in]      p_cfg              Configuration structure for this instance..
 *
 * @retval  SSP_SUCCESS            No parameter checking violations identified.
 * @retval  SSP_ERR_ASSERTION      A pointer is NULL or a lower level configuration is invalid.
 **********************************************************************************************************************/
ssp_err_t sf_audio_playback_hw_dac_param_check (sf_audio_playback_hw_dac_instance_ctrl_t      * const p_ctrl,
                                                sf_audio_playback_hw_cfg_t              const * const p_cfg)
{
    SSP_ASSERT(NULL != p_ctrl);
    SSP_ASSERT(NULL != p_cfg);

    sf_audio_playback_hw_dac_cfg_t const * p_cfg_extend = (sf_audio_playback_hw_dac_cfg_t const *) p_cfg->p_extend;

    SSP_ASSERT(NULL != p_cfg_extend);
    SSP_ASSERT(NULL != p_cfg_extend->p_lower_lvl_dac);
    SSP_ASSERT(NULL != p_cfg_extend->p_lower_lvl_timer);
    SSP_ASSERT(NULL != p_cfg_extend->p_lower_lvl_transfer);
    SSP_ASSERT(NULL != p_cfg_extend->p_lower_lvl_dac->p_api);
    SSP_ASSERT(NULL != p_cfg_extend->p_lower_lvl_timer->p_api);
    SSP_ASSERT(NULL != p_cfg_extend->p_lower_lvl_transfer->p_api);

    return sf_audio_playback_hw_dac_lower_level_cfg_check(p_cfg);
}
#endif /* if SF_AUDIO_PLAYBACK_HW_DAC_CFG_PARAM_CHECKING_ENABLE */

#if SF_AUDIO_PLAYBACK_HW_DAC_CFG_DMAC_SUPPORT_ENABLE
/******************************************************************************************************************//**
 * This function wait till the DAC is ramped to half of maximum output or till the timeout.
 *
 * @param[in,out]  p_ctrl   Control block for this instance.
 **********************************************************************************************************************/
static void sf_audio_playback_hw_ramped_up_check (sf_audio_playback_hw_dac_instance_ctrl_t * p_ctrl)
{
    /** If DMAC is selected, wait till the DAC is ramped to half of maximum output or till the timeout */
    if (&g_transfer_on_dmac == p_ctrl->p_lower_lvl_transfer->p_api)
    {
        uint32_t timeout = 0x4ffffffULL;
        while ((false == p_ctrl->is_dac_ramped_up) && (0U < timeout))
        {
            timeout--;
        }
    }
}

/*******************************************************************************************************************//**
 * Callback function intercepted from DMAC driver.
 *
 * @param[in]  p_args Callback data can be used to identify what triggered the interrupt.
 **********************************************************************************************************************/
static void sf_audio_playback_hw_callback_dmac (transfer_callback_args_t * p_args)
{
    if (NULL != p_args->p_context)
    {
        /** Recover context from ISR. */
        sf_audio_playback_hw_dac_instance_ctrl_t * p_ctrl = (sf_audio_playback_hw_dac_instance_ctrl_t *) (p_args->p_context);

        /** The first playback complete event is generated because of the dac ramp up,
         * hence set the ramped up state and return without further callback operation */
        if (false == p_ctrl->is_dac_ramped_up)
        {
            p_ctrl->is_dac_ramped_up = true;
            return;
        }

        /** Create callback arguments. */
        sf_audio_playback_hw_callback_args_t args;
        args.p_context = p_ctrl->p_context;
        args.event = SF_AUDIO_PLAYBACK_HW_EVENT_PLAYBACK_COMPLETE;

        /** Call user callback if not NULL. */
        if (NULL != p_ctrl->p_callback)
        {
            p_ctrl->p_callback(&args);
        }
    }
} /* End of function sf_sf_audio_playback_hw_callback_dmac */
#endif

/*******************************************************************************************************************//**
 * Callback function intercepted from timer driver.
 *
 * @param[in]  p_args Callback data can be used to identify what triggered the interrupt.
 **********************************************************************************************************************/
static void sf_audio_playback_hw_callback_timer (timer_callback_args_t * p_args)
{
    if (NULL != p_args->p_context)
    {
        /** Recover context from ISR. */
        sf_audio_playback_hw_dac_instance_ctrl_t * p_ctrl = (sf_audio_playback_hw_dac_instance_ctrl_t *) (p_args->p_context);

        /** Create callback arguments. */
        sf_audio_playback_hw_callback_args_t args;
        args.p_context = p_ctrl->p_context;
        args.event = SF_AUDIO_PLAYBACK_HW_EVENT_PLAYBACK_COMPLETE;

        /** Call user callback if not NULL. */
        if (NULL != p_ctrl->p_callback)
        {
            p_ctrl->p_callback(&args);
        }
    }
} /* End of function sf_sf_audio_playback_hw_callback_timer */

/*******************************************************************************************************************//**
 * Configures the transfer size of the transfer driver depending upon the underlying DAC resolution.
 *
 * @param[in,out]  p_transfer                        Pointer to transfer driver configuration.
 * @param[in,out]  p_cfg_extend                      Pointer to hardware dependent configuration for DAC audio driver.
 **********************************************************************************************************************/
static void sf_audio_playback_hw_transfer_size_config(transfer_cfg_t const * p_transfer, sf_audio_playback_hw_dac_cfg_t const * p_cfg_extend)
{
    dac_info_t  dac_info ;
    dac_info.bit_width = (uint8_t) 0U;
    p_cfg_extend->p_lower_lvl_dac->p_api->infoGet(&dac_info);
    if(dac_info.bit_width > 8U)
    {
        p_transfer->p_info->size = TRANSFER_SIZE_2_BYTE;
    }
    else
    {
        p_transfer->p_info->size = TRANSFER_SIZE_1_BYTE;
    }
}
/*******************************************************************************************************************//**
 * @} (end addtogroup SF_AUDIO_PLAYBACK_HW_DAC)
 **********************************************************************************************************************/
