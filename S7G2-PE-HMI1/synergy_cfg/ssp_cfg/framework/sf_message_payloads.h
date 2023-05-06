/* generated messaging header file - do not edit */
#ifndef SF_MESSAGE_PAYLOADS_H_
#define SF_MESSAGE_PAYLOADS_H_
#include "sf_audio_playback_api.h"
#include "application_payloads.h"
typedef union u_sf_message_payload {
	sf_audio_playback_data_t sf_audio_playback_data;
	app_message_payload_t app_cmd_payload;
	app_message_payload_t app_cb_payload;
} sf_message_payload_t;
#endif /* SF_MESSAGE_PAYLOADS_H_ */
