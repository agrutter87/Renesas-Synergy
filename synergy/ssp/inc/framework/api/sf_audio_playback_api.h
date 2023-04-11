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
* File Name    : sf_audio_playback_api.h
* Description  : Console AMS framework driver header file
***********************************************************************************************************************/

#ifndef SF_AUDIO_PLAYBACK_API_H
#define SF_AUDIO_PLAYBACK_API_H


/*******************************************************************************************************************//**
 * @ingroup SF_Interface_Library
 * @defgroup SF_AUDIO_PLAYBACK_API Audio Framework Interface
 * @brief RTOS-integrated Audio Framework Interface.
 *
 * @section SF_AUDIO_PLAYBACK_API_SUMMARY Summary
 * The Audio Interface is a ThreadX-aware Audio Framework Interface. The Interface is implemented by
 * the @ref SF_AUDIO_PLAYBACK using the Timer driver, the Transfer driver, and a choice
 * of the following drivers for playback: DAC, PWM (to be implemented), or I2S (to be implemented).
 *
 * Interfaces used:
 * - @ref TRANSFER_API
 * - @ref DAC_API
 * - @ref TIMER_API
 *
 * Related SSP architecture topics:
 * - @ref ssp-interfaces
 * - @ref ssp-predefined-layers
 * - @ref using-ssp-modules
 *
 * Audio Framework Interface description: @ref FrameworkAudioPlaybackModule 
 *
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "bsp_api.h"
/* Includes driver interfaces. */
#include "sf_audio_playback_hw_api.h"
#include "sf_audio_playback_cfg.h"
#include "sf_message.h"

/* Include ThreadX API */
/*LDRA_NOANALYSIS tx_api.h is not maintained by Renesas, so LDRA analysis is skipped for this file only. */
#include "tx_api.h"
/*LDRA_ANALYSIS */

/* Common macro for SSP header files. There is also a corresponding SSP_FOOTER macro at the end of this file. */
SSP_HEADER

/**********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/** Version of the API defined in this file */
#define SF_AUDIO_PLAYBACK_API_VERSION_MAJOR   (2U)
#define SF_AUDIO_PLAYBACK_API_VERSION_MINOR   (0U)

/** Audio playback message size in 4 byte words, rounded up. */
#define SF_AUDIO_PLAYBACK_MESSAGE_WORDS      ((sizeof(sf_message_payload_audio_t) + 3) / 4)

/** Macro defining the maximum volume. */
#define SF_AUDIO_PLAYBACK_MAX_VOLUME         (255)

/**********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
/** Audio data for playback. */
typedef struct st_sf_audio_playback_data
{
    sf_message_header_t   header;      ///< Required common members of messaging framework payloads
    sf_audio_playback_data_type_t type;   ///< Data type.  Must be uncompressed.
    uint32_t              size_bytes;  ///< Size of data in bytes.
    void const *          p_data;      ///< Pointer to data.  Data start address must be 4-byte aligned.

    /** ThreadX timeout, select TX_NO_WAIT to play the entire buffer once, TX_WAIT_FOREVER to loop until
     *  SF_AUDIO_PLAYBACK_Pause is called from another thread, or any timeout value from (0x00000001 through
     *  0xFFFFFFFE) in ThreadX tick counts to loop until the tick counts expire. */
    UINT                  loop_timeout;

    /** This releases ownership of the stream and allows other threads to post data.  Set to true if not more data
     * will be sent as a part of this logical bitstream.  Set to false if more packets are being prepared. */
    bool                  stream_end;
} sf_audio_playback_data_t;

/** Audio playback status. */
typedef enum e_sf_audio_playback_status
{
    SF_AUDIO_PLAYBACK_STATUS_STOPPED,   ///< Stream is available to be used.
    SF_AUDIO_PLAYBACK_STATUS_PAUSED,    ///< Stream is paused.
    SF_AUDIO_PLAYBACK_STATUS_PLAYING,   ///< Stream is playing data.
    SF_AUDIO_PLAYBACK_STATUS_WAITING,   ///< Stream is between packets, waiting for the next data message.
} sf_audio_playback_status_t;

/** Audio playback common control block.  Allocate an instance specific control block to pass to
 * sf_audio_playback_api_t::open in sf_audio_playback_cfg_t.
 * @par Implemented as
 * - sf_audio_playback_common_instance_ctrl_t
 */
typedef void sf_audio_playback_common_ctrl_t;

/** Audio playback framework control block.  Allocate an instance specific control block to pass into the
 * audio playback framework API calls.
 * @par Implemented as
 * - sf_audio_playback_instance_ctrl_t
 */
typedef void sf_audio_playback_ctrl_t;

/** Common configuration for RTOS integrated audio framework.  Shared by all streams. */
typedef struct st_sf_audio_playback_common_cfg
{
    UINT                    priority;  ///< Priority of the audio playback thread.
    sf_audio_playback_hw_instance_t const * p_lower_lvl_hw;   ///< Hardware instance.

    /** Pointer to messaging framework instance used to post audio messages. */
    sf_message_instance_t const * p_message;

    /** Pointer to the messaging framework queue specified for this audio stream.  Must be subscribed to the
     * SF_MESSAGE_EVENT_CLASS_AUDIO event class. */
    TX_QUEUE                 * p_queue;

    /** Implementation specific extension configuration. */
    void const               * p_extend;
} sf_audio_playback_common_cfg_t;

/** Per stream configuration for RTOS integrated audio framework. */
typedef struct st_sf_audio_playback_cfg
{
    /** Callback called when playback of a buffer passed to sf_audio_playback_api_t::start is complete.
     * Set to NULL for no callback. */
    void (* p_callback)(sf_message_callback_args_t * p_args);

    /** Pointer to the hardware control block used by this stream. */
    sf_audio_playback_common_ctrl_t      * p_common_ctrl;

    /** Pointer to common configurations shared by all streams using the same hardware. */
    sf_audio_playback_common_cfg_t const * p_common_cfg;
    uint8_t         class_instance;       ///< Class instance used to identify the stream to the messaging framework.
} sf_audio_playback_cfg_t;

/** Audio playback API structure.  Audio playback implementations use the following API. */
typedef struct st_sf_audio_playback_api
{
    /** @brief  Configure the audio framework by creating a thread for audio playback and configuring HAL
     *         layer drivers used.  This function must be called before any other audio functions.
     * @par Implemented as
     *  - SF_AUDIO_PLAYBACK_Open()
     *
     * @param[in,out] p_ctrl   Pointer to a device structure allocated by user. The device control structure is
     *                         initialized in this function.
     * @param[in]     p_cfg    Pointer to configuration structure. All elements of the structure must be set by user.
     */
    ssp_err_t (* open) (sf_audio_playback_ctrl_t      * const p_ctrl,
                        sf_audio_playback_cfg_t const * const p_cfg);

    /** @brief  The close API handles cleans up internal driver data.
     * @par Implemented as
     *  - SF_AUDIO_PLAYBACK_Close()
     *
     * @param[in]   p_ctrl      Pointer to device control block initialized in Open call for audio driver.
     */
    ssp_err_t (* close) (sf_audio_playback_ctrl_t     * const p_ctrl);

    /** @brief  Play audio.  Currently only 16-bit mono PCM buffers are supported.
     * @par Implemented as
     *  - SF_AUDIO_PLAYBACK_Start()
     *
     * @pre Call ::SF_MESSAGE_Open to configure the messaging framework control block and queues with the parameters
     * specified in sf_audio_playback_cfg_t::p_message and sf_audio_playback_cfg_t::p_queue.
     *
     * @param[in,out] p_ctrl       Pointer to device control block initialized in Open call for audio driver.
     * @param[in]     p_data       Pointer to data, description, timeout values, and synchronization options.
     * @param[in]     timeout      ThreadX timeout, represents the maximum amount of time to wait to post to the audio
     *                             queue. Options include TX_NO_WAIT (0x00000000), TX_WAIT_FOREVER (0xFFFFFFFF), and
     *                             timeout values from 0x00000001 through 0xFFFFFFFE in ThreadX tick counts.
     */
    ssp_err_t (* start)  (sf_audio_playback_ctrl_t       * const p_ctrl,
                          sf_audio_playback_data_t       * const p_data,
                          UINT const timeout);

    /** @brief  Pause audio playback.  This stops the peripheral that triggers the DMA/DTC transfer and posts a flag to
     *         notify SF_AUDIO_PLAYBACK_Start() to pause any playback in progress.
     * @par Implemented as
     *  - SF_AUDIO_PLAYBACK_Pause()
     *
     * @pre    Call SF_AUDIO_PLAYBACK_Start() before using this function.  Calling SF_AUDIO_PLAYBACK_Pause() before
     *         SF_AUDIO_PLAYBACK_Start() has no effect and does not return an error code.
     *
     * @param[in]   p_ctrl       Pointer to device control block initialized in Open call for audio driver.
     */
    ssp_err_t (* pause) (sf_audio_playback_ctrl_t        * const p_ctrl);

    /** @brief  Stop audio playback.  Causes SF_AUDIO_PLAYBACK_Start() halt playback and return.
     * @par Implemented as
     *  - SF_AUDIO_PLAYBACK_Stop()
     *
     * @pre    Call SF_AUDIO_PLAYBACK_Start() before using this function.  Calling SF_AUDIO_PLAYBACK_Stop() before
     *         SF_AUDIO_PLAYBACK_Start() has no effect and does not return an error code.
     *
     * @param[in]   p_ctrl       Pointer to device control block initialized in Open call for audio driver.
     */
    ssp_err_t (* stop) (sf_audio_playback_ctrl_t         * const p_ctrl);

    /** @brief  Resume audio playback.  Posts a flag to notify SF_AUDIO_PLAYBACK_Start() to restart the peripheral that
     *         triggers the DMA/DTC transfer.
     * @par Implemented as
     *  - SF_AUDIO_PLAYBACK_Resume()
     *
     * @pre    Call SF_AUDIO_PLAYBACK_Pause() before using this function.  Calling SF_AUDIO_PLAYBACK_Resume() before
     *         SF_AUDIO_PLAYBACK_Pause() has no effect and does not return an error code.
     *
     * @param[in]   p_ctrl       Pointer to device control block initialized in Open call for audio driver.
     */
    ssp_err_t (* resume) (sf_audio_playback_ctrl_t       * const p_ctrl);

    /** @brief     Set software volume control.  Software volume control is applied globally to all streams on
     * the hardware.
     * @par Implemented as
     *  - SF_AUDIO_PLAYBACK_VolumeSet()
     *
     * @warning   Software volume control reduces resolution and may require extra memory and processing bandwidth.
     *
     * @param[in]   p_ctrl       Pointer to device control block initialized in Open call for audio driver.
     * @param[in]   volume       Volume level requested.  Valid range is from 0 (muted, which will stop playback) to 255
     *                           (maximum volume, default on open).
     */
    ssp_err_t (* volumeSet)(sf_audio_playback_ctrl_t     * const p_ctrl,
                            uint8_t                        const volume);

    /** @brief  Store version information in provided pointer.
     * @par Implemented as
     *  - SF_AUDIO_PLAYBACK_VersionGet()
     *
     * @param[in]   p_version    Pointer to device control block initialized in Open call for UART driver.
     */
    ssp_err_t (* versionGet) (ssp_version_t * const p_version);
} sf_audio_playback_api_t;

/** This structure encompasses everything that is needed to use an instance of this interface. */
typedef struct st_sf_audio_playback_instance
{
    sf_audio_playback_ctrl_t      * p_ctrl;    ///< Pointer to the control structure for this instance
    sf_audio_playback_cfg_t const * p_cfg;     ///< Pointer to the configuration structure for this instance
    sf_audio_playback_api_t const * p_api;     ///< Pointer to the API structure for this instance
} sf_audio_playback_instance_t;


/*******************************************************************************************************************//**
 * @} (end defgroup SF_AUDIO_PLAYBACK_API)
 **********************************************************************************************************************/

/* Common macro for SSP header files. There is also a corresponding SSP_HEADER macro at the top of this file. */
SSP_FOOTER

#endif /* SF_AUDIO_PLAYBACK_API_H */
