/* generated messaging source file - do not edit */
#include "sf_message.h"
#ifndef SF_MESSAGE_CFG_QUEUE_SIZE
#define SF_MESSAGE_CFG_QUEUE_SIZE (16)
#endif
TX_QUEUE audio_thread_message_queue;
static uint8_t queue_memory_audio_thread_message_queue[SF_MESSAGE_CFG_QUEUE_SIZE];
TX_QUEUE sf_audio_dummy_message_queue;
static uint8_t queue_memory_sf_audio_dummy_message_queue[SF_MESSAGE_CFG_QUEUE_SIZE];
TX_QUEUE gui_thread_message_queue;
static uint8_t queue_memory_gui_thread_message_queue[SF_MESSAGE_CFG_QUEUE_SIZE];
static sf_message_subscriber_t sf_audio_dummy_message_queue_0_0 = { .p_queue =
		&sf_audio_dummy_message_queue, .instance_range =
		{ .start = 0, .end = 0 } };
static sf_message_subscriber_t audio_thread_message_queue_0_0 =
		{ .p_queue = &audio_thread_message_queue, .instance_range = {
				.start = 0, .end = 0 } };
static sf_message_subscriber_t gui_thread_message_queue_0_0 = { .p_queue =
		&gui_thread_message_queue, .instance_range = { .start = 0, .end = 0 } };
static sf_message_subscriber_t *gp_group_SF_MESSAGE_EVENT_CLASS_AUDIO[] = {
		&sf_audio_dummy_message_queue_0_0, };
static sf_message_subscriber_list_t g_list_SF_MESSAGE_EVENT_CLASS_AUDIO = {
		.event_class = SF_MESSAGE_EVENT_CLASS_AUDIO, .number_of_nodes = 1,
		.pp_subscriber_group = gp_group_SF_MESSAGE_EVENT_CLASS_AUDIO };
static sf_message_subscriber_t *gp_group_SF_MESSAGE_EVENT_CLASS_APP_CMD[] = {
		&audio_thread_message_queue_0_0, };
static sf_message_subscriber_list_t g_list_SF_MESSAGE_EVENT_CLASS_APP_CMD = {
		.event_class = SF_MESSAGE_EVENT_CLASS_APP_CMD, .number_of_nodes = 1,
		.pp_subscriber_group = gp_group_SF_MESSAGE_EVENT_CLASS_APP_CMD };
static sf_message_subscriber_t *gp_group_SF_MESSAGE_EVENT_CLASS_APP_CB[] = {
		&gui_thread_message_queue_0_0, };
static sf_message_subscriber_list_t g_list_SF_MESSAGE_EVENT_CLASS_APP_CB = {
		.event_class = SF_MESSAGE_EVENT_CLASS_APP_CB, .number_of_nodes = 1,
		.pp_subscriber_group = gp_group_SF_MESSAGE_EVENT_CLASS_APP_CB };
sf_message_subscriber_list_t *p_subscriber_lists[] = {
		&g_list_SF_MESSAGE_EVENT_CLASS_AUDIO,
		&g_list_SF_MESSAGE_EVENT_CLASS_APP_CMD,
		&g_list_SF_MESSAGE_EVENT_CLASS_APP_CB, NULL };
void g_message_init(void);
void g_message_init(void) {
	tx_queue_create(&audio_thread_message_queue,
			(CHAR*) "Audio Thread Message Queue", 1,
			&queue_memory_audio_thread_message_queue,
			sizeof(queue_memory_audio_thread_message_queue));
	tx_queue_create(&sf_audio_dummy_message_queue,
			(CHAR*) "Audio Framework Dummy Thread Message Queue", 1,
			&queue_memory_sf_audio_dummy_message_queue,
			sizeof(queue_memory_sf_audio_dummy_message_queue));
	tx_queue_create(&gui_thread_message_queue,
			(CHAR*) "GUI Thread Message Queue", 1,
			&queue_memory_gui_thread_message_queue,
			sizeof(queue_memory_gui_thread_message_queue));
}
