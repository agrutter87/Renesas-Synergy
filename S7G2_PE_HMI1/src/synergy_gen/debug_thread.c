/* generated thread source file - do not edit */
#include "debug_thread.h"

TX_THREAD debug_thread;
void debug_thread_create(void);
static void debug_thread_func(ULONG thread_input);
static uint8_t debug_thread_stack[1024] BSP_PLACE_IN_SECTION_V2(".stack.debug_thread") BSP_ALIGN_VARIABLE_V2(BSP_STACK_ALIGNMENT);
void tx_startup_err_callback(void *p_instance, void *p_data);
void tx_startup_common_init(void);
TX_QUEUE g_debug_queue;
static uint8_t queue_memory_g_debug_queue[1024];
extern bool g_ssp_common_initialized;
extern uint32_t g_ssp_common_thread_count;
extern TX_SEMAPHORE g_ssp_common_initialized_semaphore;

void debug_thread_create(void) {
	/* Increment count so we will know the number of ISDE created threads. */
	g_ssp_common_thread_count++;

	/* Initialize each kernel object. */
	UINT err_g_debug_queue;
	err_g_debug_queue = tx_queue_create(&g_debug_queue, (CHAR*) "Debug Queue",
			16, &queue_memory_g_debug_queue,
			sizeof(queue_memory_g_debug_queue));
	if (TX_SUCCESS != err_g_debug_queue) {
		tx_startup_err_callback(&g_debug_queue, 0);
	}

	UINT err;
	err = tx_thread_create(&debug_thread, (CHAR*) "Debug Thread",
			debug_thread_func, (ULONG) NULL, &debug_thread_stack, 1024, 8, 8, 1,
			TX_AUTO_START);
	if (TX_SUCCESS != err) {
		tx_startup_err_callback(&debug_thread, 0);
	}
}

static void debug_thread_func(ULONG thread_input) {
	/* Not currently using thread_input. */
	SSP_PARAMETER_NOT_USED(thread_input);

	/* Initialize common components */
	tx_startup_common_init();

	/* Initialize each module instance. */

	/* Enter user code for this thread. */
	debug_thread_entry();
}
