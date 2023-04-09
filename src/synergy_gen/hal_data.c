/* generated HAL source file - do not edit */
#include "hal_data.h"
#if (BSP_IRQ_DISABLED) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_backlight_pwm) && !defined(SSP_SUPPRESS_ISR_GPT11)
SSP_VECTOR_DEFINE_CHAN(gpt_counter_overflow_isr, GPT, COUNTER_OVERFLOW, 11);
#endif
#endif
static gpt_instance_ctrl_t g_backlight_pwm_ctrl;
static const timer_on_gpt_cfg_t g_backlight_pwm_extend = { .gtioca = {
		.output_enabled = true, .stop_level = GPT_PIN_LEVEL_LOW }, .gtiocb = {
		.output_enabled = false, .stop_level = GPT_PIN_LEVEL_LOW },
		.shortest_pwm_signal = GPT_SHORTEST_LEVEL_OFF, };
static const timer_cfg_t g_backlight_pwm_cfg = { .mode = TIMER_MODE_PWM,
		.period = 10, .unit = TIMER_UNIT_PERIOD_MSEC, .duty_cycle = 100,
		.duty_cycle_unit = TIMER_PWM_UNIT_PERCENT, .channel = 11, .autostart =
				true, .p_callback = NULL, .p_context = &g_backlight_pwm,
		.p_extend = &g_backlight_pwm_extend, .irq_ipl = (BSP_IRQ_DISABLED), };
/* Instance structure to use this module. */
const timer_instance_t g_backlight_pwm = { .p_ctrl = &g_backlight_pwm_ctrl,
		.p_cfg = &g_backlight_pwm_cfg, .p_api = &g_timer_on_gpt };
void g_hal_init(void) {
	g_common_init();
}
