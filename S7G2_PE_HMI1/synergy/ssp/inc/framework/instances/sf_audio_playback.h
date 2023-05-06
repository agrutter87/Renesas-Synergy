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
* File Name    : sf_audio_playback.h
* Description  : Console AMS framework driver header file
***********************************************************************************************************************/

#ifndef SF_AUDIO_PLAYBACK_H
#define SF_AUDIO_PLAYBACK_H

/*******************************************************************************************************************//**
 * @ingroup SF_Library
 * @defgroup SF_AUDIO_PLAYBACK Audio Framework
 * @brief RTOS-integrated Audio Framework.
 *
 * @section SF_AUDIO_PLAYBACK_SUMMARY Summary
 * This module is a ThreadX-aware Audio Framework. The module implements @ref SF_AUDIO_PLAYBACK_API.
 *
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "bsp_api.h"

/* Includes driver interfaces. */
#include "sf_audio_playback_api.h"

/* Include configuration */
#include "sf_audio_playback_cfg.h"

/* Common macro for SSP header files. There is also a corresponding SSP_FOOTER macro at the end of this file. */
SSP_HEADER

/**********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/** Version of code that implements the API defined in this file */
#define SF_AUDIO_PLAYBACK_CODE_VERSION_MAJOR  (2U)
#define SF_AUDIO_PLAYBACK_CODE_VERSION_MINOR  (0U)

/** Audio playback internal thread stack size.  Varies by application, but rarely requires more than 256 bytes. */
#define SF_AUDIO_PLAYBACK_STACK_SIZE          (SF_AUDIO_PLAYBACK_CFG_THREAD_STACK_SIZE)

/**********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
/* Forward declaration of stream control block used in the hardware control block. */
typedef struct st_sf_audio_playback_instance_ctrl sf_audio_playback_instance_ctrl_t;

/** Audio common instance control block. DO NOT INITIALIZE.  Initialization the first time
 * sf_audio_playback_api_t::open is called.  Shared by all streams. */
typedef struct st_sf_audio_playback_common_instance_ctrl
{
    uint32_t        open;          ///< Used to determine if driver is initialized.
    void const *    p_next_buffer; ///< Pointer to next buffer (to be played when the current buffer completes).
    uint32_t        next_length;   ///< Length of next buffer (to be played when the current buffer completes).
    sf_message_instance_t const * p_message; ///< Pointer to message control block.
    TX_QUEUE *      p_queue;       ///< Queue subscribed to SF_MESSAGE_EVENT_CLASS_AUDIO events.
    sf_audio_playback_hw_instance_t const * p_lower_lvl_hw;   ///< Hardware API's used.
    sf_audio_playback_instance_ctrl_t * p_stream[SF_AUDIO_PLAYBACK_CFG_MAX_STREAMS]; ///< Stream specific data
    TX_THREAD       thread;        ///< Main audio thread.
    TX_EVENT_FLAGS_GROUP flags;    ///< Event flags used to end wait in audio thread.
    sf_audio_playback_data_type_t data_type; ///< Sample format required by the hardware.
    uint8_t         volume;        ///< Volume from 0 (muted) to 255 (maximum, default on open).
    uint8_t         buffer_index;  ///< Which ping pong buffer to use

    /** Stack for audio thread. */
    uint8_t         stack[SF_AUDIO_PLAYBACK_STACK_SIZE] BSP_ALIGN_VARIABLE_V2(BSP_STACK_ALIGNMENT);

    /** Ping pong buffers, used to store converted data during transfer. */
    int16_t         samples[2][SF_AUDIO_PLAYBACK_CFG_BUFFER_SIZE_BYTES / sizeof(int16_t)];
    volatile bool   playing;        ///< State of audio instance (currently playing if true)
    TX_MUTEX        common_instance_mutex;          ///< Mutex for internal use
} sf_audio_playback_common_instance_ctrl_t;

/** Audio stream instance control block. DO NOT INITIALIZE.  Initialization occurs when
 * sf_audio_playback_api_t::open is called. */
/*LDRA_INSPECTED 381 S This structure is typedeffed earlier in this file. */
struct st_sf_audio_playback_instance_ctrl
{
    uint32_t        open;           ///< Used to determine if driver is initialized.

    /** Pointer to thread that began the stream at this index.  Used to ensure multiple threads don't interleave data
     * on the same stream. */
    TX_THREAD *     p_owner;

    /** Callback called when playback of a buffer passed to sf_audio_playback_api_t::start is complete. */
    void (* p_callback)(sf_message_callback_args_t * p_args);
    uint8_t         class_instance;       ///< Class instance used to identify the stream to the messaging framework.
    uint32_t        samples_remaining;    ///< Internal state of data samples remaining for this stream.
    uint32_t        samples_total;        ///< Total number of samples to play (independent of sample size like 8/12/16).
    uint32_t        index;                ///< Internal state of current data index for this stream.
    uint32_t        end;                  ///< Used to track completion of looped playback.
    sf_audio_playback_data_t * p_data[2]; ///< Audio data read from queue.
    sf_audio_playback_status_t status;    ///< Status of current stream.

    /** Pointer to the hardware control block used by this stream. */
    sf_audio_playback_common_instance_ctrl_t * p_common_ctrl;
};

/**********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/
/** @cond INC_HEADER_DEFS_SEC */
/** Filled in Interface API structure for this Instance. */
extern const sf_audio_playback_api_t      g_sf_audio_playback_on_sf_audio_playback;
/** @endcond */

/*******************************************************************************************************************//**
 * @} (end defgroup SF_AUDIO_PLAYBACK)
 **********************************************************************************************************************/

/* Common macro for SSP header files. There is also a corresponding SSP_HEADER macro at the top of this file. */
SSP_FOOTER

#endif /* SF_AUDIO_PLAYBACK_H */
