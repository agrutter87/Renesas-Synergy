/* generated thread source file - do not edit */
#include "gui_thread.h"

TX_THREAD gui_thread;
void gui_thread_create(void);
static void gui_thread_func(ULONG thread_input);
static uint8_t gui_thread_stack[2048] BSP_PLACE_IN_SECTION_V2(".stack.gui_thread") BSP_ALIGN_VARIABLE_V2(BSP_STACK_ALIGNMENT);
void tx_startup_err_callback(void *p_instance, void *p_data);
void tx_startup_common_init(void);
#if (3) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_external_irq0) && !defined(SSP_SUPPRESS_ISR_ICU12)
SSP_VECTOR_DEFINE( icu_irq_isr, ICU, IRQ12);
#endif
#endif
static icu_instance_ctrl_t g_external_irq0_ctrl;
static const external_irq_cfg_t g_external_irq0_cfg = { .channel = 12,
		.trigger = EXTERNAL_IRQ_TRIG_FALLING, .filter_enable = true, .pclk_div =
				EXTERNAL_IRQ_PCLK_DIV_BY_1, .autostart = true, .p_callback =
				NULL, .p_context = &g_external_irq0, .p_extend = NULL,
		.irq_ipl = (3), };
/* Instance structure to use this module. */
const external_irq_instance_t g_external_irq0 = { .p_ctrl =
		&g_external_irq0_ctrl, .p_cfg = &g_external_irq0_cfg, .p_api =
		&g_external_irq_on_icu };
sf_external_irq_instance_ctrl_t g_sf_external_irq0_ctrl;
const sf_external_irq_cfg_t g_sf_external_irq0_cfg =
		{ .event = SF_EXTERNAL_IRQ_EVENT_SEMAPHORE_PUT, .p_lower_lvl_irq =
				&g_external_irq0, };
/* Instance structure to use this module. */
const sf_external_irq_instance_t g_sf_external_irq0 = { .p_ctrl =
		&g_sf_external_irq0_ctrl, .p_cfg = &g_sf_external_irq0_cfg, .p_api =
		&g_sf_external_irq_on_sf_external_irq };
/** Get driver cfg from bus and use all same settings except slave address and addressing mode. */
const i2c_cfg_t g_sf_i2c_device0_i2c_cfg = { .channel = g_sf_i2c_bus0_CHANNEL,
		.rate = g_sf_i2c_bus0_RATE, .slave = 0x38, .addr_mode =
				I2C_ADDR_MODE_7BIT, .sda_delay = g_sf_i2c_bus0_SDA_DELAY,
		.p_transfer_tx = g_sf_i2c_bus0_P_TRANSFER_TX, .p_transfer_rx =
				g_sf_i2c_bus0_P_TRANSFER_RX, .p_callback =
				g_sf_i2c_bus0_P_CALLBACK, .p_context = g_sf_i2c_bus0_P_CONTEXT,
		.rxi_ipl = g_sf_i2c_bus0_RXI_IPL, .txi_ipl = g_sf_i2c_bus0_TXI_IPL,
		.tei_ipl = g_sf_i2c_bus0_TEI_IPL, .eri_ipl = g_sf_i2c_bus0_ERI_IPL,
		.p_extend = g_sf_i2c_bus0_P_EXTEND, };

sf_i2c_instance_ctrl_t g_sf_i2c_device0_ctrl = { .p_lower_lvl_ctrl =
		&g_i2c0_ctrl, };
const sf_i2c_cfg_t g_sf_i2c_device0_cfg = { .p_bus =
		(sf_i2c_bus_t*) &g_sf_i2c_bus0, .p_lower_lvl_cfg =
		&g_sf_i2c_device0_i2c_cfg, };
/* Instance structure to use this module. */
const sf_i2c_instance_t g_sf_i2c_device0 = { .p_ctrl = &g_sf_i2c_device0_ctrl,
		.p_cfg = &g_sf_i2c_device0_cfg, .p_api = &g_sf_i2c_on_sf_i2c };
sf_touch_panel_chip_ft5x06_instance_ctrl_t g_touch_panel_chip_ft5x06_ctrl;
const sf_touch_panel_chip_on_ft5x06_cfg_t g_touch_panel_chip_ft5x06_cfg_extend =
		{ .pin = IOPORT_PORT_10_PIN_02, .p_lower_lvl_framewrk =
				&g_sf_i2c_device0, .p_lower_lvl_irq = &g_sf_external_irq0 };
const sf_touch_panel_chip_cfg_t g_touch_panel_chip_ft5x06_cfg = { .p_extend =
		&g_touch_panel_chip_ft5x06_cfg_extend };
const sf_touch_panel_chip_instance_t g_touch_panel_chip_ft5x06 = { .p_ctrl =
		&g_touch_panel_chip_ft5x06_ctrl,
		.p_cfg = &g_touch_panel_chip_ft5x06_cfg, .p_api =
				&g_sf_touch_panel_chip_ft5x06 };
#if defined(__ICCARM__)
            #define g_sf_touch_panel_err_callback_WEAK_ATTRIBUTE
            #pragma weak g_sf_touch_panel_err_callback  = g_sf_touch_panel_err_callback_internal
            #elif defined(__GNUC__)
            #define g_sf_touch_panel_err_callback_WEAK_ATTRIBUTE   __attribute__ ((weak, alias("g_sf_touch_panel_err_callback_internal")))
            #endif
void g_sf_touch_panel_err_callback(void *p_instance, void *p_data)
g_sf_touch_panel_err_callback_WEAK_ATTRIBUTE;
sf_touch_panel_v2_instance_ctrl_t g_sf_touch_panel_ctrl;
const sf_touch_panel_v2_extend_cfg_t g_sf_touch_panel_cfg_extend = { .p_chip =
		&g_touch_panel_chip_ft5x06 };
const sf_touch_panel_v2_cfg_t g_sf_touch_panel_cfg = { .hsize_pixels = 800,
		.vsize_pixels = 480, .priority = 3, .update_hz = 10, .p_extend =
				&g_sf_touch_panel_cfg_extend, .rotation_angle = 0, .p_callback =
				touch_callback, .p_context = &g_sf_touch_panel };

/* Instance structure to use this module. */
const sf_touch_panel_v2_instance_t g_sf_touch_panel = { .p_ctrl =
		&g_sf_touch_panel_ctrl, .p_cfg = &g_sf_touch_panel_cfg, .p_api =
		&g_sf_touch_panel_v2_on_sf_touch_panel_v2 };
/*******************************************************************************************************************//**
 * @brief      Initialization function that the user can choose to have called automatically during thread entry.
 *             The user can call this function at a later time if desired using the prototype below.

 *             - void g_sf_touch_panel_err_callback(void * p_instance, void * p_data)
 *
 * @param[in]  p_instance arguments used to identify which instance caused the error and p_data Callback arguments
 used to identify what error caused the callback.
 **********************************************************************************************************************/
void g_sf_touch_panel_err_callback_internal(void *p_instance, void *p_data);
void g_sf_touch_panel_err_callback_internal(void *p_instance, void *p_data) {
	/** Suppress compiler warning for not using parameters. */
	SSP_PARAMETER_NOT_USED(p_instance);
	SSP_PARAMETER_NOT_USED(p_data);
	/** An error has occurred. Please check function arguments for more information. */
	BSP_CFG_HANDLE_UNRECOVERABLE_ERROR(0);
}
/*******************************************************************************************************************//**
 * @brief     This is sf touch panel initialization function. User Can call this function in the application 
 if required with the below mentioned prototype.
 *            - void sf_touch_panel_init(void)
 **********************************************************************************************************************/
void sf_touch_panel_init(void) {
	ssp_err_t ssp_err_g_sf_touch_panel;
	ssp_err_g_sf_touch_panel = g_sf_touch_panel.p_api->open(
			g_sf_touch_panel.p_ctrl, g_sf_touch_panel.p_cfg);
	if (SSP_SUCCESS != ssp_err_g_sf_touch_panel) {
		g_sf_touch_panel_err_callback((void*) &g_sf_touch_panel,
				&ssp_err_g_sf_touch_panel);
	}
	if (1) {
		ssp_err_g_sf_touch_panel = g_sf_touch_panel.p_api->start(
				g_sf_touch_panel.p_ctrl);
		if (SSP_SUCCESS != ssp_err_g_sf_touch_panel) {
			g_sf_touch_panel_err_callback((void*) &g_sf_touch_panel,
					&ssp_err_g_sf_touch_panel);
		}
	}
}
extern bool g_ssp_common_initialized;
extern uint32_t g_ssp_common_thread_count;
extern TX_SEMAPHORE g_ssp_common_initialized_semaphore;

void gui_thread_create(void) {
	/* Increment count so we will know the number of ISDE created threads. */
	g_ssp_common_thread_count++;

	/* Initialize each kernel object. */

	UINT err;
	err = tx_thread_create(&gui_thread, (CHAR*) "GUI Thread", gui_thread_func,
			(ULONG) NULL, &gui_thread_stack, 2048, 5, 5, 1, TX_DONT_START);
	if (TX_SUCCESS != err) {
		tx_startup_err_callback(&gui_thread, 0);
	}
}

static void gui_thread_func(ULONG thread_input) {
	/* Not currently using thread_input. */
	SSP_PARAMETER_NOT_USED(thread_input);

	/* Initialize common components */
	tx_startup_common_init();

	/* Initialize each module instance. */
	/** Call initialization function if user has selected to do so. */
#if (1)
	sf_touch_panel_init();
#endif

	/* Enter user code for this thread. */
	gui_thread_entry();
}
