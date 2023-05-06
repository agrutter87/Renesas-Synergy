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

/*************************************************************************//*******************************************
 * File Name    : sf_audio_playback.c
 * Description  : Source code for audio framework functions.
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "bsp_api.h"
#include "sf_audio_playback.h"
#include "sf_audio_playback_hw_api.h"
#include "sf_audio_playback_cfg.h"
#include "sf_audio_playback_private_api.h"
/* CMSIS-CORE currently generates warnings when compiling arm_math.h.
 * We are not modifying these files so we will ignore these warnings temporarily. */
#if defined(__GNUC__)
/*LDRA_INSPECTED 69 S */
#pragma GCC diagnostic ignored "-Wconversion"
/*LDRA_INSPECTED 69 S */
#pragma GCC diagnostic ignored "-Wsign-conversion"
/*LDRA_INSPECTED 69 S */
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"
/*LDRA_INSPECTED 69 S */
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif
#if defined(__IAR_SYSTEMS_ICC__)
/*LDRA_INSPECTED 69 S */
#pragma diag_suppress=Pe223
#endif
#if   defined(__IAR_SYSTEMS_ICC__)
    #if   (__CORE__ == __ARM7EM__)
#define ARM_MATH_CM4
    #elif (__CORE__ == __ARM6M__)
#define ARM_MATH_CM0PLUS
    #elif (__CORE__ == __ARM8M_BASELINE__)
#define ARM_MATH_ARMV8MBL
    #endif
#elif defined(__GNUC__)
    #if   __ARM_ARCH_7EM__
#define ARM_MATH_CM4
    #elif __ARM_ARCH_6M__
#define ARM_MATH_CM0PLUS
    #elif __ARM_ARCH_8M_BASE__
#define ARM_MATH_ARMV8MBL
    #endif
#endif
/*LDRA_NOANALYSIS arm_math.h is not maintained by Renesas, so LDRA analysis is skipped for this file only. */
#include "arm_math.h"
/*LDRA_ANALYSIS */
#if defined(__IAR_SYSTEMS_ICC__)
/*LDRA_INSPECTED 69 S */
#pragma diag_default=Pe223
#endif
#if defined(__GNUC__)
/* Restore warning settings to as specified on command line. */
/*LDRA_INSPECTED 69 S */
#pragma GCC diagnostic pop
#endif

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
/** Macro for error logger. */
#ifndef SF_AUDIO_PLAYBACK_ERROR_RETURN
/*LDRA_INSPECTED 77 S This macro does not work when surrounded by parentheses. */
#define SF_AUDIO_PLAYBACK_ERROR_RETURN(a, err) SSP_ERROR_RETURN((a), (err), &g_module_name[0], &g_sf_audio_playback_version)
#endif

#ifndef SF_AUDIO_PLAYBACK_ERROR_LOG
/*LDRA_INSPECTED 77 S This macro does not work when surrounded by parentheses. */
#define SF_AUDIO_PLAYBACK_ERROR_LOG(err) SSP_ERROR_LOG((err), g_module_name, &g_sf_audio_playback_version)
#endif

/** Maximum wait for queue creation is 5 ticks. */
#define SF_AUDIO_PLAYBACK_PRV_WAIT               (5U)

/** "AUDO" in ASCII, used to identify general timer handle */
#define OPEN (0x6175646FU)

/** Lowest supported playback frequency */
#define SF_AUDIO_PLAYBACK_PRV_LOWEST_PLAYBACK_HZ (8000U)

/** Only 16 bit sample arrays currently supported. */
#define SF_AUDIO_PLAYBACK_BITS_PER_SAMPLE        (16U)

/** Maximum number of samples per play. */
#define SF_AUDIO_PLAYBACK_DTC_NORMAL_MAX_LENGTH  (0x400U)

/** Macro to explain constant in macro calculation below */
#define MSEC_PER_SEC (1000U)

/** RTOS tick frequency in ms. */
#define RTOS_TICK_MS (MSEC_PER_SEC / TX_TIMER_TICKS_PER_SECOND)

/** ThreadX Queue ID, used to determine if a queue is created. */
#define TX_QUEUE_ID  ((ULONG) 0x51554555U)

/** Maximum playback timeout is (maximum number of samples) / (slowest playback frequency) / (RTOS tick in s).  This
 * framework will wait twice this time before failing, and at least 2 ticks. */
#define SF_AUDIO_PLAYBACK_PRV_PLAYBACK_TIMEOUT                               \
    (((((SF_AUDIO_PLAYBACK_CFG_BUFFER_SIZE_BYTES * MSEC_PER_SEC) / RTOS_TICK_MS) \
       / SF_AUDIO_PLAYBACK_PRV_LOWEST_PLAYBACK_HZ) + 1U) * 2U)

/* Internal ThreadX mutex ID, used to know if a mutex is created. */
#define TX_MUTEX_ID                             ((ULONG) 0x4D555445)

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/
/** Bit vector of events used in SF_AUDIO_PLAYBACK_Start.
 * @warning The system only supports up to 6 streams on a single event flag group (5 bits per stream, 32 bits max). */
typedef enum e_sf_audio_playback_prv_flag
{
    SF_AUDIO_PLAYBACK_PRV_FLAG_PLAYBACK_COMPLETE   = 1 << 0, ///< Playback is complete
    SF_AUDIO_PLAYBACK_PRV_FLAG_RESTART             = 1 << 1, ///< Playback must be restarted
} sf_audio_playback_prv_flag_t;

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
static void sf_audio_playback_callback (sf_audio_playback_hw_callback_args_t * p_args);

static ssp_err_t sf_audio_playback_common_open (sf_audio_playback_common_instance_ctrl_t * const p_ctrl,
                                                sf_audio_playback_common_cfg_t     const * const p_cfg);

static ssp_err_t sf_audio_playback_hw_open (sf_audio_playback_common_instance_ctrl_t * const p_ctrl,
                                            sf_audio_playback_common_cfg_t     const * const p_cfg);

static void sf_audio_playback_samples_limit(uint32_t * const p_num_transfers, uint32_t limit);

static void sf_audio_playback_samples_prepare (sf_audio_playback_common_instance_ctrl_t * const           p_ctrl);

static ssp_err_t sf_audio_playback_message_send(sf_audio_playback_instance_ctrl_t * const p_ctrl,
                                                sf_message_event_t                        event);

static void sf_audio_playback_max_samples_get (sf_audio_playback_common_instance_ctrl_t * const p_ctrl,
                                               uint32_t                                 * const p_num_samples);

static bool sf_audio_playback_mix_streams (sf_audio_playback_common_instance_ctrl_t * const p_ctrl,
                                           uint32_t                                 * const p_num_samples,
                                           int16_t                           const **       pp_samples_to_play,
                                           sf_audio_playback_data_type_t            *       p_type,
                                           q15_t                                    *       p_dest);

static void sf_audio_playback_shift_and_scale (sf_audio_playback_common_instance_ctrl_t * const p_ctrl,
                                               uint32_t                                 * const p_num_samples,
                                               int16_t                           const **       pp_samples_to_play,
                                               sf_audio_playback_data_type_t            * const p_type,
                                               bool                                     * const p_buffer_index_updated);

static ssp_err_t sf_audio_playback_start (sf_audio_playback_common_instance_ctrl_t * const p_ctrl,
                                          int16_t  const * const                           p_samples_to_play,
                                          uint32_t const                                   num_transfers);

static ssp_err_t sf_audio_playback_message_parse (sf_audio_playback_common_instance_ctrl_t * const p_ctrl, sf_message_header_t * const p_data);

static void sf_audio_playback_thread  (ULONG thread_input);

static void sf_audio_playback_consume_all_messages (sf_audio_playback_common_instance_ctrl_t * const p_ctrl,
                                                    ULONG                                    *       p_wait_option);

static void sf_audio_playback_status_update (sf_audio_playback_common_instance_ctrl_t * const p_ctrl, ULONG * p_wait_option);

static void sf_audio_playback_stream_timeout_update (sf_audio_playback_common_instance_ctrl_t * const p_ctrl);

#if SF_AUDIO_PLAYBACK_CFG_PARAM_CHECKING_ENABLE
static ssp_err_t sf_audio_playback_open_param_check (sf_audio_playback_instance_ctrl_t * const p_ctrl,
                                                     sf_audio_playback_cfg_t     const * const p_cfg);

static ssp_err_t sf_audio_playback_common_open_param_check (sf_audio_playback_common_cfg_t     const * const p_cfg);
#endif

static void sf_audio_playback_message_release (sf_audio_playback_common_instance_ctrl_t * const p_ctrl, uint32_t i);

static void sf_audio_playback_mem_align (q15_t * p_source, q15_t * p_destination, uint32_t  num_samples);

static void sf_audio_playback_bit_depth_match (sf_audio_playback_common_instance_ctrl_t * const p_ctrl,
                                               sf_audio_playback_data_type_t            * const p_type,
                                               q15_t                                    * p_src_bit,
                                               q15_t                                    * p_dest_bit,
                                               uint32_t                                   samples_bit);

static void sf_audio_playback_signed_data_match (sf_audio_playback_common_instance_ctrl_t * const p_ctrl,
                                                 q15_t                                    * p_src_data,
                                                 q15_t                                    * p_dest_data,
                                                 uint32_t                                 num_samples_data);

static void sf_audio_playback_apply_volume (sf_audio_playback_common_instance_ctrl_t  * const p_ctrl,
                                            sf_audio_playback_data_type_t             * const p_type,
                                            uint32_t                                  * const  num_transfers,
                                            q15_t                                     * p_src,
                                            q15_t                                     * p_dest);

static void sf_audio_playback_samples_limit_call (sf_audio_playback_common_instance_ctrl_t * const p_ctrl,
                                                  sf_audio_playback_data_type_t            * const p_type,
                                                  uint32_t                                 * const p_num_samples);

static void sf_audio_playback_index_update (sf_audio_playback_instance_ctrl_t        * const p_stream,
                                            sf_audio_playback_data_type_t            * const p_type,
                                            q7_t                                     ** pp_src_data);

static void sf_audio_playback_store_stream_pointers (sf_audio_playback_instance_ctrl_t * const p_ctrl,
                                                     sf_audio_playback_cfg_t     const * const p_cfg,
                                                     bool                              * found);

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/
#if defined(__GNUC__)
/* This structure is affected by warnings from a GCC compiler bug. This pragma suppresses the warnings in this
 * structure only.*/
/*LDRA_INSPECTED 69 S */
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
/** Version data structure used by error logger macro. */
static const ssp_version_t g_sf_audio_playback_version =
{
    .api_version_major  = SF_AUDIO_PLAYBACK_API_VERSION_MAJOR,
    .code_version_minor = SF_AUDIO_PLAYBACK_CODE_VERSION_MINOR,
    .api_version_minor  = SF_AUDIO_PLAYBACK_API_VERSION_MINOR,
    .code_version_major = SF_AUDIO_PLAYBACK_CODE_VERSION_MAJOR,
};
#if defined(__GNUC__)
/* Restore warning settings for 'missing-field-initializers' to as specified on command line. */
/*LDRA_INSPECTED 69 S */
#pragma GCC diagnostic pop
#endif

/*LDRA_NOANALYSIS LDRA_INSPECTED below not working. */
/*LDRA_INSPECTED 27 D This structure must be accessible in user code. It cannot be static. */
const sf_audio_playback_api_t g_sf_audio_playback_on_sf_audio_playback =
{
    .open            = SF_AUDIO_PLAYBACK_Open,
    .close           = SF_AUDIO_PLAYBACK_Close,
    .start           = SF_AUDIO_PLAYBACK_Start,
    .pause           = SF_AUDIO_PLAYBACK_Pause,
    .stop            = SF_AUDIO_PLAYBACK_Stop,
    .resume          = SF_AUDIO_PLAYBACK_Resume,
    .volumeSet       = SF_AUDIO_PLAYBACK_VolumeSet,
    .versionGet      = SF_AUDIO_PLAYBACK_VersionGet,
};
/*LDRA_ANALYSIS */

/** Name of module used by error logger macro */
#if BSP_CFG_ERROR_LOG != 0
static const char          g_module_name[] = "sf_audio_playback";
#endif

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @addtogroup SF_AUDIO_PLAYBACK
 * @{
 **********************************************************************************************************************/

/******************************************************************************************************************//**
 * Implements sf_audio_playback_api_t::open.
 *
 * @retval  SSP_SUCCESS            Audio hardware successfully configured.
 * @retval  SSP_ERR_ASSERTION      A pointer is NULL or a parameter is invalid.
 * @retval  SSP_ERR_OUT_OF_MEMORY  The number of streams open at once is limited to SF_AUDIO_PLAYBACK_CFG_MAX_STREAMS.
 *                                 If this number is exceeded, an out of memory error occurs.
 * @retval  SSP_ERR_INTERNAL       An internal ThreadX error has occurred. This is typically a failure to create/use
 *                                 a mutex.
 * @return                         See @ref Common_Error_Codes or lower level drivers for other possible return codes.
 * @note This function is not reentrant.
 **********************************************************************************************************************/
ssp_err_t SF_AUDIO_PLAYBACK_Open (sf_audio_playback_ctrl_t * const p_api_ctrl, sf_audio_playback_cfg_t const * const p_cfg)
{
    sf_audio_playback_instance_ctrl_t * p_ctrl = (sf_audio_playback_instance_ctrl_t *) p_api_ctrl;

    ssp_err_t err;
    UINT status = TX_MUTEX_ERROR;
#if SF_AUDIO_PLAYBACK_CFG_PARAM_CHECKING_ENABLE
    err = sf_audio_playback_open_param_check(p_ctrl, p_cfg);
    SF_AUDIO_PLAYBACK_ERROR_RETURN(SSP_SUCCESS == err, err);
#endif

    /** Open hardware if it is not already open. */
    p_ctrl->p_common_ctrl = p_cfg->p_common_ctrl;
    while (TX_SUCCESS != status)
    {
        /** Enter a critical section before checking the common instance mutex status. */
        SSP_CRITICAL_SECTION_DEFINE;
        SSP_CRITICAL_SECTION_ENTER;

        /** Check if common instance mutex is already created. If not then create the mutex. */
        if (TX_MUTEX_ID != p_ctrl->p_common_ctrl->common_instance_mutex.tx_mutex_id)
        {
            /** Create common_instance_mutex to protect common initialization,
             *  including initialization of shared event flags, audio playback thread and lower level hardware. */
            status = tx_mutex_create(&p_ctrl->p_common_ctrl->common_instance_mutex, NULL, TX_INHERIT);
            if (TX_SUCCESS != status)
            {
                /** If mutex create fails, return error. */
                SSP_ERROR_LOG(SSP_ERR_INTERNAL, &g_module_name[0], &g_sf_audio_playback_version);
                SSP_CRITICAL_SECTION_EXIT;
                return SSP_ERR_INTERNAL;
            }
        }
        /** Exit critical section */
        SSP_CRITICAL_SECTION_EXIT;

        /** Acquire the mutex before accessing the shared resource.
         *  Try again if the mutex was deleted in close. */
        status = tx_mutex_get(&p_ctrl->p_common_ctrl->common_instance_mutex, TX_WAIT_FOREVER);
    }

    if (OPEN != p_ctrl->p_common_ctrl->open)
    {
        /** Create event flags to notify playback thread when playback of a buffer is complete. */
        err = sf_audio_playback_common_open(p_ctrl->p_common_ctrl, p_cfg->p_common_cfg);
        SF_AUDIO_PLAYBACK_ERROR_RETURN(((SSP_SUCCESS == err) || (SSP_ERR_NOT_ENABLED == err)), err);
    }

    /** Store stream pointer in common control block. */
    bool found = false;
    sf_audio_playback_store_stream_pointers(p_ctrl, p_cfg, &found);

    /** Release the mutex. */
    /*  The return code is not checked here because tx_mutex_put cannot fail when called with
     *  a mutex owned by the current thread. The mutex is owned by the current thread because this call follows
     *  a successful call to tx_mutex_get. */
    tx_mutex_put(&p_ctrl->p_common_ctrl->common_instance_mutex);

    if (!found)
    {
        /*  The return codes are not checked because tx_mutex_delete cannot fail
         *  when called with successfully created RTOS objects. */
        tx_mutex_delete(&(p_ctrl->p_common_ctrl->common_instance_mutex));
        return SSP_ERR_OUT_OF_MEMORY;
    }

    /** Mark stream opened so it can be used by other API's. */
    p_ctrl->open = OPEN;

    return SSP_SUCCESS;
}  /* End of function SF_AUDIO_PLAYBACK_Open */

/******************************************************************************************************************//**
 * Implements sf_audio_playback_api_t::close.
 *
 * @retval  SSP_SUCCESS          Audio instance successfully closed
 * @retval  SSP_ERR_ASSERTION    p_ctrl is NULL
 * @retval  SSP_ERR_NOT_OPEN     The stream control block p_ctrl is not initialized.
 * @return                       See @ref Common_Error_Codes or lower level drivers for other possible return codes.
 * @note This function is not reentrant.
 **********************************************************************************************************************/
ssp_err_t SF_AUDIO_PLAYBACK_Close (sf_audio_playback_ctrl_t * const p_api_ctrl)
{
    sf_audio_playback_instance_ctrl_t * p_ctrl = (sf_audio_playback_instance_ctrl_t *) p_api_ctrl;

#if SF_AUDIO_PLAYBACK_CFG_PARAM_CHECKING_ENABLE
    SSP_ASSERT(NULL != p_ctrl);
    SF_AUDIO_PLAYBACK_ERROR_RETURN(OPEN == p_ctrl->open, SSP_ERR_NOT_OPEN);
#endif

    /** Mark stream as unused in hardware control block and determine if all streams are closed. */
    bool close_hardware = true;

    /* The return codes are not checked here because Close API should not return an error except for
     * parameter checking */
    tx_mutex_get(&p_ctrl->p_common_ctrl->common_instance_mutex, TX_WAIT_FOREVER);

    for (uint32_t i = 0U; i < SF_AUDIO_PLAYBACK_CFG_MAX_STREAMS; i++)
    {
        if (p_ctrl == p_ctrl->p_common_ctrl->p_stream[i])
        {
            /* Release all messages in the stream. */
            for (uint32_t j = 0U; j < 2U; j++)
            {
                if (NULL != p_ctrl->p_common_ctrl->p_stream[i]->p_data[j])
                {
                    p_ctrl->p_common_ctrl->p_message->p_api->bufferRelease(p_ctrl->p_common_ctrl->p_message->p_ctrl,
                            (sf_message_header_t *) p_ctrl->p_common_ctrl->p_stream[i]->p_data[j], SF_MESSAGE_RELEASE_OPTION_NAK);
                    p_ctrl->p_common_ctrl->p_stream[i]->p_data[j] = NULL;
                }
            }

            /* This is the current stream, clear the pointer. */
            p_ctrl->p_common_ctrl->p_stream[i] = NULL;
        }
        else if (NULL != p_ctrl->p_common_ctrl->p_stream[i])
        {
            /* Another stream is open. */
            close_hardware = false;
        }
        else
        {
            /* Stream is unused. Do nothing. */
        }
    }

    if (close_hardware)
    {
        /** Mark hardware control block as unused so it can be reconfigured. */
        p_ctrl->p_common_ctrl->open = 0U;

        /** Close lower level drivers */
        /* The return codes are not checked here because Close API should not return an error except for
         * parameter checking */
        p_ctrl->p_common_ctrl->p_lower_lvl_hw->p_api->close(p_ctrl->p_common_ctrl->p_lower_lvl_hw->p_ctrl);

        /** Delete RTOS services used */
        tx_event_flags_delete(&(p_ctrl->p_common_ctrl->flags));
        tx_thread_terminate(&(p_ctrl->p_common_ctrl->thread));
        tx_thread_delete(&(p_ctrl->p_common_ctrl->thread));
        tx_mutex_delete(&(p_ctrl->p_common_ctrl->common_instance_mutex));
    }
    else
    {
        /* The return codes are not checked here because Close API should not return an error except for
         * parameter checking */
        tx_mutex_put(&p_ctrl->p_common_ctrl->common_instance_mutex);
    }

    /** Mark control block as unused so it can be reconfigured. */
    p_ctrl->open = 0U;

    return SSP_SUCCESS;
}  /* End of function SF_AUDIO_PLAYBACK_Close */

/******************************************************************************************************************//**
 * Implements sf_audio_playback_api_t::start.
 *
 * @retval  SSP_SUCCESS          Buffer successfully sent to audio playback thread
 * @retval  SSP_ERR_ASSERTION    p_ctrl is NULL
 * @retval  SSP_ERR_NOT_OPEN     The stream control block p_ctrl is not initialized.
 * @note This function is reentrant.
 **********************************************************************************************************************/
ssp_err_t SF_AUDIO_PLAYBACK_Start  (sf_audio_playback_ctrl_t       * const p_api_ctrl,
                                    sf_audio_playback_data_t       * const p_data,
                                    UINT const                             timeout)
{
    sf_audio_playback_instance_ctrl_t * p_ctrl = (sf_audio_playback_instance_ctrl_t *) p_api_ctrl;

#if SF_AUDIO_PLAYBACK_CFG_PARAM_CHECKING_ENABLE
    SSP_ASSERT(NULL != p_ctrl);
    SSP_ASSERT(NULL != p_data);
    SSP_ASSERT(NULL != p_data->p_data);
    SSP_ASSERT(0U != p_data->size_bytes);
    SF_AUDIO_PLAYBACK_ERROR_RETURN(OPEN == p_ctrl->open, SSP_ERR_NOT_OPEN);
#endif

    /** Ensure that audio data is only posted from the current stream owner. */
    TX_THREAD * p_stream = tx_thread_identify();
    if (p_ctrl->p_owner != p_stream)
    {
        if (NULL == p_ctrl->p_owner)
        {
            /** Store new stream owner if stream is unowned. */
            p_ctrl->p_owner = p_stream;
        }
        else
        {
            /* Release buffer. */
            p_ctrl->p_common_ctrl->p_message->p_api->bufferRelease(p_ctrl->p_common_ctrl->p_message->p_ctrl, (sf_message_header_t *) p_data,
                    SF_MESSAGE_RELEASE_OPTION_NAK);
            SF_AUDIO_PLAYBACK_ERROR_LOG(SSP_ERR_IN_USE);
            return SSP_ERR_IN_USE;
        }
    }

    /** Set message header to audio start event.  Set instance to stream instance. */
    p_data->header.event_b.class_code = (uint32_t) SF_MESSAGE_EVENT_CLASS_AUDIO;
    p_data->header.event_b.code = (uint32_t) SF_MESSAGE_EVENT_AUDIO_START;
    p_data->header.event_b.class_instance = p_ctrl->class_instance;

    /** Send message with audio data to audio thread. */
    sf_message_post_cfg_t post_cfg =
    {
        .priority = SF_MESSAGE_PRIORITY_NORMAL,
        .p_callback = p_ctrl->p_callback,
    };
    ssp_err_t err = p_ctrl->p_common_ctrl->p_message->p_api->post(p_ctrl->p_common_ctrl->p_message->p_ctrl, (sf_message_header_t *) p_data, &post_cfg, NULL, timeout);
    SF_AUDIO_PLAYBACK_ERROR_RETURN(SSP_SUCCESS == err, err);

    return SSP_SUCCESS;
}

/******************************************************************************************************************//**
 * Implements sf_audio_playback_api_t::pause.
 *
 * @retval  SSP_SUCCESS          Audio playback pause message sent to audio playback thread for this stream.
 * @retval  SSP_ERR_ASSERTION    p_ctrl is NULL
 * @retval  SSP_ERR_NOT_OPEN     The stream control block p_ctrl is not initialized.
 * @return                       See @ref Common_Error_Codes or lower level drivers for other possible return codes.
 * @note This function is reentrant.
 **********************************************************************************************************************/
ssp_err_t SF_AUDIO_PLAYBACK_Pause (sf_audio_playback_ctrl_t       * const p_api_ctrl)
{
    sf_audio_playback_instance_ctrl_t * p_ctrl = (sf_audio_playback_instance_ctrl_t *) p_api_ctrl;

#if SF_AUDIO_PLAYBACK_CFG_PARAM_CHECKING_ENABLE
    SSP_ASSERT(NULL != p_ctrl);
    SF_AUDIO_PLAYBACK_ERROR_RETURN(OPEN == p_ctrl->open, SSP_ERR_NOT_OPEN);
#endif
    /** Send message with pause event to audio thread. */
    ssp_err_t err = sf_audio_playback_message_send(p_ctrl, SF_MESSAGE_EVENT_AUDIO_PAUSE);
    SF_AUDIO_PLAYBACK_ERROR_RETURN(SSP_SUCCESS == err, err);

    return SSP_SUCCESS;
} /* End of function SF_AUDIO_PLAYBACK_Pause */


/******************************************************************************************************************//**
 * Implements sf_audio_playback_api_t::stop.
 *
 * @retval  SSP_SUCCESS          Audio playback stop message sent to audio playback thread for this stream.
 * @retval  SSP_ERR_ASSERTION    p_ctrl is NULL
 * @retval  SSP_ERR_NOT_OPEN     The stream control block p_ctrl is not initialized.
 * @return                       See @ref Common_Error_Codes or lower level drivers for other possible return codes.
 * @note This function is reentrant.
 **********************************************************************************************************************/
ssp_err_t SF_AUDIO_PLAYBACK_Stop (sf_audio_playback_ctrl_t       * const p_api_ctrl)
{
    sf_audio_playback_instance_ctrl_t * p_ctrl = (sf_audio_playback_instance_ctrl_t *) p_api_ctrl;

#if SF_AUDIO_PLAYBACK_CFG_PARAM_CHECKING_ENABLE
    SSP_ASSERT(NULL != p_ctrl);
    SF_AUDIO_PLAYBACK_ERROR_RETURN(OPEN == p_ctrl->open, SSP_ERR_NOT_OPEN);
#endif
    /** Send message with stop event to audio thread. */
    ssp_err_t err = sf_audio_playback_message_send(p_ctrl, SF_MESSAGE_EVENT_AUDIO_STOP);
    SF_AUDIO_PLAYBACK_ERROR_RETURN(SSP_SUCCESS == err, err);

    return SSP_SUCCESS;
} /* End of function SF_AUDIO_PLAYBACK_Pause */


/******************************************************************************************************************//**
 * Implements sf_audio_playback_api_t::resume.
 *
 * @retval  SSP_SUCCESS          Audio playback resume message sent to audio playback thread for this stream.
 * @retval  SSP_ERR_ASSERTION    p_ctrl is NULL
 * @retval  SSP_ERR_NOT_OPEN     The stream control block p_ctrl is not initialized.
 * @note This function is reentrant.
 **********************************************************************************************************************/
ssp_err_t SF_AUDIO_PLAYBACK_Resume (sf_audio_playback_ctrl_t       * const p_api_ctrl)
{
    sf_audio_playback_instance_ctrl_t * p_ctrl = (sf_audio_playback_instance_ctrl_t *) p_api_ctrl;

#if SF_AUDIO_PLAYBACK_CFG_PARAM_CHECKING_ENABLE
    SSP_ASSERT(NULL != p_ctrl);
    SF_AUDIO_PLAYBACK_ERROR_RETURN(OPEN == p_ctrl->open, SSP_ERR_NOT_OPEN);
#endif
    /** Send message with resume event to audio thread. */
    ssp_err_t err = sf_audio_playback_message_send(p_ctrl, SF_MESSAGE_EVENT_AUDIO_RESUME);
    SF_AUDIO_PLAYBACK_ERROR_RETURN(SSP_SUCCESS == err, err);

    return SSP_SUCCESS;
} /* End of function SF_AUDIO_PLAYBACK_Resume */

/******************************************************************************************************************//**
 * Implements sf_audio_playback_api_t::volumeSet.
 *
 * @retval  SSP_SUCCESS          Audio playback software volume level updated (applies to all streams).
 * @retval  SSP_ERR_ASSERTION    p_ctrl is NULL
 * @retval  SSP_ERR_NOT_OPEN     The stream control block p_ctrl is not initialized.
 *********************************************************************************************************************/
ssp_err_t SF_AUDIO_PLAYBACK_VolumeSet (sf_audio_playback_ctrl_t     * const p_api_ctrl,
                                       uint8_t                        const volume)
{
    sf_audio_playback_instance_ctrl_t * p_ctrl = (sf_audio_playback_instance_ctrl_t *) p_api_ctrl;

#if SF_AUDIO_PLAYBACK_CFG_PARAM_CHECKING_ENABLE
    SSP_ASSERT(NULL != p_ctrl);
    SF_AUDIO_PLAYBACK_ERROR_RETURN(OPEN == p_ctrl->open, SSP_ERR_NOT_OPEN);
#endif

    /** Update volume in control block. */
    p_ctrl->p_common_ctrl->volume = volume;

    return SSP_SUCCESS;
} /* End of function SF_AUDIO_PLAYBACK_VolumeSet */

/******************************************************************************************************************//**
 * Implements sf_audio_playback_api_t::versionGet.
 *
 * @retval SSP_SUCCESS           Version returned successfully.
 * @retval SSP_ERR_ASSERTION     Parameter p_version was null.
 *********************************************************************************************************************/
ssp_err_t SF_AUDIO_PLAYBACK_VersionGet (ssp_version_t * const p_version)
{
#if SF_AUDIO_PLAYBACK_CFG_PARAM_CHECKING_ENABLE
    SSP_ASSERT(NULL != p_version);
#endif

    p_version->version_id = g_sf_audio_playback_version.version_id;

    return SSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup SF_AUDIO_PLAYBACK)
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Opens audio common layer.
 *
 * @param[in,out]  p_ctrl             Common control block for this audio framework.
 * @param[in]      p_cfg              Common configuration structure.
 *
 * @retval         SSP_SUCCESS          Audio hardware layer successfully opened.
 * @retval         SSP_ERR_ASSERTION    A required pointer is NULL or a parameter is invalid.
 * @retval         SSP_ERR_INTERNAL     Error occurred in thread creation.
 * @return         See @ref Common_Error_Codes or lower level drivers for other possible return codes.
 **********************************************************************************************************************/
static ssp_err_t sf_audio_playback_common_open (sf_audio_playback_common_instance_ctrl_t * const p_ctrl,
                                                sf_audio_playback_common_cfg_t     const * const p_cfg)
{
    ssp_err_t err;
    UINT tx_err;
#if SF_AUDIO_PLAYBACK_CFG_PARAM_CHECKING_ENABLE
    err = sf_audio_playback_common_open_param_check(p_cfg);
    SF_AUDIO_PLAYBACK_ERROR_RETURN(SSP_SUCCESS == err, err);
#endif

    /** Store messaging control structure. */
    p_ctrl->p_message = p_cfg->p_message;

    /** For each stream, store queue pointers for audio data and clear stream owner pointers. */
    for (uint32_t i = 0U; i < SF_AUDIO_PLAYBACK_CFG_MAX_STREAMS; i++)
    {
        p_ctrl->p_stream[i] = NULL;
    }

    /** Create event flags for use with wait function. */
    tx_err = tx_event_flags_create(&p_ctrl->flags, NULL);
    if (TX_SUCCESS != tx_err)
    {
        /* The return codes are not checked because tx_mutex_delete
         * cannot fail when called with successfully created RTOS objects. */
        tx_mutex_delete(&p_ctrl->common_instance_mutex);
        return SSP_ERR_INTERNAL;
    }

    /** Open HAL drivers. */
    err = sf_audio_playback_hw_open(p_ctrl, p_cfg);
    if (SSP_SUCCESS != err)
    {
        /* The return codes are not checked because tx_mutex_delete and tx_event_flags_delete
         * cannot fail when called with successfully created RTOS objects. */
        tx_mutex_delete(&p_ctrl->common_instance_mutex);
        tx_event_flags_delete(&p_ctrl->flags);
        return err;
    }

    /** Initialize variables. */
    p_ctrl->volume = 255;
    p_ctrl->p_lower_lvl_hw = p_cfg->p_lower_lvl_hw;
    p_ctrl->p_queue = p_cfg->p_queue;
    p_ctrl->buffer_index = 0U;

    /** Create main audio thread. */
    tx_err = tx_thread_create(&p_ctrl->thread,
                              (CHAR *) "sf_audio_playback thread",
                              sf_audio_playback_thread,
                              (ULONG) p_ctrl,
                              &p_ctrl->stack,
                              SF_AUDIO_PLAYBACK_STACK_SIZE,
                              p_cfg->priority,
                              p_cfg->priority,
                              TX_NO_TIME_SLICE,
                              TX_AUTO_START);
    if (TX_SUCCESS != tx_err)
    {
        /* The return codes are not checked because tx_mutex_delete and tx_event_flags_delete
         * cannot fail when called with successfully created RTOS objects. */
        tx_mutex_delete(&p_ctrl->common_instance_mutex);
        tx_event_flags_delete(&p_ctrl->flags);
        return SSP_ERR_INTERNAL;
    }


    /** Set the restart flag to ensure the hardware is started. */
    /*  The return codes are not checked because tx_event_flags_set cannot fail
     *  when called with successfully created RTOS objects. */
    tx_event_flags_set(&p_ctrl->flags, SF_AUDIO_PLAYBACK_PRV_FLAG_RESTART, TX_OR);

    /** Mark control block open. */
    p_ctrl->open   = OPEN;

    return SSP_SUCCESS;
}  /* End of function sf_audio_playback_common_open */

/*******************************************************************************************************************//**
 * Initialize and store stream pointer in common control block.
 *
 * @param[in]  p_ctrl             Control block for this audio framework.
 * @param[in]  p_cfg              Stream configuration structure.
 * @param[in]  found              Indicates storing stream pointer is successful.
 **********************************************************************************************************************/
static void sf_audio_playback_store_stream_pointers (sf_audio_playback_instance_ctrl_t * const p_ctrl,
                                                     sf_audio_playback_cfg_t     const * const p_cfg,
                                                     bool                              * found)
{
    /** Initialize stream variables. */
    for (uint32_t i = 0U; i < 2U; i++)
    {
        p_ctrl->p_data[i] = NULL;
    }
    p_ctrl->end = 0U;
    p_ctrl->index = 0U;
    p_ctrl->samples_remaining = 0U;

    /** Store queue pointers for audio data and clear stream owner pointers. */
    p_ctrl->p_owner = NULL;
    p_ctrl->class_instance = p_cfg->class_instance;
    p_ctrl->status = SF_AUDIO_PLAYBACK_STATUS_STOPPED;
    p_ctrl->p_callback = p_cfg->p_callback;

    /** Store stream pointer in common control block. */
    for (uint32_t i = 0U; i < SF_AUDIO_PLAYBACK_CFG_MAX_STREAMS; i++)
    {
        if (NULL == p_ctrl->p_common_ctrl->p_stream[i])
        {
            p_ctrl->p_common_ctrl->p_stream[i] = p_ctrl;
            *found = true;
            break;
        }
    }
}

/*******************************************************************************************************************//**
 * Opens audio hardware layer.
 *
 * @param[in,out]  p_ctrl             Common control block for this audio framework.
 * @param[in]      p_cfg              Common configuration structure.
 *
 * @retval         SSP_SUCCESS          Audio hardware layer successfully opened.
 * @retval         SSP_ERR_ASSERTION    A required pointer is NULL or a parameter is invalid.
 * @return         See @ref Common_Error_Codes or lower level drivers for other possible return codes.
 **********************************************************************************************************************/
static ssp_err_t sf_audio_playback_hw_open (sf_audio_playback_common_instance_ctrl_t * const p_ctrl,
                                            sf_audio_playback_common_cfg_t     const * const p_cfg)
{
#if SF_AUDIO_PLAYBACK_CFG_PARAM_CHECKING_ENABLE
    /* The following APIs are required by the audio playback thread. */
    SSP_ASSERT(NULL != p_cfg->p_lower_lvl_hw->p_api->open);
    SSP_ASSERT(NULL != p_cfg->p_lower_lvl_hw->p_api->dataTypeGet);
    SSP_ASSERT(NULL != p_cfg->p_lower_lvl_hw->p_api->start);
    SSP_ASSERT(NULL != p_cfg->p_lower_lvl_hw->p_api->play);
    SSP_ASSERT(NULL != p_cfg->p_lower_lvl_hw->p_cfg);
#endif /* if SF_AUDIO_PLAYBACK_CFG_PARAM_CHECKING_ENABLE */

    /** Open lower level driver. */
    sf_audio_playback_hw_cfg_t cfg = *p_cfg->p_lower_lvl_hw->p_cfg;
    cfg.p_context = p_ctrl;
    cfg.p_callback = sf_audio_playback_callback;
    ssp_err_t err = p_cfg->p_lower_lvl_hw->p_api->open(p_cfg->p_lower_lvl_hw->p_ctrl, &cfg);
    SF_AUDIO_PLAYBACK_ERROR_RETURN(SSP_SUCCESS == err, err);

    /** Store data type. */
    p_cfg->p_lower_lvl_hw->p_api->dataTypeGet(p_cfg->p_lower_lvl_hw->p_ctrl, &p_ctrl->data_type);

    return SSP_SUCCESS;
}

/******************************************************************************************************************//**
 * The audio playback thread receives control messages from application threads and plays audio data on active streams.
 *
 * @param[in]  thread_input  Pointer to audio control block
 **********************************************************************************************************************/
static void sf_audio_playback_thread  (ULONG thread_input)
{
    /** Get audio control block */
    sf_audio_playback_common_instance_ctrl_t * p_ctrl = (sf_audio_playback_common_instance_ctrl_t *) thread_input;
    ssp_err_t err;

    while (1)
    {
        /** Only wait for data if nothing is playing. */
        ULONG wait_option = TX_NO_WAIT;
        sf_audio_playback_status_update(p_ctrl, &wait_option);

        /** Consume all messages in the audio queue. */
        sf_audio_playback_consume_all_messages(p_ctrl, &wait_option);

        if (NULL == p_ctrl->p_next_buffer)
        {
            sf_audio_playback_samples_prepare(p_ctrl);
        }

        /** Check event flag to see if playback must be restarted. */
        ULONG actual_flags = 0;
        UINT tx_err;
        tx_err = tx_event_flags_get(&p_ctrl->flags, (ULONG) SF_AUDIO_PLAYBACK_PRV_FLAG_RESTART, TX_OR_CLEAR, &actual_flags, TX_NO_WAIT);

        if (TX_SUCCESS == tx_err)
        {
            /** Clear playback complete flag. */
            /*  The return codes are not checked because tx_event_flags_get cannot fail
             *  when called with successfully created RTOS objects. */
            tx_event_flags_get(&p_ctrl->flags, SF_AUDIO_PLAYBACK_PRV_FLAG_PLAYBACK_COMPLETE, TX_OR_CLEAR, &actual_flags, TX_NO_WAIT);

            /** Start playback */
            err = sf_audio_playback_start(p_ctrl, p_ctrl->p_next_buffer, p_ctrl->next_length);
            if (SSP_SUCCESS != err)
            {
                /** Nothing to do.  Loop around. */
                SF_AUDIO_PLAYBACK_ERROR_LOG(err);
                continue;
            }
        }

        /** Check for timeout. */
        sf_audio_playback_stream_timeout_update(p_ctrl);

        /** Wait for playback complete event if requested. */
        ULONG timeout = TX_NO_WAIT;
        if (NULL != p_ctrl->p_next_buffer)
        {
            /** Only wait if enough data is ready for the next transfer */
            timeout = SF_AUDIO_PLAYBACK_PRV_PLAYBACK_TIMEOUT;
        }

        actual_flags = 0U;
        tx_err = tx_event_flags_get(&p_ctrl->flags, SF_AUDIO_PLAYBACK_PRV_FLAG_PLAYBACK_COMPLETE, TX_OR_CLEAR, &actual_flags, timeout);
        if ((timeout > 0U) && (TX_SUCCESS != tx_err))
        {
            SF_AUDIO_PLAYBACK_ERROR_LOG(SSP_ERR_TIMEOUT);
        }
    }
}  /* End of function sf_audio_playback_thread */

/*******************************************************************************************************************//**
 * @brief  Limit the number of samples to play.
 * @param[in,out]  p_num_transfers         Pointer to number of transfers
 * @param[in]      limit                   Number of transfers limit
 **********************************************************************************************************************/
static void sf_audio_playback_samples_limit(uint32_t * const p_num_transfers, uint32_t limit)
{
    if (*p_num_transfers > limit)
    {
        *p_num_transfers = limit;
    }
}

/*******************************************************************************************************************//**
 * Memory Alignment operation to store 8 bit data in 16-bit destination.
 *
 * @param[in]      num_samples             The number of samples to play.
 * @param[in,out]  p_source                Pointer to source buffer.
 * @param[in,out]  p_destination           Pointer to destination buffer.
 **********************************************************************************************************************/
static void sf_audio_playback_mem_align(q15_t * p_source, q15_t * p_destination, uint32_t num_samples)
{
    q7_t *p_tmp_src = (q7_t *)  (p_source + ((num_samples / 2)));
    q15_t *p_tmp_dst =(q15_t *) (p_destination + (num_samples));

    /* Data is starting at the end of the array so source samples are not overwritten before they are used if the source and destination point to the same address. */
    p_tmp_src--;
    p_tmp_dst--;

    /* Copy data from source to destination. */
    for (uint32_t j = num_samples; j > 0U; j--)
    {
        *p_tmp_dst = (q15_t) *p_tmp_src;
        p_tmp_dst--;
        p_tmp_src--;
    }
}

/*******************************************************************************************************************//**
 * Prepares audio samples for playback, including mixing multiple streams if applicable, shifting and scaling if
 * required, and software volume control.
 *
 * @param[in,out]  p_ctrl             Control block for this audio framework.
 **********************************************************************************************************************/
static void sf_audio_playback_samples_prepare (sf_audio_playback_common_instance_ctrl_t * const p_ctrl)
{
    /** Determine the maximum number of transfers. */
    uint32_t num_transfers = 0U;
    sf_audio_playback_max_samples_get(p_ctrl, &num_transfers);
    if (0U == num_transfers)
    {
        return;
    }
    sf_audio_playback_samples_limit(&num_transfers, SF_AUDIO_PLAYBACK_DTC_NORMAL_MAX_LENGTH);

    /** Calculate the audio samples to play by using saturating addition to combine data from all playing streams. */
    int16_t const * p_samples_to_play    = NULL;
    sf_audio_playback_data_type_t type   = {.scale_bits_max = 0U, .is_signed = false};
    q15_t * p_dest                       = &p_ctrl->samples[p_ctrl->buffer_index][0];
    bool buffer_index_updated            = false;

    buffer_index_updated = sf_audio_playback_mix_streams(p_ctrl, &num_transfers, &p_samples_to_play, &type, &p_dest[0]);

    /** Apply volume (if applicable). */
    q15_t * p_src = NULL;
    if (255U != p_ctrl->volume)
    {
        p_src = (q15_t *) p_samples_to_play;
        p_dest = (q15_t *) &p_ctrl->samples[p_ctrl->buffer_index];
        sf_audio_playback_apply_volume(p_ctrl, &type, &num_transfers, p_src, &p_dest[0]);
        p_samples_to_play    = &p_ctrl->samples[p_ctrl->buffer_index][0];
        buffer_index_updated = true;
    }

    /** Shift and scale samples to fix the hardware requirements. */
    sf_audio_playback_shift_and_scale(p_ctrl, &num_transfers, &p_samples_to_play, &type, &buffer_index_updated);

    /* Update sample pointer */
    if (buffer_index_updated)
    {
        p_ctrl->buffer_index = !p_ctrl->buffer_index;
    }

    /** Store next buffer. */
    p_ctrl->next_length   = num_transfers;
    p_ctrl->p_next_buffer = p_samples_to_play;

    /** Update variables */
    for (uint32_t i = 0U; i < SF_AUDIO_PLAYBACK_CFG_MAX_STREAMS; i++)
    {
        if (NULL != p_ctrl->p_stream[i])
        {

            if (SF_AUDIO_PLAYBACK_STATUS_PLAYING == p_ctrl->p_stream[i]->status)
            {
                p_ctrl->p_stream[i]->index += num_transfers;
                p_ctrl->p_stream[i]->samples_remaining -= num_transfers; // Samples remaining after next transfer
                if (0U == p_ctrl->p_stream[i]->samples_remaining)
                {
                    if (TX_NO_WAIT != p_ctrl->p_stream[i]->p_data[0]->loop_timeout)
                    {
                        /* Play again */
                        p_ctrl->p_stream[i]->index             = 0U;
                        p_ctrl->p_stream[i]->samples_remaining = p_ctrl->p_stream[i]->samples_total;
                    }
                    else
                    {
                        /** Release audio message if playback is complete. */
                        sf_audio_playback_message_release(p_ctrl, i);
                    }
                }
            }
        }
    }
}

/*******************************************************************************************************************//**
 * Shifts the data to match the bit depth of the hardware
 *
 * @param[in,out]  p_ctrl                  Control block for this audio framework.
 * @param[in,out]  samples_bit             The number of samples to play.
 * @param[in,out]  p_type                  Data Type of input audio.
 * @param[in,out]  p_src_bit               Pointer to source buffer.
 * @param[in,out]  p_dest_bit              Pointer to destination buffer.
 **********************************************************************************************************************/
static void sf_audio_playback_bit_depth_match(sf_audio_playback_common_instance_ctrl_t * const p_ctrl,
                                              sf_audio_playback_data_type_t             * const p_type,
                                              q15_t                                         * p_src_bit,
                                              q15_t                                        * p_dest_bit,
                                              uint32_t                                      samples_bit)
{
    if ((p_type->scale_bits_max <= 8U) && (p_ctrl->data_type.scale_bits_max > 8U))
    {
        /* Convert input 8-bit data words to 16-bit data words required by the hardware. */
        sf_audio_playback_mem_align(p_src_bit, p_dest_bit, samples_bit);
        p_src_bit = p_dest_bit;
    }
    int8_t shift_bits = (int8_t) ((int8_t) p_type->scale_bits_max - (int8_t) p_ctrl->data_type.scale_bits_max);
    if ((p_type->scale_bits_max > 8U) ||
            (p_ctrl->data_type.scale_bits_max > 8U))
    {
        arm_shift_q15(p_src_bit, (int8_t) (-shift_bits), p_dest_bit, samples_bit);
        /* Mem align to match 12/16 bit data to the underlying hardware. */
        if (p_ctrl->data_type.scale_bits_max <= 8U)
        {
            q15_t * p_tmp_src = p_dest_bit;
            q7_t * p_tmp_dst = (q7_t *) p_dest_bit;
            for (uint32_t j = samples_bit; j > 0U; j--)
            {
                *p_tmp_dst =  *(q7_t *) p_tmp_src;
                p_tmp_dst++;
                p_tmp_src++;
            }
        }
    }
    else
    {
        arm_shift_q7((q7_t *) p_src_bit, (int8_t) (-shift_bits), (q7_t *) p_dest_bit, samples_bit );
    }
}

/*******************************************************************************************************************//**
 * Shifts the data to match hardware requirement
 *
 * @param[in,out]  p_ctrl                  Control block for this audio framework.
 * @param[in,out]  num_samples_data        The number of samples to play.
 * @param[in,out]  p_src_data              Pointer to source buffer.
 * @param[in,out]  p_dest_data             Pointer to destination buffer.
 **********************************************************************************************************************/
static void sf_audio_playback_signed_data_match(sf_audio_playback_common_instance_ctrl_t * const p_ctrl,
                                                q15_t                                      * p_src_data,
                                                q15_t                                     * p_dest_data,
                                                uint32_t                               num_samples_data)
{
    if (!p_ctrl->data_type.is_signed)
    {

        /* Shift up to create unsigned data */
        if (p_ctrl->data_type.scale_bits_max > 8U)
        {
            arm_offset_q15(p_src_data, (q15_t) (1U << (p_ctrl->data_type.scale_bits_max - 1U)), p_dest_data, num_samples_data);
        }
        else
        {
            q7_t * p_tmp_src = (q7_t *) p_src_data;
            q7_t * p_tmp_dest = (q7_t *) p_dest_data;
            for (uint32_t i = 0U; i < num_samples_data; i++)
            {
                (*(p_tmp_dest)) = (q7_t) ((*p_tmp_src + (q7_t) ((uint8_t) 1U << (p_ctrl->data_type.scale_bits_max - 1U))));
                p_tmp_src++;
                p_tmp_dest++;
            }

        }
    }
    else
    {
        /* Shift down to create signed data for matching the hardware requirement. */
        for (uint32_t i = 0U; i < num_samples_data; i++)
        {
            *(p_dest_data + i) = (int16_t) (*(p_src_data + i) - (q15_t) (1U << (p_ctrl->data_type.scale_bits_max - 1U)));
        }
    }
}

/*******************************************************************************************************************//**
 * Apply volume if applicable and scale data.
 *
 * @param[in,out]  p_ctrl                  Control block for this audio framework.
 * @param[in,out]  p_type                  The audio data type required by the audio hardware.
 * @param[in,out]  p_num_transfers         Pointer to number of transfers.
 * @param[in,out]  p_src                   Pointer to source buffer.
 * @param[in,out]  p_dest                  Pointer to destination buffer.
 **********************************************************************************************************************/
static void sf_audio_playback_apply_volume (sf_audio_playback_common_instance_ctrl_t * const p_ctrl,
                                            sf_audio_playback_data_type_t            * const p_type,
                                            uint32_t                        * const p_num_transfers,
                                            q15_t                                           * p_src,
                                            q15_t                                          * p_dest)
{
    sf_audio_playback_samples_limit_call(p_ctrl, p_type, p_num_transfers);

    /* Scale samples using formula new_sample = old_sample * current_volume / max_volume. */
    /* Note: p_src cannot be const because the ARM DSP library functions do not accept const input parameters.
     * This section has been tested using data in flash to show that the ARM DSP libary functions do work on
     * data in flash, so the cast from const data (p_samples_to_play) to q15_t * (not const) is alright here.
     */

    q15_t scale = (q15_t) p_ctrl->volume;
    if (p_type->scale_bits_max <= 8U)
    {
        arm_scale_q7((q7_t *) p_src, (q7_t) scale, 7, (q7_t *) p_dest, *p_num_transfers);
    }
    else
    {
        arm_scale_q15(p_src, scale, 7, p_dest, *p_num_transfers);
    }
}

/*******************************************************************************************************************//**
 * The number of transfers limit parameter is decided based on the underlying hardware.
 *
 * @param[in,out]  p_ctrl                  Control block for this audio framework.
 * @param[in,out]  p_type                  The audio data type required by the audio hardware.
 * @param[in,out]  p_num_samples           The number of samples to play.
 **********************************************************************************************************************/
static void sf_audio_playback_samples_limit_call (sf_audio_playback_common_instance_ctrl_t * const p_ctrl,
                                                  sf_audio_playback_data_type_t            * const p_type,
                                                  uint32_t                                 * const p_num_samples )
{
    if ((p_ctrl->data_type.scale_bits_max <= 8U) && (p_type->scale_bits_max <= 8U))
    {
        sf_audio_playback_samples_limit(p_num_samples, (SF_AUDIO_PLAYBACK_CFG_BUFFER_SIZE_BYTES / sizeof(int8_t)));
    }
    else
    {
        sf_audio_playback_samples_limit(p_num_samples, (SF_AUDIO_PLAYBACK_CFG_BUFFER_SIZE_BYTES / sizeof(int16_t)));
    }
}

/*******************************************************************************************************************//**
 * Updates the index for next play of the buffer.
 *
 * @param[in]      p_stream                Indicates current stream control block.
 * @param[in]      p_type                  The audio data type required by the audio hardware.
 * @param[in,out]  pp_src_data             Pointer to pointer to source buffer.
 **********************************************************************************************************************/
static void sf_audio_playback_index_update (sf_audio_playback_instance_ctrl_t        * const p_stream,
                                            sf_audio_playback_data_type_t            * const p_type,
                                            q7_t                                     ** pp_src_data)
{
    if (p_type->scale_bits_max <= 8U)
    {
        *pp_src_data = (*pp_src_data) + (int32_t) (p_stream->index);
    }
    else
    {
        *pp_src_data = (*pp_src_data) + (int32_t) ((p_stream->index) * 2);
    }
}

/*******************************************************************************************************************//**
 * Shifts and scales audio samples to match hardware requirements.
 *
 * @param[in,out]  p_ctrl                  Control block for this audio framework.
 * @param[in,out]  p_num_samples           The number of samples to play.
 * @param[in,out]  pp_samples_to_play      Pointer to pointer to samples to play.
 * @param[in,out]  p_type                  The audio data type required by the audio hardware.
 * @param[in,out]  p_buffer_index_updated  Whether or not the buffer is used and the buffer index needs to be updated
 **********************************************************************************************************************/
static void sf_audio_playback_shift_and_scale (sf_audio_playback_common_instance_ctrl_t * const p_ctrl,
                                               uint32_t                                 * const p_num_samples,
                                               int16_t                           const **       pp_samples_to_play,
                                               sf_audio_playback_data_type_t            * const p_type,
                                               bool                                     * const p_buffer_index_updated)
{
    /** Convert samples (if applicable) */
    q15_t * p_src = NULL;
    q15_t * p_dest = NULL;

    if (p_ctrl->data_type.scale_bits_max != p_type->scale_bits_max)
    {
        if ((0U != p_ctrl->data_type.scale_bits_max) &&
            (p_ctrl->data_type.scale_bits_max <= SF_AUDIO_PLAYBACK_BITS_PER_SAMPLE))
        {
            sf_audio_playback_samples_limit_call(p_ctrl, p_type, p_num_samples);

            p_src = (q15_t *) *pp_samples_to_play;
            p_dest = (q15_t *) &p_ctrl->samples[p_ctrl->buffer_index];
            if (*p_buffer_index_updated)
            {
                /* Scaled data is already in the destination buffer. */
                p_src = p_dest;
            }

            /* Shift off unused LSB's.  Negative number used to right shift data. */
            sf_audio_playback_bit_depth_match(p_ctrl, p_type ,p_src, p_dest, *p_num_samples);

            *pp_samples_to_play    = &p_ctrl->samples[p_ctrl->buffer_index][0];
            *p_buffer_index_updated = true;
        }
    }

    /** Convert samples (if applicable) */
    if (p_ctrl->data_type.is_signed != p_type->is_signed)
    {
        sf_audio_playback_samples_limit_call(p_ctrl, p_type, p_num_samples);
        p_src = (q15_t *) *pp_samples_to_play;
        p_dest = (q15_t *) &p_ctrl->samples[p_ctrl->buffer_index];
        if (*p_buffer_index_updated)
        {
            /* Scaled data is already in the destination buffer. */
            p_src = p_dest;
        }

        sf_audio_playback_signed_data_match(p_ctrl, p_src, p_dest, *p_num_samples);

        *pp_samples_to_play    = &p_ctrl->samples[p_ctrl->buffer_index][0];
        *p_buffer_index_updated = true;
    }
}

/*******************************************************************************************************************//**
 * Iterates over all playing streams in the system and mixes the samples using saturating vector addition.
 *
 * @param[in,out]  p_ctrl                  Control block for this audio framework.
 * @param[in,out]  p_num_samples           The number of samples to play.
 * @param[in,out]  pp_samples_to_play      Pointer to pointer to samples to play.
 * @param[in,out]  p_type                  The audio data type required by the audio hardware.
 * @param[in,out]  p_dest                  Pointer to destination buffer
 *
 * @retval true     Returns true if multiple streams are mixed.
 * @retval false    Returns false if only one stream is playing.
 **********************************************************************************************************************/
static bool sf_audio_playback_mix_streams (sf_audio_playback_common_instance_ctrl_t * const p_ctrl,
                                           uint32_t                                 * const p_num_samples,
                                           int16_t                           const **       pp_samples_to_play,
                                           sf_audio_playback_data_type_t            *       p_type,
                                           q15_t                                    *       p_dest)
{
    /** Identify all streams that are playing. */
    q7_t * p_src_a                      = NULL;
    int16_t ** pp_samples_to_play_temp   = (int16_t **) pp_samples_to_play;
    bool ret = false;

    for (uint32_t i = 0U; i < SF_AUDIO_PLAYBACK_CFG_MAX_STREAMS; i++)
    {
        if (NULL != p_ctrl->p_stream[i])
        {
            if (SF_AUDIO_PLAYBACK_STATUS_PLAYING == p_ctrl->p_stream[i]->status)
            {
                if (NULL == p_src_a)
                {
                    /** This is the first stream, store the samples to play. */
                    *p_type = p_ctrl->p_stream[i]->p_data[0]->type;
                    p_src_a = (q7_t *) p_ctrl->p_stream[i]->p_data[0]->p_data;
                    sf_audio_playback_index_update(p_ctrl->p_stream[i], p_type, &p_src_a);
                    *pp_samples_to_play_temp = (int16_t *) p_src_a;
                }
                else if ((p_type->is_signed == true) && (p_ctrl->p_stream[i]->p_data[0]->type.is_signed == true)
                        && (p_type->scale_bits_max == p_ctrl->p_stream[i]->p_data[0]->type.scale_bits_max))
                {
                    /** If any other streams are active, use saturating vector addition to combine the samples
                     * with samples from the previous stream(s). */
                    ret = true;
                    sf_audio_playback_samples_limit_call(p_ctrl, p_type, p_num_samples);
                    q7_t * p_src_b = (q7_t *) p_ctrl->p_stream[i]->p_data[0]->p_data;
                    sf_audio_playback_index_update(p_ctrl->p_stream[i], p_type, &p_src_b);
                    if (p_ctrl->p_stream[i]->p_data[0]->type.scale_bits_max <= 8U)
                    {
                        /* Do 8 bit additions and mem align. */
                        arm_add_q7((q7_t *) p_src_a, (q7_t *) p_src_b, (q7_t *) p_dest, *p_num_samples);
                        /* Point p_src_a to sum so that it can be added with next stream. */
                        p_src_a = (q7_t *) p_dest;
                    }
                    else
                    {
                        arm_add_q15((q15_t *) p_src_a, (q15_t *) p_src_b, p_dest, *p_num_samples);

                        /* Point src to destination holding mixed data of previous streams. */
                        p_src_a = (q7_t *) p_dest;
                    }

                    *pp_samples_to_play_temp = (int16_t *) p_dest;
                }
                else
                {
                    /** Nothing to do. Loop Around */
                }
            }
        }
    }
    return ret;
}

/*******************************************************************************************************************//**
 * Checks all active streams to determine how many samples can be played at once.
 *
 * @param[in,out]  p_ctrl             Control block for this audio framework.
 * @param[in,out]  p_num_samples      The maximum number of samples that can be played at once.
 **********************************************************************************************************************/
static void sf_audio_playback_max_samples_get (sf_audio_playback_common_instance_ctrl_t * const p_ctrl,
                                               uint32_t                                 * const p_num_samples)
{
    for (uint32_t i = 0U; i < SF_AUDIO_PLAYBACK_CFG_MAX_STREAMS; i++)
    {
        if (NULL != p_ctrl->p_stream[i])
        {
            if (SF_AUDIO_PLAYBACK_STATUS_PLAYING == p_ctrl->p_stream[i]->status)
            {
                uint32_t stream_num_transfers = p_ctrl->p_stream[i]->samples_remaining;
                if ((0U == (*p_num_samples)) || (stream_num_transfers < (*p_num_samples)))
                {
                    *p_num_samples = stream_num_transfers;
                }
            }
        }
    }
}

/*******************************************************************************************************************//**
 * Starts audio hardware and begins playback.
 *
 * @param[in,out]  p_ctrl             Control block for this audio framework.
 * @param[in]      p_samples_to_play  First group of samples to play.
 * @param[in]      num_transfers      Number of samples in p_samples_to_play
 *
 * @retval SSP_SUCCESS           All hardware components were started successfully.  Playback has begun.
 * @return                       See @ref Common_Error_Codes or lower level drivers for other possible return codes.
 **********************************************************************************************************************/
static ssp_err_t sf_audio_playback_start (sf_audio_playback_common_instance_ctrl_t * const p_ctrl,
                                          int16_t                            const * const p_samples_to_play,
                                          uint32_t const                      num_transfers)
{
    if (num_transfers > 0U)
    {
        p_ctrl->p_next_buffer = NULL;
        p_ctrl->next_length   = 0U;

        /** Begin playback */
        ssp_err_t err = p_ctrl->p_lower_lvl_hw->p_api->play(p_ctrl->p_lower_lvl_hw->p_ctrl, p_samples_to_play, num_transfers);
        if (SSP_SUCCESS != err)
        {
            p_ctrl->p_next_buffer = p_samples_to_play;
            p_ctrl->next_length   = num_transfers;
        }
        SF_AUDIO_PLAYBACK_ERROR_RETURN(SSP_SUCCESS == err, err);

        /** Make sure the hardware is started */
        err = p_ctrl->p_lower_lvl_hw->p_api->start(p_ctrl->p_lower_lvl_hw->p_ctrl);
        SF_AUDIO_PLAYBACK_ERROR_RETURN(SSP_SUCCESS == err, err);
    }

    return SSP_SUCCESS;
} /* End of function sf_audio_playback_start */

/*******************************************************************************************************************//**
 * Sends message to audio playback thread.
 *
 * @param[in,out]  p_ctrl             Control block for this audio framework.
 * @param[in]      event              Event to post.
 *
 * @retval SSP_SUCCESS           All hardware components were started successfully.  Playback has begun.
 * @return                       See @ref Common_Error_Codes or lower level drivers for other possible return codes.
 **********************************************************************************************************************/
static ssp_err_t sf_audio_playback_message_send(sf_audio_playback_instance_ctrl_t * const p_ctrl,
                                                sf_message_event_t                        event)
{
    /** Acquire buffer to send pause message. */
    sf_message_header_t * p_data = NULL;
    sf_message_acquire_cfg_t acquire_cfg;
    acquire_cfg.buffer_keep = false;
    ssp_err_t err = p_ctrl->p_common_ctrl->p_message->p_api->bufferAcquire(p_ctrl->p_common_ctrl->p_message->p_ctrl, &p_data, &acquire_cfg, SF_AUDIO_PLAYBACK_PRV_PLAYBACK_TIMEOUT);
    SF_AUDIO_PLAYBACK_ERROR_RETURN(SSP_SUCCESS == err, err);

    /** Populate buffer with class and code. */
    p_data->event_b.class_code = (uint32_t) SF_MESSAGE_EVENT_CLASS_AUDIO;
    p_data->event_b.code = (uint32_t) event;
    p_data->event_b.class_instance = p_ctrl->class_instance;

    /** Send message to audio thread. */
    sf_message_post_cfg_t post_cfg = {(sf_message_priority_t) 0U};
    post_cfg.priority = SF_MESSAGE_PRIORITY_NORMAL;
    post_cfg.p_callback = NULL;
    err = p_ctrl->p_common_ctrl->p_message->p_api->post(p_ctrl->p_common_ctrl->p_message->p_ctrl, (sf_message_header_t *) p_data, &post_cfg, NULL, SF_AUDIO_PLAYBACK_PRV_PLAYBACK_TIMEOUT);
    SF_AUDIO_PLAYBACK_ERROR_RETURN(SSP_SUCCESS == err, err);

    return SSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Consumes an audio message by storing it in the control block for playback or updating the stream status and
 * releasing the message.
 *
 * @param[in]  p_ctrl    Common control block for this audio framework.
 * @param[in]  p_stream  Stream control block this message is directed to.
 * @param[in]  p_data    Audio message.
 **********************************************************************************************************************/
static void sf_audio_playback_message_consume (sf_audio_playback_common_instance_ctrl_t * const p_ctrl,
                                               sf_audio_playback_instance_ctrl_t        * const p_stream,
                                               sf_message_header_t                      * const p_data)
{
    switch (p_data->event_b.code)
    {
    case SF_MESSAGE_EVENT_AUDIO_START:
    {
        if (NULL == p_stream->p_data[0])
        {
            p_stream->p_data[0] = (sf_audio_playback_data_t *) p_data;
            p_stream->status = SF_AUDIO_PLAYBACK_STATUS_PLAYING;

            /* Save timeout value */
            p_stream->end = (uint32_t) tx_time_get() + p_stream->p_data[0]->loop_timeout;

            /* Initialize state values for the stream. */
            if (p_stream->p_data[0]->type.scale_bits_max <= 8U)
            {
                p_stream->samples_total = p_stream->p_data[0]->size_bytes;
            }
            else
            {
                p_stream->samples_total = p_stream->p_data[0]->size_bytes / sizeof(uint16_t);
            }

            p_stream->samples_remaining = p_stream->samples_total;
            p_stream->index = 0U;
        }
        else if (NULL == p_stream->p_data[1])
        {
            p_stream->p_data[1] = (sf_audio_playback_data_t *) p_data;
        }
        else
        {
            /* Message dropped. Release message. */
            p_ctrl->p_message->p_api->bufferRelease(p_ctrl->p_message->p_ctrl, p_data,
                    SF_MESSAGE_RELEASE_OPTION_NAK);
        }
        break;
    }
    case SF_MESSAGE_EVENT_AUDIO_PAUSE:
    {
        p_stream->status = SF_AUDIO_PLAYBACK_STATUS_PAUSED;

        /*  Release message. */
        p_ctrl->p_message->p_api->bufferRelease(p_ctrl->p_message->p_ctrl, p_data,
                SF_MESSAGE_RELEASE_OPTION_ACK);
        break;
    }
    case SF_MESSAGE_EVENT_AUDIO_RESUME:
    {
        p_stream->status = SF_AUDIO_PLAYBACK_STATUS_PLAYING;

        /* Set event flag to notify play loop that playback must be restarted. */
        /*  The return codes are not checked because tx_event_flags_set cannot fail
         *  when called with successfully created RTOS objects. */
        tx_event_flags_set(&p_ctrl->flags, SF_AUDIO_PLAYBACK_PRV_FLAG_RESTART, TX_OR);

        /*  Release message. */
        p_ctrl->p_message->p_api->bufferRelease(p_ctrl->p_message->p_ctrl, p_data,
                SF_MESSAGE_RELEASE_OPTION_ACK);
        break;
    }
    case SF_MESSAGE_EVENT_AUDIO_STOP:
    {
        p_stream->status = SF_AUDIO_PLAYBACK_STATUS_STOPPED;

        /* Release buffers. */
        if (NULL != p_stream->p_data[0])
        {
            p_ctrl->p_message->p_api->bufferRelease(p_ctrl->p_message->p_ctrl, (sf_message_header_t *) p_stream->p_data[0],
                    SF_MESSAGE_RELEASE_OPTION_ACK);
            p_stream->p_data[0] = NULL;
        }
        if (NULL != p_stream->p_data[1])
        {
            p_ctrl->p_message->p_api->bufferRelease(p_ctrl->p_message->p_ctrl, (sf_message_header_t *) p_stream->p_data[1],
                    SF_MESSAGE_RELEASE_OPTION_ACK);
            p_stream->p_data[1] = NULL;
        }

        /*  Release message. */
        p_ctrl->p_message->p_api->bufferRelease(p_ctrl->p_message->p_ctrl, p_data,
                SF_MESSAGE_RELEASE_OPTION_ACK);
        break;
    }
    default:
    {
        /* Unknown message event.  Release message. */
        p_ctrl->p_message->p_api->bufferRelease(p_ctrl->p_message->p_ctrl, p_data,
                SF_MESSAGE_RELEASE_OPTION_NAK);
        break;
    }
    }
}

/*******************************************************************************************************************//**
 * Waits for event flags and responds according to the event received.
 *
 * @param[in]  p_ctrl Control block for this audio framework.
 * @param[in]  p_data  Pointer to message header
 *
 * @retval SSP_SUCCESS           All event flags were processed successfully.  There is nothing in p_ctrl->p_next_buffer
 *                               and the next samples can be calculated.
 * @retval SSP_ERR_TIMEOUT       A playback complete event was expected, but it was not received in time.
 * @retval SSP_ERR_WAIT_ABORTED  An unspecified (non-timeout) error occurred while getting the event flag.
 **********************************************************************************************************************/
static ssp_err_t sf_audio_playback_message_parse (sf_audio_playback_common_instance_ctrl_t * const p_ctrl, sf_message_header_t * const p_data)
{
    /** First, find the stream this message is intended for. */
    bool found = false;
    if (SF_MESSAGE_EVENT_CLASS_AUDIO == p_data->event_b.class_code)
    {
        for (uint32_t i = 0U; i < SF_AUDIO_PLAYBACK_CFG_MAX_STREAMS; i++)
        {
            if (NULL != p_ctrl->p_stream[i])
            {
                if (p_ctrl->p_stream[i]->class_instance == p_data->event_b.class_instance)
                {
                    /** Respond to the message. */
                    found = true;
                    sf_audio_playback_message_consume(p_ctrl, p_ctrl->p_stream[i], p_data);
                }
            }
        }
    }
    if (!found)
    {
        /* Unknown message event.  Release message. */
        p_ctrl->p_message->p_api->bufferRelease(p_ctrl->p_message->p_ctrl, p_data,
                SF_MESSAGE_RELEASE_OPTION_NAK);
    }

    return SSP_SUCCESS;
} /* End of function sf_audio_playback_flags_parse */

/*******************************************************************************************************************//**
 * Releases message buffer and updates control block data when playback is complete.
 *
 * @param[in]  p_ctrl Control block for this audio framework.
 * @param[in]  i      Index of stream control block to update.
 **********************************************************************************************************************/
static void sf_audio_playback_message_release (sf_audio_playback_common_instance_ctrl_t * const p_ctrl, uint32_t i)
{
    if (p_ctrl->p_stream[i]->p_data[0]->stream_end)
    {
        p_ctrl->p_stream[i]->status  = SF_AUDIO_PLAYBACK_STATUS_STOPPED;
        p_ctrl->p_stream[i]->p_owner = NULL;
    }
    else
    {
        if (NULL == p_ctrl->p_stream[i]->p_data[1])
        {
            /* Playback of current data packet is complete.  Mark stream waiting to receive next packet from queue. */
            p_ctrl->p_stream[i]->status = SF_AUDIO_PLAYBACK_STATUS_WAITING;
        }
    }

    /* Release buffer. */
    p_ctrl->p_message->p_api->bufferRelease(p_ctrl->p_message->p_ctrl, (sf_message_header_t *) p_ctrl->p_stream[i]->p_data[0],
            SF_MESSAGE_RELEASE_OPTION_ACK);
    p_ctrl->p_stream[i]->p_data[0] = p_ctrl->p_stream[i]->p_data[1];
    p_ctrl->p_stream[i]->p_data[1] = NULL;

    if (NULL != p_ctrl->p_stream[i]->p_data[0])
    {
        /* Save timeout value */
        p_ctrl->p_stream[i]->end = (uint32_t) tx_time_get() + p_ctrl->p_stream[i]->p_data[0]->loop_timeout;

        /* Initialize state values for the stream. */
        p_ctrl->p_stream[i]->samples_remaining = p_ctrl->p_stream[i]->samples_total;
        p_ctrl->p_stream[i]->index = 0U;
    }
}

/******************************************************************************************************************//**
 * Receives and parses all messages in the audio playback queue.
 *
 * @param[in,out]  p_ctrl            The audio common control block.
 * @param[in,out]  p_wait_option     How long to wait for the first message to arrive.
 **********************************************************************************************************************/
static void sf_audio_playback_consume_all_messages (sf_audio_playback_common_instance_ctrl_t * const p_ctrl,
                                                    ULONG                                    *       p_wait_option)
{
    ssp_err_t err = SSP_SUCCESS;
    while (SSP_SUCCESS == err)
    {
        if (TX_QUEUE_ID != p_ctrl->p_queue->tx_queue_id)
        {
            /* The queue isn't created.  Wait, then loop back and check again. */
            tx_thread_sleep(SF_AUDIO_PLAYBACK_PRV_WAIT);
        }
        else
        {
            /** Receive message. */
            sf_message_header_t * p_data = NULL;
            err = p_ctrl->p_message->p_api->pend(p_ctrl->p_message->p_ctrl, p_ctrl->p_queue, &p_data, *p_wait_option);

            if (SSP_SUCCESS == err)
            {
                /** Parse message. */
                sf_audio_playback_message_parse(p_ctrl, p_data);
                *p_wait_option = TX_NO_WAIT;
            }
        }
    }
}

/******************************************************************************************************************//**
 * Sets a playing flag in the control block to true if any stream is playing or false if no streams are playing.
 *
 * @param[in,out]  p_ctrl            The audio common control block.
 * @param[in,out]  p_wait_option     Timeout for pending on message stored here.
 **********************************************************************************************************************/
static void sf_audio_playback_status_update (sf_audio_playback_common_instance_ctrl_t * const p_ctrl, ULONG * p_wait_option)
{
    ULONG wait_option = TX_WAIT_FOREVER;
    bool all_streams_stopped = true;
    for (uint32_t i = 0U; i < SF_AUDIO_PLAYBACK_CFG_MAX_STREAMS; i++)
    {
        if (NULL != p_ctrl->p_stream[i])
        {
            if (SF_AUDIO_PLAYBACK_STATUS_STOPPED != p_ctrl->p_stream[i]->status)
            {
                all_streams_stopped = false;

                if (SF_AUDIO_PLAYBACK_STATUS_PAUSED != p_ctrl->p_stream[i]->status)
                {
                    if (NULL != p_ctrl->p_stream[i]->p_data[0])
                    {
                        wait_option = TX_NO_WAIT;
                    }
                }
            }
        }
    }
    if (all_streams_stopped)
    {
        /** Clear the stored buffer if no streams are active. */
        p_ctrl->p_next_buffer = NULL;
        p_ctrl->next_length = 0U;
    }
    *p_wait_option = wait_option;
}

/******************************************************************************************************************//**
 * Iterates over all active streams in the system and checks to see if a timeout occurred.  If a timeout has occurred,
 * the stream status is updated.
 *
 * @param[in,out]  p_ctrl            The audio common control block.
 **********************************************************************************************************************/
static void sf_audio_playback_stream_timeout_update (sf_audio_playback_common_instance_ctrl_t * const p_ctrl)
{
    ULONG time_now = tx_time_get();
    for (uint32_t i = 0U; i < SF_AUDIO_PLAYBACK_CFG_MAX_STREAMS; i++)
    {
        if (NULL != p_ctrl->p_stream[i])
        {
            if ((SF_AUDIO_PLAYBACK_STATUS_STOPPED != p_ctrl->p_stream[i]->status) &&
                (SF_AUDIO_PLAYBACK_STATUS_WAITING != p_ctrl->p_stream[i]->status))
            {
                if ((TX_WAIT_FOREVER != p_ctrl->p_stream[i]->p_data[0]->loop_timeout) &&
                        (TX_NO_WAIT != p_ctrl->p_stream[i]->p_data[0]->loop_timeout) &&
                        (time_now >= p_ctrl->p_stream[i]->end))
                {
                    /* Playback is complete after these samples.  Break loop. */
                    p_ctrl->p_stream[i]->samples_remaining = 0U;

                    sf_audio_playback_message_release(p_ctrl, i);
                }
            }
        }
    }
}

#if SF_AUDIO_PLAYBACK_CFG_PARAM_CHECKING_ENABLE
/******************************************************************************************************************//**
 * This function performs parameter checking for SF_AUDIO_PLAYBACK_Open.
 *
 * @param[in,out]  p_ctrl             Stream control block for this stream.
 * @param[in]      p_cfg              Stream configuration structure.
 *
 * @retval  SSP_SUCCESS            No parameter checking violations identified.
 * @retval  SSP_ERR_ASSERTION      A pointer is NULL or the control block is already in use.
 **********************************************************************************************************************/
static ssp_err_t sf_audio_playback_open_param_check (sf_audio_playback_instance_ctrl_t * const p_ctrl,
                                                     sf_audio_playback_cfg_t     const * const p_cfg)
{
    SSP_ASSERT(NULL != p_ctrl);
    SSP_ASSERT(NULL != p_cfg);
    SSP_ASSERT(NULL != p_cfg->p_common_ctrl);
    SF_AUDIO_PLAYBACK_ERROR_RETURN(OPEN != p_ctrl->open, SSP_ERR_IN_USE);

    return SSP_SUCCESS;
}

/******************************************************************************************************************//**
 * This function performs parameter checking for sf_audio_playback_common_open.
 *
 * @param[in]      p_cfg              Common configuration structure.
 *
 * @retval  SSP_SUCCESS            No parameter checking violations identified.
 * @retval  SSP_ERR_ASSERTION      A pointer is NULL or the thread priority is invalid.
 **********************************************************************************************************************/
static ssp_err_t sf_audio_playback_common_open_param_check (sf_audio_playback_common_cfg_t     const * const p_cfg)
{
    SSP_ASSERT(NULL != p_cfg);
    SSP_ASSERT(NULL != p_cfg->p_lower_lvl_hw);
    SSP_ASSERT(NULL != p_cfg->p_lower_lvl_hw->p_api);
    SSP_ASSERT(NULL != p_cfg->p_message);
    SSP_ASSERT(NULL != p_cfg->p_queue);
    SSP_ASSERT(p_cfg->priority < (UINT) TX_MAX_PRIORITIES);

    return SSP_SUCCESS;
}
#endif /* if SF_AUDIO_PLAYBACK_CFG_PARAM_CHECKING_ENABLE */

/*******************************************************************************************************************//**
 * Callback function intercepted from lower audio hardware layer.
 *
 * @param[in]  p_args   Callback arguments from lower layer.
 **********************************************************************************************************************/
static void sf_audio_playback_callback (sf_audio_playback_hw_callback_args_t * p_args)
{
    if (NULL != p_args)
    {
        /** Recover context from ISR. */
        sf_audio_playback_common_instance_ctrl_t * p_ctrl = (sf_audio_playback_common_instance_ctrl_t *) (p_args->p_context);

        if (NULL != p_ctrl)
        {
            /** Verify control block is valid */
            if (OPEN == p_ctrl->open)
            {
                if (((NULL == p_ctrl->p_next_buffer) || (0U == p_ctrl->next_length)) ||
                        (SF_AUDIO_PLAYBACK_HW_EVENT_PLAYBACK_COMPLETE != p_args->event))
                {
                    /** The next buffer is not ready, or an error occurred in the hardware. */

                    /** Stop the hardware. */
                    p_ctrl->p_lower_lvl_hw->p_api->stop(p_ctrl->p_lower_lvl_hw->p_ctrl);

                    /** Set event flag to notify play loop that playback must be restarted. */
                    ULONG flags = (ULONG) SF_AUDIO_PLAYBACK_PRV_FLAG_PLAYBACK_COMPLETE;
                    flags |= (ULONG) SF_AUDIO_PLAYBACK_PRV_FLAG_RESTART;
                    /*  The return codes are not checked because tx_event_flags_set cannot fail
                     *  when called with successfully created RTOS objects. */
                    tx_event_flags_set(&p_ctrl->flags, flags, TX_OR);
                }
                else
                {
                    /** Play next buffer */
                    ssp_err_t err = p_ctrl->p_lower_lvl_hw->p_api->play(p_ctrl->p_lower_lvl_hw->p_ctrl,
                            p_ctrl->p_next_buffer,
                            p_ctrl->next_length);

                    if (SSP_SUCCESS == err)
                    {
                        /** Clear data to stop playback if data isn't refreshed. */
                        p_ctrl->p_next_buffer = NULL;
                        p_ctrl->next_length   = 0U;

                        /** Set event flag to notify play loop that playback has completed. */
                        /*  The return codes are not checked because tx_event_flags_set cannot fail
                         *  when called with successfully created RTOS objects. */
                        tx_event_flags_set(&p_ctrl->flags, SF_AUDIO_PLAYBACK_PRV_FLAG_PLAYBACK_COMPLETE, TX_OR);
                    }
                    else
                    {
                        /** Lower level playback failed. */

                        /** Stop the hardware. */
                        p_ctrl->p_lower_lvl_hw->p_api->stop(p_ctrl->p_lower_lvl_hw->p_ctrl);

                        /** Set event flag to notify play loop that playback must be restarted. */
                        ULONG flags = (ULONG) SF_AUDIO_PLAYBACK_PRV_FLAG_PLAYBACK_COMPLETE;
                        flags |= (ULONG) SF_AUDIO_PLAYBACK_PRV_FLAG_RESTART;
                        /*  The return codes are not checked because tx_event_flags_set cannot fail
                         *  when called with successfully created RTOS objects. */
                        tx_event_flags_set(&p_ctrl->flags, flags, TX_OR);
                    }
                }
            }
        }
    }
} /* End of function sf_audio_playback_callback */

/* End of file */
