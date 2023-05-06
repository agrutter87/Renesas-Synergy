/* generated thread source file - do not edit */
#include "accelerometer_thread.h"

TX_THREAD accelerometer_thread;
void accelerometer_thread_create(void);
static void accelerometer_thread_func(ULONG thread_input);
static uint8_t accelerometer_thread_stack[1024] BSP_PLACE_IN_SECTION_V2(".stack.accelerometer_thread") BSP_ALIGN_VARIABLE_V2(BSP_STACK_ALIGNMENT);
void tx_startup_err_callback(void *p_instance, void *p_data);
void tx_startup_common_init(void);
#if (BSP_IRQ_DISABLED) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_transfer3) && !defined(SSP_SUPPRESS_ISR_DTCELC_EVENT_SCI8_RXI)
#define DTC_ACTIVATION_SRC_ELC_EVENT_SCI8_RXI
#if defined(DTC_ACTIVATION_SRC_ELC_EVENT_ELC_SOFTWARE_EVENT_0) && !defined(DTC_VECTOR_DEFINED_SOFTWARE_EVENT_0)
SSP_VECTOR_DEFINE(elc_software_event_isr, ELC, SOFTWARE_EVENT_0);
#define DTC_VECTOR_DEFINED_SOFTWARE_EVENT_0
#endif
#if defined(DTC_ACTIVATION_SRC_ELC_EVENT_ELC_SOFTWARE_EVENT_1) && !defined(DTC_VECTOR_DEFINED_SOFTWARE_EVENT_1)
SSP_VECTOR_DEFINE(elc_software_event_isr, ELC, SOFTWARE_EVENT_1);
#define DTC_VECTOR_DEFINED_SOFTWARE_EVENT_1
#endif
#endif
#endif

dtc_instance_ctrl_t g_transfer3_ctrl;
transfer_info_t g_transfer3_info = { .dest_addr_mode =
		TRANSFER_ADDR_MODE_INCREMENTED, .repeat_area =
		TRANSFER_REPEAT_AREA_DESTINATION, .irq = TRANSFER_IRQ_END, .chain_mode =
		TRANSFER_CHAIN_MODE_DISABLED, .src_addr_mode = TRANSFER_ADDR_MODE_FIXED,
		.size = TRANSFER_SIZE_1_BYTE, .mode = TRANSFER_MODE_NORMAL, .p_dest =
				(void*) NULL, .p_src = (void const*) NULL, .num_blocks = 0,
		.length = 0, };
const transfer_cfg_t g_transfer3_cfg = { .p_info = &g_transfer3_info,
		.activation_source = ELC_EVENT_SCI8_RXI, .auto_enable = false,
		.p_callback = NULL, .p_context = &g_transfer3, .irq_ipl =
				(BSP_IRQ_DISABLED) };
/* Instance structure to use this module. */
const transfer_instance_t g_transfer3 = { .p_ctrl = &g_transfer3_ctrl, .p_cfg =
		&g_transfer3_cfg, .p_api = &g_transfer_on_dtc };
#if (BSP_IRQ_DISABLED) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_transfer2) && !defined(SSP_SUPPRESS_ISR_DTCELC_EVENT_SCI8_TXI)
#define DTC_ACTIVATION_SRC_ELC_EVENT_SCI8_TXI
#if defined(DTC_ACTIVATION_SRC_ELC_EVENT_ELC_SOFTWARE_EVENT_0) && !defined(DTC_VECTOR_DEFINED_SOFTWARE_EVENT_0)
SSP_VECTOR_DEFINE(elc_software_event_isr, ELC, SOFTWARE_EVENT_0);
#define DTC_VECTOR_DEFINED_SOFTWARE_EVENT_0
#endif
#if defined(DTC_ACTIVATION_SRC_ELC_EVENT_ELC_SOFTWARE_EVENT_1) && !defined(DTC_VECTOR_DEFINED_SOFTWARE_EVENT_1)
SSP_VECTOR_DEFINE(elc_software_event_isr, ELC, SOFTWARE_EVENT_1);
#define DTC_VECTOR_DEFINED_SOFTWARE_EVENT_1
#endif
#endif
#endif

dtc_instance_ctrl_t g_transfer2_ctrl;
transfer_info_t g_transfer2_info = { .dest_addr_mode = TRANSFER_ADDR_MODE_FIXED,
		.repeat_area = TRANSFER_REPEAT_AREA_SOURCE, .irq = TRANSFER_IRQ_END,
		.chain_mode = TRANSFER_CHAIN_MODE_DISABLED, .src_addr_mode =
				TRANSFER_ADDR_MODE_INCREMENTED, .size = TRANSFER_SIZE_1_BYTE,
		.mode = TRANSFER_MODE_NORMAL, .p_dest = (void*) NULL, .p_src =
				(void const*) NULL, .num_blocks = 0, .length = 0, };
const transfer_cfg_t g_transfer2_cfg = { .p_info = &g_transfer2_info,
		.activation_source = ELC_EVENT_SCI8_TXI, .auto_enable = false,
		.p_callback = NULL, .p_context = &g_transfer2, .irq_ipl =
				(BSP_IRQ_DISABLED) };
/* Instance structure to use this module. */
const transfer_instance_t g_transfer2 = { .p_ctrl = &g_transfer2_ctrl, .p_cfg =
		&g_transfer2_cfg, .p_api = &g_transfer_on_dtc };
#if !defined(SSP_SUPPRESS_ISR_g_sci_spi8) && !defined(SSP_SUPPRESS_ISR_SCI8)
SSP_VECTOR_DEFINE_CHAN(sci_spi_rxi_isr, SCI, RXI, 8);
#endif
#if !defined(SSP_SUPPRESS_ISR_g_sci_spi8) && !defined(SSP_SUPPRESS_ISR_SCI8)
SSP_VECTOR_DEFINE_CHAN(sci_spi_txi_isr, SCI, TXI, 8);
#endif
#if !defined(SSP_SUPPRESS_ISR_g_sci_spi8) && !defined(SSP_SUPPRESS_ISR_SCI8)
SSP_VECTOR_DEFINE_CHAN(sci_spi_tei_isr, SCI, TEI, 8);
#endif
#if !defined(SSP_SUPPRESS_ISR_g_sci_spi8) && !defined(SSP_SUPPRESS_ISR_SCI8)
SSP_VECTOR_DEFINE_CHAN(sci_spi_eri_isr, SCI, ERI, 8);
#endif
sci_spi_instance_ctrl_t g_sci_spi8_ctrl;

/** SPI extended configuration */
const sci_spi_extended_cfg g_sci_spi8_cfg_extend =
		{ .bitrate_modulation = true };

const spi_cfg_t g_sci_spi8_cfg = { .channel = 8, .operating_mode =
		SPI_MODE_MASTER, .clk_phase = SPI_CLK_PHASE_EDGE_ODD, .clk_polarity =
		SPI_CLK_POLARITY_LOW, .mode_fault = SPI_MODE_FAULT_ERROR_DISABLE,
		.bit_order = SPI_BIT_ORDER_MSB_FIRST, .bitrate = 100000,
#define SYNERGY_NOT_DEFINED (1)             
#if (SYNERGY_NOT_DEFINED == g_transfer2)
                .p_transfer_tx       = NULL,
#else
		.p_transfer_tx = &g_transfer2,
#endif
#if (SYNERGY_NOT_DEFINED == g_transfer3)
                .p_transfer_rx       = NULL,
#else
		.p_transfer_rx = &g_transfer3,
#endif
#undef SYNERGY_NOT_DEFINED	
		.p_callback = sci_spi8_callback, .p_context = (void*) &g_sci_spi8,
		.rxi_ipl = (12), .txi_ipl = (12), .tei_ipl = (12), .eri_ipl = (12),
		.p_extend = &g_sci_spi8_cfg_extend, };
/* Instance structure to use this module. */
const spi_instance_t g_sci_spi8 = { .p_ctrl = &g_sci_spi8_ctrl, .p_cfg =
		&g_sci_spi8_cfg, .p_api = &g_spi_on_sci };
TX_EVENT_FLAGS_GROUP g_sci_spi8_event_flags;
extern bool g_ssp_common_initialized;
extern uint32_t g_ssp_common_thread_count;
extern TX_SEMAPHORE g_ssp_common_initialized_semaphore;

void accelerometer_thread_create(void) {
	/* Increment count so we will know the number of ISDE created threads. */
	g_ssp_common_thread_count++;

	/* Initialize each kernel object. */
	UINT err_g_sci_spi8_event_flags;
	err_g_sci_spi8_event_flags = tx_event_flags_create(&g_sci_spi8_event_flags,
			(CHAR*) "SCI SPI8 Event Flags");
	if (TX_SUCCESS != err_g_sci_spi8_event_flags) {
		tx_startup_err_callback(&g_sci_spi8_event_flags, 0);
	}

	UINT err;
	err = tx_thread_create(&accelerometer_thread,
			(CHAR*) "Accelerometer Thread", accelerometer_thread_func,
			(ULONG) NULL, &accelerometer_thread_stack, 1024, 6, 6, 1,
			TX_DONT_START);
	if (TX_SUCCESS != err) {
		tx_startup_err_callback(&accelerometer_thread, 0);
	}
}

static void accelerometer_thread_func(ULONG thread_input) {
	/* Not currently using thread_input. */
	SSP_PARAMETER_NOT_USED(thread_input);

	/* Initialize common components */
	tx_startup_common_init();

	/* Initialize each module instance. */

	/* Enter user code for this thread. */
	accelerometer_thread_entry();
}
