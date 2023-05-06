/* generated messaging header file - do not edit */
#ifndef SF_MESSAGE_PORT_H_
#define SF_MESSAGE_PORT_H_
typedef enum e_sf_message_event_class {
	SF_MESSAGE_EVENT_CLASS_AUDIO, /* Audio Playback */
	SF_MESSAGE_EVENT_CLASS_APP_CMD, /* App Command */
	SF_MESSAGE_EVENT_CLASS_APP_CB, /* App Callback */
} sf_message_event_class_t;
typedef enum e_sf_message_event {
	SF_MESSAGE_EVENT_UNUSED, /* Unused */
	SF_MESSAGE_EVENT_NEW_DATA, /* New Data */
	SF_MESSAGE_EVENT_AUDIO_START, /* Audio Playback Start */
	SF_MESSAGE_EVENT_AUDIO_STOP, /* Audio Playback Stop */
	SF_MESSAGE_EVENT_AUDIO_PAUSE, /* Audio Playback Pause */
	SF_MESSAGE_EVENT_AUDIO_RESUME, /* Audio Playback Resume */
	SF_MESSAGE_EVENT_APP_OPEN, /* App Command Open */
	SF_MESSAGE_EVENT_APP_PAUSE, /* App Command Pause */
	SF_MESSAGE_EVENT_APP_RESUME, /* App Command Resume */
	SF_MESSAGE_EVENT_APP_CLOSE, /* App Command Close */
	SF_MESSAGE_EVENT_APP_REWIND, /* App Command Rewind */
	SF_MESSAGE_EVENT_APP_FORWARD, /* App Command Forward */
	SF_MESSAGE_EVENT_APP_GOTO, /* App Command Goto */
	SF_MESSAGE_EVENT_APP_RESTART, /* App Command Restart */
	SF_MESSAGE_EVENT_APP_VOL_UP, /* App Command Volume Up */
	SF_MESSAGE_EVENT_APP_VOL_DOWN, /* App Command Volume Down */
	SF_MESSAGE_EVENT_APP_CB_STATUS, /* App Callback Status Changed */
	SF_MESSAGE_EVENT_APP_CB_VOLUME, /* App Callback Volume Changed */
	SF_MESSAGE_EVENT_APP_CB_USB_IN, /* App Callback USB Inserted */
	SF_MESSAGE_EVENT_APP_CB_USB_OUT, /* App Callback USB Removed */
	SF_MESSAGE_EVENT_APP_ERR_OPEN, /* App Error Open */
	SF_MESSAGE_EVENT_APP_ERR_HEADER, /* App Error Header */
	SF_MESSAGE_EVENT_APP_ERR_PAUSE, /* App Error Pause */
	SF_MESSAGE_EVENT_APP_ERR_RESUME, /* App Error Resume */
	SF_MESSAGE_EVENT_APP_ERR_CLOSE, /* App Error Close */
	SF_MESSAGE_EVENT_APP_ERR_VOLUME, /* App Error Volume */
	SF_MESSAGE_EVENT_APP_ERR_PLAYBACK, /* App Error Playback */
	SF_MESSAGE_EVENT_APP_LAST, /* App Last Event */
} sf_message_event_t;
extern TX_QUEUE audio_thread_message_queue;
extern TX_QUEUE sf_audio_dummy_message_queue;
extern TX_QUEUE gui_thread_message_queue;
#endif /* SF_MESSAGE_PORT_H_ */
