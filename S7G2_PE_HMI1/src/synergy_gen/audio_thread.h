/* generated thread header file - do not edit */
#ifndef AUDIO_THREAD_H_
#define AUDIO_THREAD_H_
#include "bsp_api.h"
#include "tx_api.h"
#include "hal_data.h"
#ifdef __cplusplus
                extern "C" void audio_thread_entry(void);
                #else
extern void audio_thread_entry(void);
#endif
#include "sf_audio_playback.h"
#ifdef __cplusplus
extern "C" {
#endif
extern sf_audio_playback_common_instance_ctrl_t g_sf_audio_playback_common;
extern const sf_audio_playback_common_cfg_t g_sf_audio_playback_common_cfg;
extern const sf_audio_playback_instance_t g_sf_audio_playback;
/** Pointer to Audio Playback API */
extern const sf_audio_playback_api_t *g_sf_audio_playback_api;
#ifndef sf_audio_playback_callback
void sf_audio_playback_callback(sf_message_callback_args_t *p_args);
#endif
void g_sf_audio_playback_err_callback(void *p_instance, void *p_data);
void sf_audio_playback_init0(void);
extern TX_SEMAPHORE g_sf_audio_playback_semaphore;
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* AUDIO_THREAD_H_ */
