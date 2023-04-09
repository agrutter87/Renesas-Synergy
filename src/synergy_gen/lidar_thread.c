/* generated thread source file - do not edit */
#include "lidar_thread.h"

TX_THREAD lidar_thread;
void lidar_thread_create(void);
static void lidar_thread_func(ULONG thread_input);
static uint8_t lidar_thread_stack[1024] BSP_PLACE_IN_SECTION_V2(".stack.lidar_thread") BSP_ALIGN_VARIABLE_V2(BSP_STACK_ALIGNMENT);
void tx_startup_err_callback(void *p_instance, void *p_data);
void tx_startup_common_init(void);
#if (BSP_IRQ_DISABLED) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_transfer4) && !defined(SSP_SUPPRESS_ISR_DTCELC_EVENT_SCI7_TXI)
#define DTC_ACTIVATION_SRC_ELC_EVENT_SCI7_TXI
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

dtc_instance_ctrl_t g_transfer4_ctrl;
transfer_info_t g_transfer4_info = { .dest_addr_mode = TRANSFER_ADDR_MODE_FIXED,
		.repeat_area = TRANSFER_REPEAT_AREA_SOURCE, .irq = TRANSFER_IRQ_END,
		.chain_mode = TRANSFER_CHAIN_MODE_DISABLED, .src_addr_mode =
				TRANSFER_ADDR_MODE_INCREMENTED, .size = TRANSFER_SIZE_1_BYTE,
		.mode = TRANSFER_MODE_NORMAL, .p_dest = (void*) NULL, .p_src =
				(void const*) NULL, .num_blocks = 0, .length = 0, };
const transfer_cfg_t g_transfer4_cfg = { .p_info = &g_transfer4_info,
		.activation_source = ELC_EVENT_SCI7_TXI, .auto_enable = false,
		.p_callback = NULL, .p_context = &g_transfer4, .irq_ipl =
				(BSP_IRQ_DISABLED) };
/* Instance structure to use this module. */
const transfer_instance_t g_transfer4 = { .p_ctrl = &g_transfer4_ctrl, .p_cfg =
		&g_transfer4_cfg, .p_api = &g_transfer_on_dtc };
#if SCI_UART_CFG_RX_ENABLE
#if (12) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_uart7) && !defined(SSP_SUPPRESS_ISR_SCI7)
SSP_VECTOR_DEFINE_CHAN(sci_uart_rxi_isr, SCI, RXI, 7);
#endif
#endif
#endif
#if SCI_UART_CFG_TX_ENABLE
#if (12) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_uart7) && !defined(SSP_SUPPRESS_ISR_SCI7)
SSP_VECTOR_DEFINE_CHAN(sci_uart_txi_isr, SCI, TXI, 7);
#endif
#endif
#if (12) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_uart7) && !defined(SSP_SUPPRESS_ISR_SCI7)
SSP_VECTOR_DEFINE_CHAN(sci_uart_tei_isr, SCI, TEI, 7);
#endif
#endif
#endif
#if SCI_UART_CFG_RX_ENABLE
#if (12) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_uart7) && !defined(SSP_SUPPRESS_ISR_SCI7)
SSP_VECTOR_DEFINE_CHAN(sci_uart_eri_isr, SCI, ERI, 7);
#endif
#endif
#endif
sci_uart_instance_ctrl_t g_uart7_ctrl;

/** UART extended configuration for UARTonSCI HAL driver */
const uart_on_sci_cfg_t g_uart7_cfg_extend =
		{ .clk_src = SCI_CLK_SRC_INT, .baudclk_out = false, .rx_edge_start =
				true, .noisecancel_en = false, .p_extpin_ctrl = NULL,
				.bitrate_modulation = true, .rx_fifo_trigger =
						SCI_UART_RX_FIFO_TRIGGER_MAX, .baud_rate_error_x_1000 =
						(uint32_t)(2.0 * 1000), .uart_comm_mode =
						UART_MODE_RS232, .uart_rs485_mode = UART_RS485_HD,
				.rs485_de_pin = IOPORT_PORT_09_PIN_14, };

/** UART interface configuration */
const uart_cfg_t g_uart7_cfg = { .channel = 7, .baud_rate = 115200, .data_bits =
		UART_DATA_BITS_8, .parity = UART_PARITY_OFF, .stop_bits =
		UART_STOP_BITS_1, .ctsrts_en = false, .p_callback = NULL, .p_context =
		&g_uart7, .p_extend = &g_uart7_cfg_extend,
#define SYNERGY_NOT_DEFINED (1)                        
#if (SYNERGY_NOT_DEFINED == g_transfer4)
                .p_transfer_tx       = NULL,
#else
		.p_transfer_tx = &g_transfer4,
#endif            
#if (SYNERGY_NOT_DEFINED == SYNERGY_NOT_DEFINED)
		.p_transfer_rx = NULL,
#else
                .p_transfer_rx       = &SYNERGY_NOT_DEFINED,
#endif   
#undef SYNERGY_NOT_DEFINED            
		.rxi_ipl = (12), .txi_ipl = (12), .tei_ipl = (12), .eri_ipl = (12), };

/* Instance structure to use this module. */
const uart_instance_t g_uart7 = { .p_ctrl = &g_uart7_ctrl,
		.p_cfg = &g_uart7_cfg, .p_api = &g_uart_on_sci };
#if defined(__ICCARM__)
#define g_sf_uart7_err_callback_WEAK_ATTRIBUTE
#pragma weak g_sf_uart7_err_callback  = g_sf_uart7_err_callback_internal
#elif defined(__GNUC__)
#define g_sf_uart7_err_callback_WEAK_ATTRIBUTE   __attribute__ ((weak, alias("g_sf_uart7_err_callback_internal")))
#endif
void g_sf_uart7_err_callback(void *p_instance, void *p_data)
g_sf_uart7_err_callback_WEAK_ATTRIBUTE;
static sf_uart_comms_instance_ctrl_t g_sf_uart7_ctrl;
static const sf_uart_comms_cfg_t g_sf_uart7_cfg_extend = { .p_lower_lvl_uart =
		&g_uart7, };
static const sf_comms_cfg_t g_sf_uart7_cfg = { .p_extend =
		&g_sf_uart7_cfg_extend, };
/* Instance structure to use this module. */
const sf_comms_instance_t g_sf_uart7 = { .p_ctrl = &g_sf_uart7_ctrl, .p_cfg =
		&g_sf_uart7_cfg, .p_api = &g_sf_comms_on_sf_uart_comms };
/*******************************************************************************************************************//**
 * @brief      This is a weak example initialization error function.  It should be overridden by defining a user  function 
 *             with the prototype below.
 *             - void g_sf_uart7_err_callback(void * p_instance, void * p_data)
 *
 * @param[in]  p_instance arguments used to identify which instance caused the error and p_data Callback arguments used to identify what error caused the callback.
 **********************************************************************************************************************/
void g_sf_uart7_err_callback_internal(void *p_instance, void *p_data);
void g_sf_uart7_err_callback_internal(void *p_instance, void *p_data) {
	/** Suppress compiler warning for not using parameters. */
	SSP_PARAMETER_NOT_USED(p_instance);
	SSP_PARAMETER_NOT_USED(p_data);

	/** An error has occurred. Please check function arguments for more information. */
	BSP_CFG_HANDLE_UNRECOVERABLE_ERROR(0);
}
/*******************************************************************************************************************//**
 * @brief     Initialization function that the user can choose to have called automatically during thread entry.
 *            The user can call this function at a later time if desired using the prototype below.
 *            - void sf_uart_init(void)
 **********************************************************************************************************************/
void sf_uart_init(void) {
	ssp_err_t ssp_err_g_sf_uart7;
	ssp_err_g_sf_uart7 = g_sf_uart7.p_api->open(g_sf_uart7.p_ctrl,
			g_sf_uart7.p_cfg);
	if (SSP_SUCCESS != ssp_err_g_sf_uart7) {
		g_sf_uart7_err_callback((void*) &g_sf_uart7, &ssp_err_g_sf_uart7);
	}
}
extern bool g_ssp_common_initialized;
extern uint32_t g_ssp_common_thread_count;
extern TX_SEMAPHORE g_ssp_common_initialized_semaphore;

void lidar_thread_create(void) {
	/* Increment count so we will know the number of ISDE created threads. */
	g_ssp_common_thread_count++;

	/* Initialize each kernel object. */

	UINT err;
	err = tx_thread_create(&lidar_thread, (CHAR*) "LiDAR Thread",
			lidar_thread_func, (ULONG) NULL, &lidar_thread_stack, 1024, 9, 9, 1,
			TX_DONT_START);
	if (TX_SUCCESS != err) {
		tx_startup_err_callback(&lidar_thread, 0);
	}
}

static void lidar_thread_func(ULONG thread_input) {
	/* Not currently using thread_input. */
	SSP_PARAMETER_NOT_USED(thread_input);

	/* Initialize common components */
	tx_startup_common_init();

	/* Initialize each module instance. */
	/** Call initialization function if user has selected to do so. */
#if (1)
	sf_uart_init();
#endif

	/* Enter user code for this thread. */
	lidar_thread_entry();
}
