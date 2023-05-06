/* generated thread source file - do not edit */
#include "main_system_thread.h"

TX_THREAD main_system_thread;
void main_system_thread_create(void);
static void main_system_thread_func(ULONG thread_input);
static uint8_t main_system_thread_stack[1024] BSP_PLACE_IN_SECTION_V2(".stack.main_system_thread") BSP_ALIGN_VARIABLE_V2(BSP_STACK_ALIGNMENT);
void tx_startup_err_callback(void *p_instance, void *p_data);
void tx_startup_common_init(void);
#if (6) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_adc0) && !defined(SSP_SUPPRESS_ISR_ADC0)
SSP_VECTOR_DEFINE_CHAN(adc_scan_end_isr, ADC, SCAN_END, 0);
#endif
#endif
#if (6) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_adc0) && !defined(SSP_SUPPRESS_ISR_ADC0)
SSP_VECTOR_DEFINE_CHAN(adc_scan_end_b_isr, ADC, SCAN_END_B, 0);
#endif
#endif
adc_instance_ctrl_t g_adc0_ctrl;
const adc_cfg_t g_adc0_cfg = { .unit = 0, .mode = ADC_MODE_SINGLE_SCAN,
		.resolution = ADC_RESOLUTION_12_BIT, .alignment = ADC_ALIGNMENT_RIGHT,
		.add_average_count = ADC_ADD_AVERAGE_FOUR, .clearing =
				ADC_CLEAR_AFTER_READ_ON, .trigger = ADC_TRIGGER_SOFTWARE,
		.trigger_group_b = ADC_TRIGGER_SYNC_ELC, .p_callback = adc_callback,
		.p_context = &g_adc0, .scan_end_ipl = (6), .scan_end_b_ipl = (6),
		.calib_adc_skip = false, .voltage_ref = ADC_EXTERNAL_VOLTAGE,
		.over_current = OVER_CURRENT_DETECTION_ENABLE, .pga0 = PGA_DISABLE,
		.pga1 = PGA_DISABLE, .pga2 = PGA_DISABLE, };
const adc_channel_cfg_t g_adc0_channel_cfg = { .scan_mask = (uint32_t)(
		((uint64_t) ADC_MASK_CHANNEL_0) | ((uint64_t) 0) | ((uint64_t) 0)
				| ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
				| ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
				| ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
				| ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
				| ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
				| ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
				| ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
				| ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
				| ((uint64_t) 0) | ((uint64_t) ADC_MASK_TEMPERATURE)
				| ((uint64_t) ADC_MASK_VOLT)),
/** Group B channel mask is right shifted by 32 at the end to form the proper mask */
.scan_mask_group_b = (uint32_t)(
		(((uint64_t) ADC_MASK_CHANNEL_0) | ((uint64_t) 0) | ((uint64_t) 0)
				| ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
				| ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
				| ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
				| ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
				| ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
				| ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
				| ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
				| ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
				| ((uint64_t) 0) | ((uint64_t) ADC_MASK_TEMPERATURE)
				| ((uint64_t) ADC_MASK_VOLT)) >> 32), .priority_group_a =
		ADC_GROUP_A_PRIORITY_OFF, .add_mask = (uint32_t)(
		(ADC_MASK_CHANNEL_0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0)
				| (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0)
				| (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0)
				| (ADC_MASK_TEMPERATURE) | (ADC_MASK_VOLT)), .sample_hold_mask =
		(uint32_t)((0) | (0) | (0)), .sample_hold_states = 24, };
/* Instance structure to use this module. */
const adc_instance_t g_adc0 = { .p_ctrl = &g_adc0_ctrl, .p_cfg = &g_adc0_cfg,
		.p_channel_cfg = &g_adc0_channel_cfg, .p_api = &g_adc_on_adc };
TX_EVENT_FLAGS_GROUP g_main_system_event_flags;
extern bool g_ssp_common_initialized;
extern uint32_t g_ssp_common_thread_count;
extern TX_SEMAPHORE g_ssp_common_initialized_semaphore;

void main_system_thread_create(void) {
	/* Increment count so we will know the number of ISDE created threads. */
	g_ssp_common_thread_count++;

	/* Initialize each kernel object. */
	UINT err_g_main_system_event_flags;
	err_g_main_system_event_flags = tx_event_flags_create(
			&g_main_system_event_flags, (CHAR*) "Main System Event Flags");
	if (TX_SUCCESS != err_g_main_system_event_flags) {
		tx_startup_err_callback(&g_main_system_event_flags, 0);
	}

	UINT err;
	err = tx_thread_create(&main_system_thread, (CHAR*) "Main System Thread",
			main_system_thread_func, (ULONG) NULL, &main_system_thread_stack,
			1024, 7, 7, 1, TX_AUTO_START);
	if (TX_SUCCESS != err) {
		tx_startup_err_callback(&main_system_thread, 0);
	}
}

static void main_system_thread_func(ULONG thread_input) {
	/* Not currently using thread_input. */
	SSP_PARAMETER_NOT_USED(thread_input);

	/* Initialize common components */
	tx_startup_common_init();

	/* Initialize each module instance. */

	/* Enter user code for this thread. */
	main_system_thread_entry();
}
