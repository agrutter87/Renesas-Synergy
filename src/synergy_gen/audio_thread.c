/* generated thread source file - do not edit */
#include "audio_thread.h"

TX_THREAD audio_thread;
void audio_thread_create(void);
static void audio_thread_func(ULONG thread_input);
static uint8_t audio_thread_stack[2048] BSP_PLACE_IN_SECTION_V2(".stack.audio_thread") BSP_ALIGN_VARIABLE_V2(BSP_STACK_ALIGNMENT);
void tx_startup_err_callback(void *p_instance, void *p_data);
void tx_startup_common_init(void);
#if defined(__ICCARM__)
              #define g_sf_audio_playback_err_callback_WEAK_ATTRIBUTE
              #pragma weak g_sf_audio_playback_err_callback  = g_sf_audio_playback_err_callback_internal
              #elif defined(__GNUC__)
              #define g_sf_audio_playback_err_callback_WEAK_ATTRIBUTE   __attribute__ ((weak, alias("g_sf_audio_playback_err_callback_internal")))
              #endif
void g_sf_audio_playback_err_callback(void *p_instance, void *p_data)
g_sf_audio_playback_err_callback_WEAK_ATTRIBUTE;
sf_audio_playback_instance_ctrl_t g_sf_audio_playback_ctrl;
const sf_audio_playback_cfg_t g_sf_audio_playback_cfg = { .p_common_cfg =
		&g_sf_audio_playback_common_cfg, .p_common_ctrl =
		&g_sf_audio_playback_common, .class_instance = 0, .p_callback =
		sf_audio_playback_callback, };
const sf_audio_playback_instance_t g_sf_audio_playback = { .p_api =
		&g_sf_audio_playback_on_sf_audio_playback, .p_ctrl =
		&g_sf_audio_playback_ctrl, .p_cfg = &g_sf_audio_playback_cfg, };
/*******************************************************************************************************************//**
 * @brief      This is a weak example initialization error function.  It should be overridden by defining a user  function 
 *             with the prototype below.
 *             - void g_sf_audio_playback_err_callback(void * p_instance, void * p_data)
 *
 * @param[in]  p_instance arguments used to identify which instance caused the error and p_data Callback arguments used to identify what error caused the callback.
 **********************************************************************************************************************/
void g_sf_audio_playback_err_callback_internal(void *p_instance, void *p_data);
void g_sf_audio_playback_err_callback_internal(void *p_instance, void *p_data) {
	/** Suppress compiler warning for not using parameters. */
	SSP_PARAMETER_NOT_USED(p_instance);
	SSP_PARAMETER_NOT_USED(p_data);

	/** An error has occurred. Please check function arguments for more information. */
	BSP_CFG_HANDLE_UNRECOVERABLE_ERROR(0);
}

/*******************************************************************************************************************//**
 * @brief     Initialization function that the user can choose to have called automatically during thread entry.
 *            The user can call this function at a later time if desired using the prototype below.
 *            - void sf_audio_playback_init0(void)
 **********************************************************************************************************************/
void sf_audio_playback_init0(void) {
	ssp_err_t ssp_err_g_sf_audio_playback;
	ssp_err_g_sf_audio_playback = g_sf_audio_playback.p_api->open(
			g_sf_audio_playback.p_ctrl, g_sf_audio_playback.p_cfg);
	if (SSP_SUCCESS != ssp_err_g_sf_audio_playback) {
		g_sf_audio_playback_err_callback((void*) &g_sf_audio_playback,
				&ssp_err_g_sf_audio_playback);
	}
}
TX_SEMAPHORE g_sf_audio_playback_semaphore;
extern bool g_ssp_common_initialized;
extern uint32_t g_ssp_common_thread_count;
extern TX_SEMAPHORE g_ssp_common_initialized_semaphore;

void audio_thread_create(void) {
	/* Increment count so we will know the number of ISDE created threads. */
	g_ssp_common_thread_count++;

	/* Initialize each kernel object. */
	UINT err_g_sf_audio_playback_semaphore;
	err_g_sf_audio_playback_semaphore = tx_semaphore_create(
			&g_sf_audio_playback_semaphore, (CHAR*) "Audio Semaphore", 0);
	if (TX_SUCCESS != err_g_sf_audio_playback_semaphore) {
		tx_startup_err_callback(&g_sf_audio_playback_semaphore, 0);
	}

	UINT err;
	err = tx_thread_create(&audio_thread, (CHAR*) "Audio Thread",
			audio_thread_func, (ULONG) NULL, &audio_thread_stack, 2048, 8, 8, 1,
			TX_AUTO_START);
	if (TX_SUCCESS != err) {
		tx_startup_err_callback(&audio_thread, 0);
	}
}

static void audio_thread_func(ULONG thread_input) {
	/* Not currently using thread_input. */
	SSP_PARAMETER_NOT_USED(thread_input);

	/* Initialize common components */
	tx_startup_common_init();

	/* Initialize each module instance. */
	/** Call initialization function if user has selected to do so. */
#if (1)
	sf_audio_playback_init0();
#endif

	/* Enter user code for this thread. */
	audio_thread_entry();
}
