/* generated thread source file - do not edit */
#include "sf_audio_dummy.h"

TX_THREAD sf_audio_dummy;
void sf_audio_dummy_create(void);
static void sf_audio_dummy_func(ULONG thread_input);
static uint8_t sf_audio_dummy_stack[512] BSP_PLACE_IN_SECTION_V2(".stack.sf_audio_dummy") BSP_ALIGN_VARIABLE_V2(BSP_STACK_ALIGNMENT);
void tx_startup_err_callback(void *p_instance, void *p_data);
void tx_startup_common_init(void);
extern bool g_ssp_common_initialized;
extern uint32_t g_ssp_common_thread_count;
extern TX_SEMAPHORE g_ssp_common_initialized_semaphore;

void sf_audio_dummy_create(void) {
	/* Increment count so we will know the number of ISDE created threads. */
	g_ssp_common_thread_count++;

	/* Initialize each kernel object. */

	UINT err;
	err = tx_thread_create(&sf_audio_dummy,
			(CHAR*) "Audio Framework Dummy Thread", sf_audio_dummy_func,
			(ULONG) NULL, &sf_audio_dummy_stack, 512, 9, 9, 1, TX_DONT_START);
	if (TX_SUCCESS != err) {
		tx_startup_err_callback(&sf_audio_dummy, 0);
	}
}

static void sf_audio_dummy_func(ULONG thread_input) {
	/* Not currently using thread_input. */
	SSP_PARAMETER_NOT_USED(thread_input);

	/* Initialize common components */
	tx_startup_common_init();

	/* Initialize each module instance. */

	/* Enter user code for this thread. */
	sf_audio_dummy_entry();
}
