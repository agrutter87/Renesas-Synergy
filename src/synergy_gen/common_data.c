/* generated common source file - do not edit */
#include "common_data.h"

#define UX_HOST_INITIALIZE
#define USB_HOST_VIDEO_CLASS_REGISTER
#define USB_HOST_HUB_CLASS_REGISTER
#define USB_HOST_HID_CLASS_REGISTER
#define USB_HOST_STORAGE_CLASS_REGISTER
#define UX_DCD_SYNERGY_USBFS_VECTOR

dac_instance_ctrl_t g_audio_dac_ctrl;
const dac_extended_cfg_t g_audio_dac_ex_ctrl = { .enable_charge_pump = false, };
const dac_cfg_t g_audio_dac_cfg = { .channel = 0, .ad_da_synchronized = false,
		.data_format = DAC_DATA_FORMAT_FLUSH_RIGHT, .output_amplifier_enabled =
				false, .p_extend = &g_audio_dac_ex_ctrl };
/* Instance structure to use this module. */
const dac_instance_t g_audio_dac = { .p_ctrl = &g_audio_dac_ctrl, .p_cfg =
		&g_audio_dac_cfg, .p_api = &g_dac_on_dac };
#if (8) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_audio_timer) && !defined(SSP_SUPPRESS_ISR_GPT0)
SSP_VECTOR_DEFINE_CHAN(gpt_counter_overflow_isr, GPT, COUNTER_OVERFLOW, 0);
#endif
#endif
static gpt_instance_ctrl_t g_audio_timer_ctrl;
static const timer_on_gpt_cfg_t g_audio_timer_extend = { .gtioca = {
		.output_enabled = false, .stop_level = GPT_PIN_LEVEL_LOW }, .gtiocb = {
		.output_enabled = false, .stop_level = GPT_PIN_LEVEL_LOW },
		.shortest_pwm_signal = GPT_SHORTEST_LEVEL_OFF, };
static const timer_cfg_t g_audio_timer_cfg = { .mode = TIMER_MODE_PERIODIC,
		.period = 44100, .unit = TIMER_UNIT_FREQUENCY_HZ, .duty_cycle = 50,
		.duty_cycle_unit = TIMER_PWM_UNIT_RAW_COUNTS, .channel = 0, .autostart =
				false, .p_callback = NULL, .p_context = &g_audio_timer,
		.p_extend = &g_audio_timer_extend, .irq_ipl = (8), };
/* Instance structure to use this module. */
const timer_instance_t g_audio_timer = { .p_ctrl = &g_audio_timer_ctrl, .p_cfg =
		&g_audio_timer_cfg, .p_api = &g_timer_on_gpt };
#if (BSP_IRQ_DISABLED) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_audio_transfer) && !defined(SSP_SUPPRESS_ISR_DTCELC_EVENT_GPT0_COUNTER_OVERFLOW)
#define DTC_ACTIVATION_SRC_ELC_EVENT_GPT0_COUNTER_OVERFLOW
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

dtc_instance_ctrl_t g_audio_transfer_ctrl;
transfer_info_t g_audio_transfer_info = { .dest_addr_mode =
		TRANSFER_ADDR_MODE_FIXED, .repeat_area = TRANSFER_REPEAT_AREA_SOURCE,
		.irq = TRANSFER_IRQ_END, .chain_mode = TRANSFER_CHAIN_MODE_DISABLED,
		.src_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED, .size =
				TRANSFER_SIZE_2_BYTE, .mode = TRANSFER_MODE_NORMAL, .p_dest =
				(void*) &R_DAC->DADRn[0], .p_src = (void const*) NULL,
		.num_blocks = 0, .length = 0, };
const transfer_cfg_t g_audio_transfer_cfg = { .p_info = &g_audio_transfer_info,
		.activation_source = ELC_EVENT_GPT0_COUNTER_OVERFLOW, .auto_enable =
				false, .p_callback = NULL, .p_context = &g_audio_transfer,
		.irq_ipl = (BSP_IRQ_DISABLED) };
/* Instance structure to use this module. */
const transfer_instance_t g_audio_transfer = { .p_ctrl = &g_audio_transfer_ctrl,
		.p_cfg = &g_audio_transfer_cfg, .p_api = &g_transfer_on_dtc };
static sf_audio_playback_hw_dac_instance_ctrl_t g_sf_audio_playback_hw_ctrl;
static const sf_audio_playback_hw_dac_cfg_t g_sf_audio_playback_hw_cfg_extend =
		{ .p_lower_lvl_timer = &g_audio_timer,
#define SYNERGY_NOT_DEFINED (1)
#ifdef g_audio_dac
    .p_lower_lvl_dac = &SYNERGY_NOT_DEFINED,
#else
				.p_lower_lvl_dac = &g_audio_dac,
#endif
#undef SYNERGY_NOT_DEFINED
				.p_lower_lvl_transfer = &g_audio_transfer, };
static const sf_audio_playback_hw_cfg_t g_sf_audio_playback_hw_cfg = {
		.p_extend = &g_sf_audio_playback_hw_cfg_extend };
const sf_audio_playback_hw_instance_t g_sf_audio_playback_hw = { .p_api =
		&g_sf_audio_playback_hw_on_sf_audio_playback_hw_dac, .p_ctrl =
		&g_sf_audio_playback_hw_ctrl, .p_cfg = &g_sf_audio_playback_hw_cfg, };
#if defined(__ICCARM__)
            #define g_sf_message_err_callback_WEAK_ATTRIBUTE
            #pragma weak g_sf_message_err_callback  = g_sf_message_err_callback_internal
            #elif defined(__GNUC__)
            #define g_sf_message_err_callback_WEAK_ATTRIBUTE   __attribute__ ((weak, alias("g_sf_message_err_callback_internal")))
            #endif
void g_sf_message_err_callback(void *p_instance, void *p_data)
g_sf_message_err_callback_WEAK_ATTRIBUTE;
extern sf_message_subscriber_list_t *p_subscriber_lists[];
sf_message_instance_ctrl_t g_sf_message_ctrl;
static uint8_t g_sf_message_work_buffer[2048];
/* Configures the messaging framework */
sf_message_cfg_t g_sf_message_cfg = { .p_work_memory_start =
		&g_sf_message_work_buffer, .work_memory_size_bytes = 2048,
		.buffer_size = sizeof(sf_message_payload_t), .pp_subscriber_lists =
				p_subscriber_lists, .p_block_pool_name =
				(uint8_t*) "sf_msg_blk_pool" };
/* Instance structure to use this module. */
const sf_message_instance_t g_sf_message = { .p_ctrl = &g_sf_message_ctrl,
		.p_cfg = &g_sf_message_cfg, .p_api = &g_sf_message_on_sf_message };
/*******************************************************************************************************************//**
 * @brief      This is a weak example initialization error function.  It should be overridden by defining a user  function 
 *             with the prototype below.
 *             - void g_sf_message_err_callback(void * p_instance, void * p_data)
 *
 * @param[in]  p_instance arguments used to identify which instance caused the error and p_data Callback arguments used to identify what error caused the callback.
 **********************************************************************************************************************/
void g_sf_message_err_callback_internal(void *p_instance, void *p_data);
void g_sf_message_err_callback_internal(void *p_instance, void *p_data) {
	/** Suppress compiler warning for not using parameters. */
	SSP_PARAMETER_NOT_USED(p_instance);
	SSP_PARAMETER_NOT_USED(p_data);

	/** An error has occurred. Please check function arguments for more information. */
	BSP_CFG_HANDLE_UNRECOVERABLE_ERROR(0);
}
/*******************************************************************************************************************//**
 * @brief     Initialization function that the user can choose to have called automatically during thread entry.
 *            The user can call this function at a later time if desired using the prototype below.

 *            - void sf_message_init0(void)
 **********************************************************************************************************************/
void sf_message_init0(void) {
	ssp_err_t ssp_err_g_sf_message;

	/* Initializes Messaging Framework Queues */
	g_message_init();

	/* Opens the messaging framework */
	ssp_err_g_sf_message = g_sf_message.p_api->open(g_sf_message.p_ctrl,
			g_sf_message.p_cfg);
	if (SSP_SUCCESS != ssp_err_g_sf_message) {
		/* Error returns, check the cause. */
		g_sf_message_err_callback((void*) &g_sf_message, &ssp_err_g_sf_message);
	}
}
extern TX_QUEUE sf_audio_dummy_message_queue;
extern const sf_message_instance_t g_sf_message;
sf_audio_playback_common_instance_ctrl_t g_sf_audio_playback_common;
const sf_audio_playback_common_cfg_t g_sf_audio_playback_common_cfg = {
		.p_lower_lvl_hw = &g_sf_audio_playback_hw, .priority = 3, .p_message =
				&g_sf_message, .p_queue = &sf_audio_dummy_message_queue, };
#define FX_COMMON_INITIALIZE (1)
/*******************************************************************************************************************//**
 * @brief     Initialization function that the user can choose to have called automatically during thread entry.
 *            The user can call this function at a later time if desired using the prototype below.
 *            - void fx_common_init0(void)
 **********************************************************************************************************************/
void fx_common_init0(void) {
	/** Initialize the FileX system. */
	fx_system_initialize();
}
#if (2) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_transfer10) && !defined(SSP_SUPPRESS_ISR_DMACELC_EVENT_ELC_SOFTWARE_EVENT_1)
SSP_VECTOR_DEFINE_CHAN(dmac_int_isr, DMAC, INT, 1);
#endif
#endif
dmac_instance_ctrl_t g_transfer10_ctrl;
transfer_info_t g_transfer10_info = { .dest_addr_mode =
		TRANSFER_ADDR_MODE_INCREMENTED, .repeat_area =
		TRANSFER_REPEAT_AREA_DESTINATION, .irq = TRANSFER_IRQ_EACH,
		.chain_mode = TRANSFER_CHAIN_MODE_DISABLED, .src_addr_mode =
				TRANSFER_ADDR_MODE_FIXED, .size = TRANSFER_SIZE_1_BYTE, .mode =
				TRANSFER_MODE_BLOCK, .p_dest = (void*) NULL, .p_src =
				(void const*) NULL, .num_blocks = 0, .length = 0, };
const transfer_on_dmac_cfg_t g_transfer10_extend = { .channel = 1,
		.offset_byte = 0, };
const transfer_cfg_t g_transfer10_cfg = { .p_info = &g_transfer10_info,
		.activation_source = ELC_EVENT_ELC_SOFTWARE_EVENT_1, .auto_enable =
				false, .p_callback = NULL, .p_context = &g_transfer10,
		.irq_ipl = (2), .p_extend = &g_transfer10_extend, };
/* Instance structure to use this module. */
const transfer_instance_t g_transfer10 = { .p_ctrl = &g_transfer10_ctrl,
		.p_cfg = &g_transfer10_cfg, .p_api = &g_transfer_on_dmac };
#if (2) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_transfer9) && !defined(SSP_SUPPRESS_ISR_DMACELC_EVENT_ELC_SOFTWARE_EVENT_0)
SSP_VECTOR_DEFINE_CHAN(dmac_int_isr, DMAC, INT, 0);
#endif
#endif
dmac_instance_ctrl_t g_transfer9_ctrl;
transfer_info_t g_transfer9_info = { .dest_addr_mode = TRANSFER_ADDR_MODE_FIXED,
		.repeat_area = TRANSFER_REPEAT_AREA_SOURCE, .irq = TRANSFER_IRQ_EACH,
		.chain_mode = TRANSFER_CHAIN_MODE_DISABLED, .src_addr_mode =
				TRANSFER_ADDR_MODE_INCREMENTED, .size = TRANSFER_SIZE_1_BYTE,
		.mode = TRANSFER_MODE_BLOCK, .p_dest = (void*) NULL, .p_src =
				(void const*) NULL, .num_blocks = 0, .length = 0, };
const transfer_on_dmac_cfg_t g_transfer9_extend = { .channel = 0, .offset_byte =
		0, };
const transfer_cfg_t g_transfer9_cfg = { .p_info = &g_transfer9_info,
		.activation_source = ELC_EVENT_ELC_SOFTWARE_EVENT_0, .auto_enable =
				false, .p_callback = NULL, .p_context = &g_transfer9, .irq_ipl =
				(2), .p_extend = &g_transfer9_extend, };
/* Instance structure to use this module. */
const transfer_instance_t g_transfer9 = { .p_ctrl = &g_transfer9_ctrl, .p_cfg =
		&g_transfer9_cfg, .p_api = &g_transfer_on_dmac };
/***********************************************************************************************************************
 * Registers Interrupt Vector for USBHS Controller.
 **********************************************************************************************************************/
#if (SF_EL_UX_HCD_CFG_HS_IRQ_IPL != BSP_IRQ_DISABLED)
            /* USBHS ISR vector registering. */
            #if !defined(SSP_SUPPRESS_ISR_g_sf_el_ux_hcd_hs_0) && !defined(SSP_SUPPRESS_ISR_USB)
            #if !defined(UX_DCD_SYNERGY_USBHS_VECTOR)
            SSP_VECTOR_DEFINE_UNIT(usbhs_usb_int_resume_isr, USB, HS, USB_INT_RESUME, 0);
            #endif
            #endif
            #endif

#undef SYNERGY_NOT_DEFINED
#define SYNERGY_NOT_DEFINED (1)
/***********************************************************************************************************************
 * The definition of wrapper interface for USBX Synergy Port HCD Driver.
 **********************************************************************************************************************/
static UINT g_sf_el_ux_hcd_hs_0_initialize(UX_HCD *hcd) {
#if ((SYNERGY_NOT_DEFINED != g_transfer9) && (SYNERGY_NOT_DEFINED != g_transfer10))
	/* DMA support */
	UX_HCD_SYNERGY_TRANSFER hcd_transfer;
	hcd_transfer.ux_synergy_transfer_tx = (transfer_instance_t*) &g_transfer9;
	hcd_transfer.ux_synergy_transfer_rx = (transfer_instance_t*) &g_transfer10;
	return (UINT) ux_hcd_synergy_initialize_transfer_support(hcd,
			(UX_HCD_SYNERGY_TRANSFER*) &hcd_transfer);
#else
                /* Non DMA support */
                return (UINT)ux_hcd_synergy_initialize(hcd);
            #endif
} /* End of function g_sf_el_ux_hcd_hs_0_initialize() */
#undef SYNERGY_NOT_DEFINED
#if (BSP_IRQ_DISABLED) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_transfer8) && !defined(SSP_SUPPRESS_ISR_DTCELC_EVENT_IIC1_RXI)
#define DTC_ACTIVATION_SRC_ELC_EVENT_IIC1_RXI
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

dtc_instance_ctrl_t g_transfer8_ctrl;
transfer_info_t g_transfer8_info = { .dest_addr_mode =
		TRANSFER_ADDR_MODE_INCREMENTED, .repeat_area =
		TRANSFER_REPEAT_AREA_DESTINATION, .irq = TRANSFER_IRQ_END, .chain_mode =
		TRANSFER_CHAIN_MODE_DISABLED, .src_addr_mode = TRANSFER_ADDR_MODE_FIXED,
		.size = TRANSFER_SIZE_1_BYTE, .mode = TRANSFER_MODE_NORMAL, .p_dest =
				(void*) NULL, .p_src = (void const*) NULL, .num_blocks = 0,
		.length = 0, };
const transfer_cfg_t g_transfer8_cfg = { .p_info = &g_transfer8_info,
		.activation_source = ELC_EVENT_IIC1_RXI, .auto_enable = false,
		.p_callback = NULL, .p_context = &g_transfer8, .irq_ipl =
				(BSP_IRQ_DISABLED) };
/* Instance structure to use this module. */
const transfer_instance_t g_transfer8 = { .p_ctrl = &g_transfer8_ctrl, .p_cfg =
		&g_transfer8_cfg, .p_api = &g_transfer_on_dtc };
#if (BSP_IRQ_DISABLED) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_transfer7) && !defined(SSP_SUPPRESS_ISR_DTCELC_EVENT_IIC1_TXI)
#define DTC_ACTIVATION_SRC_ELC_EVENT_IIC1_TXI
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

dtc_instance_ctrl_t g_transfer7_ctrl;
transfer_info_t g_transfer7_info = { .dest_addr_mode = TRANSFER_ADDR_MODE_FIXED,
		.repeat_area = TRANSFER_REPEAT_AREA_SOURCE, .irq = TRANSFER_IRQ_END,
		.chain_mode = TRANSFER_CHAIN_MODE_DISABLED, .src_addr_mode =
				TRANSFER_ADDR_MODE_INCREMENTED, .size = TRANSFER_SIZE_1_BYTE,
		.mode = TRANSFER_MODE_NORMAL, .p_dest = (void*) NULL, .p_src =
				(void const*) NULL, .num_blocks = 0, .length = 0, };
const transfer_cfg_t g_transfer7_cfg = { .p_info = &g_transfer7_info,
		.activation_source = ELC_EVENT_IIC1_TXI, .auto_enable = false,
		.p_callback = NULL, .p_context = &g_transfer7, .irq_ipl =
				(BSP_IRQ_DISABLED) };
/* Instance structure to use this module. */
const transfer_instance_t g_transfer7 = { .p_ctrl = &g_transfer7_ctrl, .p_cfg =
		&g_transfer7_cfg, .p_api = &g_transfer_on_dtc };
#if !defined(SSP_SUPPRESS_ISR_g_i2c0) && !defined(SSP_SUPPRESS_ISR_IIC1)
SSP_VECTOR_DEFINE_CHAN(iic_rxi_isr, IIC, RXI, 1);
#endif
#if !defined(SSP_SUPPRESS_ISR_g_i2c0) && !defined(SSP_SUPPRESS_ISR_IIC1)
SSP_VECTOR_DEFINE_CHAN(iic_txi_isr, IIC, TXI, 1);
#endif
#if !defined(SSP_SUPPRESS_ISR_g_i2c0) && !defined(SSP_SUPPRESS_ISR_IIC1)
SSP_VECTOR_DEFINE_CHAN(iic_tei_isr, IIC, TEI, 1);
#endif
#if !defined(SSP_SUPPRESS_ISR_g_i2c0) && !defined(SSP_SUPPRESS_ISR_IIC1)
SSP_VECTOR_DEFINE_CHAN(iic_eri_isr, IIC, ERI, 1);
#endif
riic_instance_ctrl_t g_i2c0_ctrl;
const riic_extended_cfg g_i2c0_extend = { .timeout_mode =
		RIIC_TIMEOUT_MODE_SHORT, };
const i2c_cfg_t g_i2c0_cfg = { .channel = 1, .rate = I2C_RATE_FAST, .slave = 0,
		.addr_mode = I2C_ADDR_MODE_7BIT, .sda_delay = 300,
#define SYNERGY_NOT_DEFINED (1)            
#if (SYNERGY_NOT_DEFINED == g_transfer7)
                .p_transfer_tx       = NULL,
#else
		.p_transfer_tx = &g_transfer7,
#endif
#if (SYNERGY_NOT_DEFINED == g_transfer8)
                .p_transfer_rx       = NULL,
#else
		.p_transfer_rx = &g_transfer8,
#endif
#undef SYNERGY_NOT_DEFINED	
		.p_callback = NULL, .p_context = (void*) &g_i2c0, .rxi_ipl = (3),
		.txi_ipl = (3), .tei_ipl = (3), .eri_ipl = (3), .p_extend =
				&g_i2c0_extend, };
/* Instance structure to use this module. */
const i2c_master_instance_t g_i2c0 = { .p_ctrl = &g_i2c0_ctrl, .p_cfg =
		&g_i2c0_cfg, .p_api = &g_i2c_master_on_riic };
static TX_MUTEX sf_bus_mutex_g_sf_i2c_bus0;
static TX_EVENT_FLAGS_GROUP sf_bus_eventflag_g_sf_i2c_bus0;
static sf_i2c_instance_ctrl_t *sf_curr_ctrl_g_sf_i2c_bus0;
static sf_i2c_instance_ctrl_t *sf_curr_bus_ctrl_g_sf_i2c_bus0;
sf_i2c_bus_t g_sf_i2c_bus0 = { .p_bus_name = (uint8_t*) "g_sf_i2c_bus0",
		.channel = 1, .p_lock_mutex = &sf_bus_mutex_g_sf_i2c_bus0,
		.p_sync_eventflag = &sf_bus_eventflag_g_sf_i2c_bus0, .pp_curr_ctrl =
				(sf_i2c_ctrl_t**) &sf_curr_ctrl_g_sf_i2c_bus0,
		.p_lower_lvl_api = (i2c_api_master_t*) &g_i2c_master_on_riic,
		.device_count = 0, .pp_curr_bus_ctrl =
				(sf_i2c_ctrl_t**) &sf_curr_bus_ctrl_g_sf_i2c_bus0, };
#if SYNERGY_NOT_DEFINED != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_DRW)
SSP_VECTOR_DEFINE(drw_int_isr, DRW, INT);
#endif
#endif
SSP_VECTOR_DEFINE( jpeg_jdti_isr, JPEG, JDTI);
SSP_VECTOR_DEFINE( jpeg_jedi_isr, JPEG, JEDI);
static jpeg_decode_instance_ctrl_t g_jpeg_decode0_ctrl;
const jpeg_decode_cfg_t g_jpeg_decode0_cfg = { .input_data_format =
		JPEG_DECODE_DATA_FORMAT_NORMAL, .output_data_format =
		JPEG_DECODE_DATA_FORMAT_NORMAL, .pixel_format =
		JPEG_DECODE_PIXEL_FORMAT_RGB565, .alpha_value = 255, .p_callback = NULL,
		.jedi_ipl = (3), .jdti_ipl = (3), };
const jpeg_decode_instance_t g_jpeg_decode0 = { .p_api =
		(jpeg_decode_api_t const*) &g_jpeg_decode_on_jpeg_decode, .p_ctrl =
		&g_jpeg_decode0_ctrl, .p_cfg = &g_jpeg_decode0_cfg };
static sf_jpeg_decode_instance_ctrl_t g_sf_jpeg_decode0_ctrl;

static const sf_jpeg_decode_cfg_t g_sf_jpeg_decode0_cfg = {
		.p_lower_lvl_jpeg_decode =
				(jpeg_decode_instance_t const*) &g_jpeg_decode0 };
const sf_jpeg_decode_instance_t g_sf_jpeg_decode0 = { .p_api =
		&g_sf_jpeg_decode_on_sf_jpeg_decode, .p_ctrl = &g_sf_jpeg_decode0_ctrl,
		.p_cfg = (sf_jpeg_decode_cfg_t const*) &g_sf_jpeg_decode0_cfg };
#if (3) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_display0) && !defined(SSP_SUPPRESS_ISR_GLCD)
SSP_VECTOR_DEFINE( glcdc_line_detect_isr, GLCDC, LINE_DETECT);
#endif
#endif
#if (3) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_display0) && !defined(SSP_SUPPRESS_ISR_GLCD)
SSP_VECTOR_DEFINE( glcdc_underflow_1_isr, GLCDC, UNDERFLOW_1);
#endif
#endif
#if (BSP_IRQ_DISABLED) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_display0) && !defined(SSP_SUPPRESS_ISR_GLCD)
SSP_VECTOR_DEFINE(glcdc_underflow_2_isr, GLCDC, UNDERFLOW_2);
#endif
#endif

/** Display frame buffer */
#if (true)
        uint8_t g_display0_fb_background[2][((800 * 480) * DISPLAY_BITS_PER_PIXEL_INPUT0) >> 3] BSP_ALIGN_VARIABLE_V2(64) BSP_PLACE_IN_SECTION_V2(".sdram");
        #else
/** Graphics screen1 is specified not to be used when starting */
#endif
#if (false)
        uint8_t g_display0_fb_foreground[2][((800 * 480) * DISPLAY_BITS_PER_PIXEL_INPUT1) >> 3] BSP_ALIGN_VARIABLE_V2(64) BSP_PLACE_IN_SECTION_V2(".sdram");
        #else
/** Graphics screen2 is specified not to be used when starting */
#endif

#if (false)
        /** Display CLUT buffer to be used for updating CLUT */
        static uint32_t CLUT_buffer[256];

        /** Display CLUT configuration(only used if using CLUT format) */
        display_clut_cfg_t g_display0_clut_cfg_glcd =
        {
            .p_base              = (uint32_t *)CLUT_buffer,
            .start               = 0,   /* User have to update this setting when using */
            .size                = 256  /* User have to update this setting when using */
        };
        #else
/** CLUT is specified not to be used */
#endif

#if (false | false | false)
        /** Display interface configuration */
        static const display_gamma_correction_t g_display0_gamma_cfg =
        {
            .r =
            {
                .enable      = false,
                .gain        = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                .threshold   = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
            },
            .g =
            {
                .enable      = false,
                .gain        = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                .threshold   = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
            },
            .b =
            {
                .enable      = false,
                .gain        = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                .threshold   = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
            }
        };
        #endif

/** Display device extended configuration */
static const glcd_cfg_t g_display0_extend_cfg = { .tcon_hsync = GLCD_TCON_PIN_0,
		.tcon_vsync = GLCD_TCON_PIN_1, .tcon_de = GLCD_TCON_PIN_2,
		.correction_proc_order =
				GLCD_CORRECTION_PROC_ORDER_BRIGHTNESS_CONTRAST2GAMMA, .clksrc =
				GLCD_CLK_SRC_INTERNAL, .clock_div_ratio =
				GLCD_PANEL_CLK_DIVISOR_8, .dithering_mode =
				GLCD_DITHERING_MODE_TRUNCATE, .dithering_pattern_A =
				GLCD_DITHERING_PATTERN_11, .dithering_pattern_B =
				GLCD_DITHERING_PATTERN_11, .dithering_pattern_C =
				GLCD_DITHERING_PATTERN_11, .dithering_pattern_D =
				GLCD_DITHERING_PATTERN_11 };

/** Display control block instance */
glcd_instance_ctrl_t g_display0_ctrl;

/** Display interface configuration */
const display_cfg_t g_display0_cfg =
		{
		/** Input1(Graphics1 screen) configuration */
		.input[0] =
		{
#if (true)
                .p_base              = (uint32_t *)&g_display0_fb_background[0],
                #else
				.p_base = NULL,
#endif
				.hsize = 800, .vsize = 480, .hstride = 800, .format =
						DISPLAY_IN_FORMAT_16BITS_RGB565,
				.line_descending_enable = false, .lines_repeat_enable = false,
				.lines_repeat_times = 0 },

		/** Input2(Graphics2 screen) configuration */
		.input[1] =
		{
#if (false)
                .p_base              = (uint32_t *)&g_display0_fb_foreground[0],
                #else
				.p_base = NULL,
#endif
				.hsize = 800, .vsize = 480, .hstride = 800, .format =
						DISPLAY_IN_FORMAT_16BITS_RGB565,
				.line_descending_enable = false, .lines_repeat_enable = false,
				.lines_repeat_times = 0 },

		/** Input1(Graphics1 screen) layer configuration */
		.layer[0] =
		{ .coordinate = { .x = 0, .y = 0 }, .bg_color = { .byte = { .a = 255,
				.r = 255, .g = 255, .b = 255 } }, .fade_control =
				DISPLAY_FADE_CONTROL_NONE, .fade_speed = 0 },

		/** Input2(Graphics2 screen) layer configuration */
		.layer[1] =
		{ .coordinate = { .x = 0, .y = 0 }, .bg_color = { .byte = { .a = 255,
				.r = 255, .g = 255, .b = 255 } }, .fade_control =
				DISPLAY_FADE_CONTROL_NONE, .fade_speed = 0 },

				/** Output configuration */
				.output =
						{ .htiming = { .total_cyc = 1024, .display_cyc = 800,
								.back_porch = 46, .sync_width = 20,
								.sync_polarity =
										DISPLAY_SIGNAL_POLARITY_LOACTIVE },
								.vtiming =
										{ .total_cyc = 525, .display_cyc = 480,
												.back_porch = 23, .sync_width =
														10,
												.sync_polarity =
														DISPLAY_SIGNAL_POLARITY_LOACTIVE },
								.format = DISPLAY_OUT_FORMAT_24BITS_RGB888,
								.endian = DISPLAY_ENDIAN_LITTLE, .color_order =
										DISPLAY_COLOR_ORDER_RGB,
								.data_enable_polarity =
										DISPLAY_SIGNAL_POLARITY_HIACTIVE,
								.sync_edge = DISPLAY_SIGNAL_SYNC_EDGE_RISING,
								.bg_color = { .byte = { .a = 255, .r = 0,
										.g = 0, .b = 0 } }, .brightness = {
										.enable = false, .r = 512, .g = 512,
										.b = 512 }, .contrast = { .enable =
										false, .r = 128, .g = 128, .b = 128 },
#if (false | false | false)
                .p_gamma_correction  = (display_gamma_correction_t *)(&g_display0_gamma_cfg),
#else
								.p_gamma_correction = NULL,
#endif
								.dithering_on = false },

				/** Display device callback function pointer */
				.p_callback = NULL, .p_context = (void*) &g_display0,

				/** Display device extended configuration */
				.p_extend = (void*) (&g_display0_extend_cfg),

				.line_detect_ipl = (3), .underflow_1_ipl = (3),
				.underflow_2_ipl = (BSP_IRQ_DISABLED), };

#if (true)
        /** Display on GLCD run-time configuration(for the graphics1 screen) */
        display_runtime_cfg_t g_display0_runtime_cfg_bg =
        {
            .input =
            {
                #if (true)
                .p_base              = (uint32_t *)&g_display0_fb_background[0],
                #else
                .p_base              = NULL,
                #endif
                .hsize               = 800,
                .vsize               = 480,
                .hstride             = 800,
                .format              = DISPLAY_IN_FORMAT_16BITS_RGB565,
                .line_descending_enable = false,
                .lines_repeat_enable = false,
                .lines_repeat_times  = 0
            },
            .layer =
            {
                .coordinate = {
                        .x           = 0,
                        .y           = 0
                },
                .bg_color            =
                {
                    .byte            =
                    {
                        .a           = 255,
                        .r           = 255,
                        .g           = 255,
                        .b           = 255
                    }
                },
                .fade_control        = DISPLAY_FADE_CONTROL_NONE,
                .fade_speed          = 0
            }
        };
#endif
#if (false)
        /** Display on GLCD run-time configuration(for the graphics2 screen) */
        display_runtime_cfg_t g_display0_runtime_cfg_fg =
        {
            .input =
            {
                #if (false)
                .p_base              = (uint32_t *)&g_display0_fb_foreground[0],
                #else
                .p_base              = NULL,
                #endif
                .hsize               = 800,
                .vsize               = 480,
                .hstride             = 800,
                .format              = DISPLAY_IN_FORMAT_16BITS_RGB565,
                .line_descending_enable = false,
                .lines_repeat_enable = false,
                .lines_repeat_times  = 0
             },
            .layer =
            {
                .coordinate = {
                        .x           = 0,
                        .y           = 0
                },
                .bg_color            =
                {
                    .byte            =
                    {
                        .a           = 255,
                        .r           = 255,
                        .g           = 255,
                        .b           = 255
                    }
                },
                .fade_control        = DISPLAY_FADE_CONTROL_NONE,
                .fade_speed          = 0
            }
        };
#endif

/* Instance structure to use this module. */
const display_instance_t g_display0 = { .p_ctrl = &g_display0_ctrl, .p_cfg =
		(display_cfg_t*) &g_display0_cfg, .p_api =
		(display_api_t*) &g_display_on_glcd };
/** GUIX Canvas Buffer */
#if false
            #if (1 == 1) /* Inherit Frame Buffer Name from Graphics Screen 1 */
            uint8_t g_sf_el_gx_canvas[sizeof(g_display0_fb_background[0])] BSP_ALIGN_VARIABLE_V2(4) BSP_PLACE_IN_SECTION_V2(".sdram");
            #else /* Inherit Frame Buffer Name from Graphics Screen 2 */
            uint8_t g_sf_el_gx_canvas[sizeof(g_display0_fb_foreground[0])] BSP_ALIGN_VARIABLE_V2(4) BSP_PLACE_IN_SECTION_V2(".sdram");
            #endif
            #endif

/** JPEG Work Buffer */
#if GX_USE_SYNERGY_JPEG
            #if (768000)
            uint8_t g_sf_el_gx_jpegbuffer[768000] BSP_ALIGN_VARIABLE_V2(64) BSP_PLACE_IN_SECTION_V2(".sdram");
            #endif
            #endif

/** GUIX Port module control block instance */
static sf_el_gx_instance_ctrl_t g_sf_el_gx_ctrl;

/** GUIX Port module configuration */
static const sf_el_gx_cfg_t g_sf_el_gx_cfg = {
/* Display Instance Configuration */
.p_display_instance = (display_instance_t*) &g_display0,

/* Display Driver Runtime Configuration */
#if (1 == 1) /* Inherit Frame Buffer Name from Graphics Screen 1 */
		.p_display_runtime_cfg = &g_display0_runtime_cfg_bg,
#else /* Inherit Frame Buffer Name from Graphics Screen 2 */
                .p_display_runtime_cfg = &g_display0_runtime_cfg_fg,
            #endif

		/* GUIX Canvas Configuration */
#if (false)
                .p_canvas        = g_sf_el_gx_canvas,
            #else
		.p_canvas = NULL,
#endif

		/* Display Driver Frame Buffer A Configuration */
#if (1 == 1) /* Inherit Frame Buffer Name from Graphics Screen 1 */
		.p_framebuffer_a = &g_display0_fb_background[0], /* Always array[0] is used */
		.inherit_frame_layer = DISPLAY_FRAME_LAYER_1,
#else /* Inherit Frame Buffer Name from Graphics Screen 2 */
                .p_framebuffer_a = &g_display0_fb_foreground[0], /* Always array[0] is used */
	         .inherit_frame_layer = DISPLAY_FRAME_LAYER_2,
            #endif

		/* Display Driver Frame Buffer B Configuration */
#if (1 == 1) /* Inherit Frame Buffer Name from Graphics Screen 1 */
#if (2 > 1) /* Multiple frame buffers are used for Graphics Screen 1 */
		.p_framebuffer_b = &g_display0_fb_background[1], /* Always array[1] is used */
#else /* Single Frame Buffer is used for Graphics Screen 1 */
                .p_framebuffer_b = NULL,
                #endif
#else /* Inherit Frame Buffer Name from Graphics Screen 2 */
                #if (2 > 1) /* Multiple frame buffers are used for Graphics Screen 2 */
                .p_framebuffer_b = &g_display0_fb_foreground[1], /* Always array[1] is used */
                #else /* Single Frame Buffer is used for Graphics Screen 2 */
                .p_framebuffer_b = NULL,
                #endif
            #endif

		/* User Callback Configuration */
		.p_callback = NULL,

		/* JPEG Work Buffer Configuration */
#if GX_USE_SYNERGY_JPEG
                .p_jpegbuffer    = g_sf_el_gx_jpegbuffer,
                .jpegbuffer_size = 768000,
                .p_sf_jpeg_decode_instance = (void *)&g_sf_jpeg_decode0,
            #else
		.p_jpegbuffer = NULL, .jpegbuffer_size = 0, .p_sf_jpeg_decode_instance =
				NULL,
#endif

		/* D/AVE 2D Buffer Cache */
		.dave2d_buffer_cache_enabled = true };

/** GUIX Port module instance */
sf_el_gx_instance_t g_sf_el_gx = { .p_api = &sf_el_gx_on_guix, .p_ctrl =
		&g_sf_el_gx_ctrl, .p_cfg = &g_sf_el_gx_cfg };
/***********************************************************************************************************************
 * USB CDC-ACM Interface Descriptor for FS mode g_usb_interface_desc_cdcacm_0
 **********************************************************************************************************************/
#if defined(__GNUC__)
            static volatile const unsigned char g_usb_interface_desc_cdcacm_0_full_speed[] BSP_PLACE_IN_SECTION_V2(".usb_interface_desc_fs") BSP_ALIGN_VARIABLE_V2(1)
            #else /* __ICCARM__ */
#pragma section = ".usb_interface_desc_fs" 1
__root static const unsigned char g_usb_interface_desc_cdcacm_0_full_speed[] BSP_PLACE_IN_SECTION_V2(".usb_interface_desc_fs")
#endif
= {
	/****************************************************************
	 * Interface Association Descriptor(IAD)                        *
	 ****************************************************************/
	0x08, /* 0 bLength */
	UX_INTERFACE_ASSOCIATION_DESCRIPTOR_ITEM, /* 1 bDescriptorType */
	0x00, /* 2 bFirstInterface */
	0x02, /* 3 bInterfaceCount */
	0x02, /* 4 bFunctionClass : Communication */
	0x02, /* 5 bFunctionSubClass : Abstract Control Model */
	0x00, /* 6 bFunctionProtocol : Standard or enhanced AT Command set protocol */
	0x00, /* 7 iFunction : String descriptor index */
	/****************************************************************
	 * Communication Class Interface Descriptor                     *
	 ****************************************************************/
	0x09, /* 0 bLength */
	UX_INTERFACE_DESCRIPTOR_ITEM, /* 1 bDescriptorType */
	0x00, /* 2 bInterfaceNumber */
	0x00, /* 3 bAlternateSetting  : Alternate for SetInterface Request */
	0x01, /* 4 bNumEndpoints      : 1 Endpoint for Interface#1 */
	0x02, /* 5 bInterfaceClass    : Communications Interface Class(0x2) */
	0x02, /* 6 bInterfaceSubClass : Abstract Control Model(0x2) */
	0x01, /* 7 bInterfaceProtocol : Common AT command(0x01) */
	0x00, /* 8 iInterface Index */
	/****************************************************************
	 * Header Functional Descriptor                                 *
	 ****************************************************************/
	0x05, /* 0 bFunctionLength */
	0x24, /* 1 bDescriptorType : CS_INTERFACE(24h) */
	0x00, /* 2 bDescriptorSubtype : Header Functional Descriptor(0x0) */
	0x10, /* 3 bcdCDC Number  0x0110 == 1.10 */
	0x01, /* 4 bcdCDC */
	/****************************************************************
	 * Abstract Control Management Functional Functional Descriptor *
	 ****************************************************************/
	0x04, /* 0 bFunctionLength */
	0x24, /* 1 bDescriptorType : CS_INTERFACE(24h) */
	0x02, /* 2 bDescriptorSubtype : Abstract Control Management Functional Descriptor(0x2) */
	0x06, /* 3 bmCapabilities (Supports SendBreak, GetLineCoding, SetControlLineState, GetLineCoding) */
	/****************************************************************
	 * Union Functional Descriptor                                  *
	 ****************************************************************/
	0x05, /* 0 bFunctionLength */
	0x24, /* 1 bDescriptorType : CS_INTERFACE(24h) */
	0x06, /* 2 bDescriptorSubtype : Union Functional Descriptor(0x6) */
	0x00, /* 3 bMasterInterface */
	0x01, /* 4 bSubordinateInterface0 */
	/****************************************************************
	 * Call Management Functional Descriptor                        *
	 ****************************************************************/
	0x05, /* 0 bFunctionLength */
	0x24, /* 1 bDescriptorType */
	0x01, /* 2 bDescriptorSubtype : Call Management Functional Descriptor(0x1) */
	0x03, /* 3 bmCapabilities */
	0x01, /* 4 bDataInterface */
	/****************************************************************
	 * CDC-ACM Endpoint descriptor (Interrupt) for Interface#0      *
	 ****************************************************************/
	0x07, /* 0 bLength */
	UX_ENDPOINT_DESCRIPTOR_ITEM, /* 1 bDescriptorType */
	(UX_ENDPOINT_IN | 3), /* 2 bEndpointAddress */
	UX_INTERRUPT_ENDPOINT, /* 3 bmAttributes  */
	0x08, /* 4 wMaxPacketSize */
	0x00, /* 5 wMaxPacketSize */
	0x0F, /* 6 bInterval */
	/****************************************************************
	 * CDC-ACM Data Class Interface Descriptor                      *
	 ****************************************************************/
	0x09, /* 0 bLength */
	UX_INTERFACE_DESCRIPTOR_ITEM, /* 1 bDescriptorType */
	0x01, /* 2 bInterfaceNumber */
	0x00, /* 3 bAlternateSetting  : Alternate for SetInterface Request */
	0x02, /* 4 bNumEndpoints      : 2 Endpoints for Interface#0 */
	0x0A, /* 5 bInterfaceClass    : Data Interface Class(0xA) */
	0x00, /* 6 bInterfaceSubClass : Abstract Control Model */
	0x00, /* 7 bInterfaceProtocol : No class specific protocol required */
	0x00, /* 8 iInterface Index   : String descriptor index */
	/****************************************************************
	 * CDC-ACM Endpoint Descriptor (Bulk-Out)  for Interface#1      *
	 ****************************************************************/
	0x07, /* 0 bLength */
	UX_ENDPOINT_DESCRIPTOR_ITEM, /* 1 bDescriptorType */
	(UX_ENDPOINT_OUT | 1), /* 2 bEndpointAddress */
	UX_BULK_ENDPOINT, /* 3 bmAttributes  */
	0x40, /* 4 wMaxPacketSize */
	0x00, /* 5 wMaxPacketSize */
	0x00, /* 6 bInterval */
	/****************************************************************
	 * CDC-ACM Endpoint Descriptor (Bulk-In) for Interface#1        *
	 ****************************************************************/
	0x07, /* 0 bLength */
	UX_ENDPOINT_DESCRIPTOR_ITEM, /* 1 bDescriptorType */
	(UX_ENDPOINT_IN | 2), /* 2 bEndpointAddress */
	UX_BULK_ENDPOINT, /* 3 bmAttributes  */
	0x40, /* 4 wMaxPacketSize */
	0x00, /* 5 wMaxPacketSize */
	0x00, /* 6 bInterval */
};

#if defined(UX_DCD_SYNERY_USE_USBHS)
/***********************************************************************************************************************
 * USB CDC-ACM Interface Descriptor for HS mode g_usb_interface_desc_cdcacm_0
 **********************************************************************************************************************/
#if defined(__GNUC__)
static volatile const unsigned char g_usb_interface_desc_cdcacm_0_high_speed[] BSP_PLACE_IN_SECTION_V2(".usb_interface_desc_hs") BSP_ALIGN_VARIABLE_V2(1)
#else /* __ICCARM__ */
#pragma section = ".usb_interface_desc_hs" 1
__root static const unsigned char g_usb_interface_desc_cdcacm_0_high_speed[] BSP_PLACE_IN_SECTION_V2(".usb_interface_desc_hs")
#endif
= {
	/****************************************************************
	 * Interface Association Descriptor(IAD)                        *
	 ****************************************************************/
	0x08, /* 0 bLength */
	UX_INTERFACE_ASSOCIATION_DESCRIPTOR_ITEM, /* 1 bDescriptorType */
	0x00, /* 2 bFirstInterface */
	0x02, /* 3 bInterfaceCount */
	0x02, /* 4 bFunctionClass : Communication */
	0x02, /* 5 bFunctionSubClass : Abstract Control Model */
	0x00, /* 6 bFunctionProtocol : Standard or enhanced AT Command set protocol */
	0x00, /* 7 iFunction : String descriptor index */
	/****************************************************************
	 * Communication Class Interface Descriptor                     *
	 ****************************************************************/
	0x09, /* 0 bLength */
	UX_INTERFACE_DESCRIPTOR_ITEM, /* 1 bDescriptorType */
	0x00, /* 2 bInterfaceNumber */
	0x00, /* 3 bAlternateSetting  : Alternate for SetInterface Request */
	0x01, /* 4 bNumEndpoints      : 1 Endpoint for Interface#1 */
	0x02, /* 5 bInterfaceClass    : Communications Interface Class(0x2) */
	0x02, /* 6 bInterfaceSubClass : Abstract Control Model(0x2) */
	0x01, /* 7 bInterfaceProtocol : Common AT command(0x01) */
	0x00, /* 8 iInterface Index */
	/****************************************************************
	 * Header Functional Descriptor                                 *
	 ****************************************************************/
	0x05, /* 0 bFunctionLength */
	0x24, /* 1 bDescriptorType : CS_INTERFACE(24h) */
	0x00, /* 2 bDescriptorSubtype : Header Functional Descriptor(0x0) */
	0x10, /* 3 bcdCDC Number  0x0110 == 1.10 */
	0x01, /* 4 bcdCDC */
	/****************************************************************
	 * Abstract Control Management Functional Functional Descriptor *
	 ****************************************************************/
	0x04, /* 0 bFunctionLength */
	0x24, /* 1 bDescriptorType : CS_INTERFACE(24h) */
	0x02, /* 2 bDescriptorSubtype : Abstract Control Management Functional Descriptor(0x2) */
	0x06, /* 3 bmCapabilities (Supports SendBreak, GetLineCoding, SetControlLineState, GetLineCoding) */
	/****************************************************************
	 * Union Functional Descriptor                                  *
	 ****************************************************************/
	0x05, /* 0 bFunctionLength */
	0x24, /* 1 bDescriptorType : CS_INTERFACE(24h) */
	0x06, /* 2 bDescriptorSubtype : Union Functional Descriptor(0x6) */
	0x00, /* 3 bMasterInterface */
	0x01, /* 4 bSubordinateInterface0 */
	/****************************************************************
	 * Call Management Functional Descriptor                        *
	 ****************************************************************/
	0x05, /* 0 bFunctionLength */
	0x24, /* 1 bDescriptorType */
	0x01, /* 2 bDescriptorSubtype : Call Management Functional Descriptor(0x1) */
	0x03, /* 3 bmCapabilities */
	0x01, /* 4 bDataInterface */
	/****************************************************************
	 * CDC-ACM Endpoint descriptor (Interrupt) for Interface#0      *
	 ****************************************************************/
	0x07, /* 0 bLength */
	UX_ENDPOINT_DESCRIPTOR_ITEM, /* 1 bDescriptorType */
	(UX_ENDPOINT_IN | 3), /* 2 bEndpointAddress */
	UX_INTERRUPT_ENDPOINT, /* 3 bmAttributes  */
	0x08, /* 4 wMaxPacketSize */
	0x00, /* 5 wMaxPacketSize */
	0x0F, /* 6 bInterval */
	/****************************************************************
	 * CDC-ACM Data Class Interface Descriptor                      *
	 ****************************************************************/
	0x09, /* 0 bLength */
	UX_INTERFACE_DESCRIPTOR_ITEM, /* 1 bDescriptorType */
	0x01, /* 2 bInterfaceNumber */
	0x00, /* 3 bAlternateSetting  : Alternate for SetInterface Request */
	0x02, /* 4 bNumEndpoints      : 2 Endpoints for Interface#0 */
	0x0A, /* 5 bInterfaceClass    : Data Interface Class(0xA) */
	0x00, /* 6 bInterfaceSubClass : Abstract Control Model */
	0x00, /* 7 bInterfaceProtocol : No class specific protocol required */
	0x00, /* 8 iInterface Index   : String descriptor index */
	/****************************************************************
	 * CDC-ACM Endpoint Descriptor (Bulk-Out)  for Interface#1      *
	 ****************************************************************/
	0x07, /* 0 bLength */
	UX_ENDPOINT_DESCRIPTOR_ITEM, /* 1 bDescriptorType */
	(UX_ENDPOINT_OUT | 1), /* 2 bEndpointAddress */
	UX_BULK_ENDPOINT, /* 3 bmAttributes  */
	0x00, /* 4 wMaxPacketSize */
	0x02, /* 5 wMaxPacketSize */
	0x00, /* 6 bInterval */
	/****************************************************************
	 * CDC-ACM Endpoint Descriptor (Bulk-In) for Interface#1        *
	 ****************************************************************/
	0x07, /* 0 bLength */
	UX_ENDPOINT_DESCRIPTOR_ITEM, /* 1 bDescriptorType */
	(UX_ENDPOINT_IN | 2), /* 2 bEndpointAddress */
	UX_BULK_ENDPOINT, /* 3 bmAttributes  */
	0x00, /* 4 wMaxPacketSize */
	0x02, /* 5 wMaxPacketSize */
	0x00, /* 6 bInterval */
};
#endif

/* Size of this USB Interface Descriptor */
#define USB_IFD_SIZE_0x00       (sizeof(g_usb_interface_desc_cdcacm_0_full_speed))
/* Number of Interface this USB Interface Descriptor has */
#define USB_IFD_NUM_0x00        (2)
#ifndef USB_IFD_NUM_0
#define USB_IFD_NUM_0  USB_IFD_NUM_0x00
#define USB_IFD_SIZE_0 USB_IFD_SIZE_0x00
#else
#ifndef USB_IFD_NUM_1
#define USB_IFD_NUM_1  USB_IFD_NUM_0x00
#define USB_IFD_SIZE_1 USB_IFD_SIZE_0x00
#else
#ifndef USB_IFD_NUM_2
#define USB_IFD_NUM_2  USB_IFD_NUM_0x00
#define USB_IFD_SIZE_2 USB_IFD_SIZE_0x00
#else
#ifndef USB_IFD_NUM_3
#define USB_IFD_NUM_3  USB_IFD_NUM_0x00
#define USB_IFD_SIZE_3 USB_IFD_SIZE_0x00
#else
#ifndef USB_IFD_NUM_4
#define USB_IFD_NUM_4  USB_IFD_NUM_0x00
#define USB_IFD_SIZE_4 USB_IFD_SIZE_0x00
#else
#ifndef USB_IFD_NUM_5
#define USB_IFD_NUM_5  USB_IFD_NUM_0x00
#define USB_IFD_SIZE_5 USB_IFD_SIZE_0x00
#else
#ifndef USB_IFD_NUM_6
#define USB_IFD_NUM_6  USB_IFD_NUM_0x00
#define USB_IFD_SIZE_6 USB_IFD_SIZE_0x00
#else
#ifndef USB_IFD_NUM_7
#define USB_IFD_NUM_7  USB_IFD_NUM_0x00
#define USB_IFD_SIZE_7 USB_IFD_SIZE_0x00
#else
#ifndef USB_IFD_NUM_8
#define USB_IFD_NUM_8  USB_IFD_NUM_0x00
#define USB_IFD_SIZE_8 USB_IFD_SIZE_0x00
#else
#ifndef USB_IFD_NUM_9
#define USB_IFD_NUM_9  USB_IFD_NUM_0x00
#define USB_IFD_SIZE_9 USB_IFD_SIZE_0x00
#endif 
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#if defined(__ICCARM__)
#define ux_v2_err_callback_WEAK_ATTRIBUTE
#pragma weak ux_v2_err_callback  = ux_v2_err_callback_internal
#elif defined(__GNUC__)
#define ux_v2_err_callback_WEAK_ATTRIBUTE   __attribute__ ((weak, alias("ux_v2_err_callback_internal")))
#endif
void ux_v2_err_callback(void *p_instance, void *p_data)
ux_v2_err_callback_WEAK_ATTRIBUTE;
/* memory pool allocation used by USBX system. */
CHAR g_ux_pool_memory[163840] = { 0 };
/*******************************************************************************************************************//**
 * @brief      This is a weak example initialization error function.  It should be overridden by defining a user  function 
 *             with the prototype below.
 *             - void ux_v2_err_callback(void * p_instance, void * p_data)
 *
 * @param[in]  p_instance arguments used to identify which instance caused the error and p_data Callback arguments used to identify what error caused the callback.
 **********************************************************************************************************************/
void ux_v2_err_callback_internal(void *p_instance, void *p_data);
void ux_v2_err_callback_internal(void *p_instance, void *p_data) {
	/** Suppress compiler warning for not using parameters. */
	SSP_PARAMETER_NOT_USED(p_instance);
	SSP_PARAMETER_NOT_USED(p_data);

	/** An error has occurred. Please check function arguments for more information. */
	BSP_CFG_HANDLE_UNRECOVERABLE_ERROR(0);
}

#ifdef UX_HOST_INITIALIZE

#ifdef USB_HOST_STORAGE_CLASS_REGISTER

/* A semaphore for synchronizing to the USB media storage mount timing. */
static TX_SEMAPHORE ux_host_storage_semaphore_insertion;

/* Temporary storage place for the pointer to a USBX Host Mass Storage Class Instance. */
static UX_HOST_CLASS_STORAGE *g_ux_new_host_storage_instance;

/* Function prototype for the function to notify changes happened to the USBX Host Class Mass Storage. */
static UINT ux_system_host_storage_change_function(ULONG event, VOID *instance);

/*******************************************************************************************************************//**
 * @brief      This is the function to register the USBX Host Class Mass Storage.
 **********************************************************************************************************************/
void ux_host_stack_class_register_storage(void) {
	UINT status;
	status = ux_host_stack_class_register(_ux_system_host_class_storage_name,
			ux_host_class_storage_entry);
	if (UX_SUCCESS != status) {
		ux_v2_err_callback(NULL, &status);
	}

	/* Create semaphores used for the USB Mass Storage Media Initialization. */

	if (ux_host_storage_semaphore_insertion.tx_semaphore_id
			!= ((ULONG) 0x53454D41)) {
		status = tx_semaphore_create(&ux_host_storage_semaphore_insertion,
				"ux_host_storage_semaphore_insertion", 0);
		if (TX_SUCCESS != status) {
			ux_v2_err_callback(NULL, &status);
		}
	}
}

/*******************************************************************************************************************//**
 * @brief      This is the function to notify changes happened to the USBX Host Class Mass Storage.
 * @param[in]  event      Event code from happened to the USBX Host system.
 * @param[in]  instance   Pointer to a USBX Host class instance, which occurs a event.
 * @retval     USBX error code
 **********************************************************************************************************************/
static UINT ux_system_host_storage_change_function(ULONG event, VOID *instance) {
	if (UX_DEVICE_INSERTION == event) /* Check if there is a device insertion. */
	{
		g_ux_new_host_storage_instance = instance;

		/* Put the semaphore for a USBX Mass Storage Media insertion. */
		tx_semaphore_put(&ux_host_storage_semaphore_insertion);
	} else if (UX_DEVICE_REMOVAL == event) /* Check if there is a device removal. */
	{
		g_ux_new_host_storage_instance = NULL;
	}
	return UX_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief      This is the function to get the FileX Media Control Block for a USB Mass Storage device.
 * @param[in]      new_instance     Pointer to a USBX Host Mass Storage Class instance.
 * @param[in/out]  p_storage_media  Pointer of the pointer to save an instance of the USBX Host Mass Storage Media.
 * @param[in/out]  p_fx_media       Pointer of the pointer to save an instance of FileX Media Control Block.
 * @retval     UX_HOST_CLASS_INSTANCE_UNKNOWN  The instance provided was not for a valid Mass Storage device.
 **********************************************************************************************************************/
UINT ux_system_host_storage_fx_media_get(UX_HOST_CLASS_STORAGE *instance,
		UX_HOST_CLASS_STORAGE_MEDIA **p_storage_media, FX_MEDIA **p_fx_media) {
	UINT error = UX_SUCCESS;
	UX_HOST_CLASS *p_host_class;
	UX_HOST_CLASS_STORAGE_MEDIA *p_storage_media_local;
	FX_MEDIA *p_fx_media_local;
	int index;

	/** Get the USBX Host Mass Storage Class. */
	ux_host_stack_class_get(_ux_system_host_class_storage_name, &p_host_class);

	/** Get the pointer to a media attached to the class container for USB Host Mass Storage. */
	p_storage_media_local =
			(UX_HOST_CLASS_STORAGE_MEDIA*) p_host_class->ux_host_class_media;

	/** Get the pointer to a FileX Media Control Block. */
	for (index = 0; index < UX_HOST_CLASS_STORAGE_MAX_MEDIA; index++) {
		p_fx_media_local = &p_storage_media_local->ux_host_class_storage_media;
		if (p_fx_media_local->fx_media_driver_info != instance) {
			/* None of FileX Media Control Blocks matched to this USBX Host Mass Storage Instance. */
			p_storage_media_local++;
		} else {
			/* Found a FileX Media Control Block used for this USBX Host Mass Storage Instance. */
			break;
		}
	}
	if (UX_HOST_CLASS_STORAGE_MAX_MEDIA == index) {
		error = UX_HOST_CLASS_INSTANCE_UNKNOWN;
	} else {
		*p_storage_media = p_storage_media_local;
		*p_fx_media = p_fx_media_local;
	}

	return error;
}
#endif /* USB_HOST_STORAGE_CLASS_REGISTER */

/*******************************************************************************************************************//**
 * @brief      This is the function to notify a USB event from the USBX Host system.
 * @param[in]  event     Event code from happened to the USBX Host system.
 * @param[in]  usb_class Pointer to a USBX Host class, which occurs a event.
 * @param[in]  instance  Pointer to a USBX Host class instance, which occurs a event.
 * @retval     USBX error code
 **********************************************************************************************************************/
UINT ux_system_host_change_function(ULONG event, UX_HOST_CLASS *host_class,
		VOID *instance) {
	UINT status = UX_SUCCESS;
	SSP_PARAMETER_NOT_USED(event);
	SSP_PARAMETER_NOT_USED(host_class);
	SSP_PARAMETER_NOT_USED(instance);

#if !defined(ux_host_event_callback)
	/* Call user function back for USBX Host Class event notification. */
	status = ux_host_event_callback(event, host_class, instance);
	if (UX_SUCCESS != status) {
		return status;
	}
#endif

#ifdef USB_HOST_STORAGE_CLASS_REGISTER
	/* Check the class container if it is for a USBX Host Mass Storage class. */
	if (UX_SUCCESS
			== _ux_utility_memory_compare(_ux_system_host_class_storage_name,
					host_class,
					_ux_utility_string_length_get(
							_ux_system_host_class_storage_name))) {
		status = ux_system_host_storage_change_function(event, instance);
	}
#endif
	return status;
}
#endif

#ifdef USB_HOST_HID_CLASS_REGISTER
/* Function prototype to register USBX Host HID Clients to the USBX Host system. */
static void ux_host_class_hid_clients_register(void);
#endif

/*******************************************************************************************************************//**
 * @brief     Initialization function that the user can choose to have called automatically during thread entry.
 *            The user can call this function at a later time if desired using the prototype below.
 *            - void ux_common_init0(void)
 **********************************************************************************************************************/
void ux_common_init0(void) {
	UINT status_ux_init;

	/** Initialize the USBX system. */
	status_ux_init = ux_system_initialize((CHAR*) g_ux_pool_memory, 163840,
			UX_NULL, 0);
	if (UX_SUCCESS != status_ux_init) {
		ux_v2_err_callback(NULL, &status_ux_init);
	}

#ifdef UX_HOST_INITIALIZE
	/** Initialize the USBX Host stack. */
	status_ux_init = ux_host_stack_initialize(ux_system_host_change_function);
	if (UX_SUCCESS != status_ux_init) {
		ux_v2_err_callback(NULL, &status_ux_init);
	}

	/** Register USB Host classes. */
#ifdef USB_HOST_HUB_CLASS_REGISTER
	status_ux_init = ux_host_stack_class_register(
			_ux_system_host_class_hub_name, ux_host_class_hub_entry);
	if (UX_SUCCESS != status_ux_init) {
		ux_v2_err_callback(NULL, &status_ux_init);
	}
#endif

#ifdef USB_HOST_CDC_ACM_CLASS_REGISTER
                status_ux_init =  ux_host_stack_class_register(_ux_system_host_class_cdc_acm_name, ux_host_class_cdc_acm_entry);
                if (UX_SUCCESS != status_ux_init)
                {
                    ux_v2_err_callback(NULL,&status_ux_init);
                }
#endif

#ifdef USB_HOST_HID_CLASS_REGISTER
	status_ux_init = ux_host_stack_class_register(
			_ux_system_host_class_hid_name, ux_host_class_hid_entry);
	if (UX_SUCCESS != status_ux_init) {
		ux_v2_err_callback(NULL, &status_ux_init);
	}

	ux_host_class_hid_clients_register();
#endif

#ifdef USB_HOST_STORAGE_CLASS_REGISTER
	ux_host_stack_class_register_storage();
#endif

#ifdef USB_HOST_VIDEO_CLASS_REGISTER
	status_ux_init = ux_host_stack_class_register(
			_ux_system_host_class_video_name, ux_host_class_video_entry);
	if (UX_SUCCESS != status_ux_init) {
		ux_v2_err_callback(NULL, &status_ux_init);
	}
#endif

#ifdef USB_HOST_AUDIO_CLASS_REGISTER
                status_ux_init =  ux_host_stack_class_register(_ux_system_host_class_audio_name, ux_host_class_audio_entry);
                if (UX_SUCCESS != status_ux_init)
                {
                    ux_v2_err_callback(NULL,&status_ux_init);
                }
#endif

#ifdef USB_HOST_PRINTER_CLASS_REGISTER
                status_ux_init =  ux_host_stack_class_register(_ux_system_host_class_printer_name, _ux_host_class_printer_entry);
                if (UX_SUCCESS != status_ux_init)
                {
                    ux_v2_err_callback(NULL,&status_ux_init);
                }
#endif

#endif /* UX_HOST_INITIALIZE */
}
#if defined(__ICCARM__)
              #define g_ux_host_0_err_callback_WEAK_ATTRIBUTE
              #pragma weak g_ux_host_0_err_callback  = g_ux_host_0_err_callback_internal
              #elif defined(__GNUC__)
              #define g_ux_host_0_err_callback_WEAK_ATTRIBUTE   __attribute__ ((weak, alias("g_ux_host_0_err_callback_internal")))
              #endif
void g_ux_host_0_err_callback(void *p_instance, void *p_data)
g_ux_host_0_err_callback_WEAK_ATTRIBUTE;
/*******************************************************************************************************************//**
 * @brief      This is a weak example initialization error function.  It should be overridden by defining a user  function 
 *             with the prototype below.
 *             - void g_ux_host_0_err_callback(void * p_instance, void * p_data)
 *
 * @param[in]  p_instance arguments used to identify which instance caused the error and p_data Callback arguments used to identify what error caused the callback.
 **********************************************************************************************************************/
void g_ux_host_0_err_callback_internal(void *p_instance, void *p_data);
void g_ux_host_0_err_callback_internal(void *p_instance, void *p_data) {
	/** Suppress compiler warning for not using parameters. */
	SSP_PARAMETER_NOT_USED(p_instance);
	SSP_PARAMETER_NOT_USED(p_data);

	/** An error has occurred. Please check function arguments for more information. */
	BSP_CFG_HANDLE_UNRECOVERABLE_ERROR(0);
}
/*******************************************************************************************************************//**
 * @brief     Initialization function that the user can choose to have called automatically during thread entry.
 *            The user can call this function at a later time if desired using the prototype below.
 *            - void ux_host_init0(void)
 **********************************************************************************************************************/
void ux_host_init0(void) {
	/** Register a USB host controller. */
	UINT status_g_ux_host_0 = ux_host_stack_hcd_register(
			(UCHAR*) "g_sf_el_ux_hcd_hs_0", g_sf_el_ux_hcd_hs_0_initialize,
			R_USBHS_BASE, UX_SYNERGY_CONTROLLER);
	if (UX_SUCCESS != status_g_ux_host_0) {
		g_ux_host_0_err_callback(NULL, &status_g_ux_host_0);
	}
}
/* Pointer to a USBX Host Video Class Instance */
UX_HOST_CLASS_VIDEO *g_ux_host_class_video0;
#ifndef UX_HOST_CLASS_HID_CLIENTS_REGISTER
#define UX_HOST_CLASS_HID_CLIENTS_REGISTER

#if defined(__ICCARM__)
            #define  g_ux_host_class_hid0_err_callback_WEAK_ATTRIBUTE
            #pragma weak  g_ux_host_class_hid0_err_callback  =  g_ux_host_class_hid0_err_callback_internal
            #elif defined(__GNUC__)
            #define  g_ux_host_class_hid0_err_callback_WEAK_ATTRIBUTE   __attribute__ ((weak, alias("g_ux_host_class_hid0_err_callback_internal")))
            #endif
void g_ux_host_class_hid0_err_callback(void *p_instance, void *p_data)
g_ux_host_class_hid0_err_callback_WEAK_ATTRIBUTE;

/*******************************************************************************************************************//**
 * @brief      This is a weak example initialization error function.  It should be overridden by defining a user  function 
 *             with the prototype below.
 *             - void  g_ux_host_class_hid0_err_callback(void * p_instance, void * p_data)
 *
 * @param[in]  p_instance arguments used to identify which instance caused the error and p_data Callback arguments used to identify what error caused the callback.
 **********************************************************************************************************************/
void g_ux_host_class_hid0_err_callback_internal(void *p_instance, void *p_data);
void g_ux_host_class_hid0_err_callback_internal(void *p_instance, void *p_data) {
	/** Suppress compiler warning for not using parameters. */
	SSP_PARAMETER_NOT_USED(p_instance);
	SSP_PARAMETER_NOT_USED(p_data);

	/** An error has occurred. Please check function arguments for more information. */
	BSP_CFG_HANDLE_UNRECOVERABLE_ERROR(0);
}

/***********************************************************************************************************************
 * Function to register HID clients.
 **********************************************************************************************************************/
static void ux_host_class_hid_clients_register(void) {
	UINT status_ux_host_hid_clients_register;
#if (1)
	/* Register the HID Keyboard client. */
	status_ux_host_hid_clients_register = ux_host_class_hid_client_register(
			_ux_system_host_class_hid_client_keyboard_name,
			ux_host_class_hid_keyboard_entry);
	if (UX_SUCCESS != status_ux_host_hid_clients_register) {
		g_ux_host_class_hid0_err_callback(NULL,
				&status_ux_host_hid_clients_register);
	}
#endif
#if (1)
	/* Register the HID Mouse client. */
	status_ux_host_hid_clients_register = ux_host_class_hid_client_register(
			_ux_system_host_class_hid_client_mouse_name,
			ux_host_class_hid_mouse_entry);
	if (UX_SUCCESS != status_ux_host_hid_clients_register) {
		g_ux_host_class_hid0_err_callback(NULL,
				&status_ux_host_hid_clients_register);
	}
#endif
#if (1)
	/* Register the HID Remote Control client. */
	status_ux_host_hid_clients_register = ux_host_class_hid_client_register(
			_ux_system_host_class_hid_client_remote_control_name,
			ux_host_class_hid_remote_control_entry);
	if (UX_SUCCESS != status_ux_host_hid_clients_register) {
		g_ux_host_class_hid0_err_callback(NULL,
				&status_ux_host_hid_clients_register);
	}
#endif
} /* End of function ux_host_class_hid_clients_register() */
#endif /* UX_HOST_CLASS_HID_CLIENTS_REGISTER */
/* Pointer to a USBX Host Mass Storage Class Instance */
UX_HOST_CLASS_STORAGE *g_ux_host_class_storage0;
/* A pointer to FileX Media Control Block for a USB flash device. */
FX_MEDIA *g_fx_media0_ptr;

/* Pointer to a USBX Host Mass Storage Class Media. */
UX_HOST_CLASS_STORAGE_MEDIA *g_ux_host_class_storage0_media;

#if defined(__ICCARM__)
            #define g_fx_media0_err_callback_failed_to_get_fx_media_WEAK_ATTRIBUTE
            #pragma weak g_fx_media0_err_callback_failed_to_get_fx_media  = g_fx_media0_err_callback_failed_to_get_fx_media_internal
            #elif defined(__GNUC__)
            #define g_fx_media0_err_callback_failed_to_get_fx_media_WEAK_ATTRIBUTE __attribute__ ((weak, alias("g_fx_media0_err_callback_failed_to_get_fx_media_internal")))
            #endif
void g_fx_media0_err_callback_failed_to_get_fx_media(void *p_instance,
		void *p_data)
g_fx_media0_err_callback_failed_to_get_fx_media_WEAK_ATTRIBUTE;

/* Function prototype for the weak initialization error function. */
void g_fx_media0_err_callback_failed_to_get_fx_media_internal(void *p_instance,
		void *p_data);

/*******************************************************************************************************************//**
 * @brief      This is a weak example initialization error function.  It should be overridden by defining a user function
 *             with the prototype below.
 *             - void g_fx_media0_err_callback_failed_to_get_fx_media(void * p_instance, void * p_data)
 *
 * @param[in]  p_instance  Not used.
 * @param[in]  p_data      Not used.
 **********************************************************************************************************************/
void g_fx_media0_err_callback_failed_to_get_fx_media_internal(void *p_instance,
		void *p_data) {
	/** Suppress compiler warning for not using parameters. */
	SSP_PARAMETER_NOT_USED(p_instance);
	SSP_PARAMETER_NOT_USED(p_data);

	/** A FileX media control block for a USB mass storage device was not found. */
	BSP_CFG_HANDLE_UNRECOVERABLE_ERROR(0);
}

/*******************************************************************************************************************//**
 * @brief      This is the function to get g_fx_media0 for a USB Mass Storage device.
 **********************************************************************************************************************/
void fx_media_init_function0(void) {
	/** Suspend the thread until a USB Mass Storage Device inserted. */
	UINT err_g_fx_media0_semaphore_get = tx_semaphore_get(
			&ux_host_storage_semaphore_insertion, 0xffffffff);
	if (TX_SUCCESS != err_g_fx_media0_semaphore_get) {
		g_fx_media0_err_callback_failed_to_get_fx_media(
				&ux_host_storage_semaphore_insertion,
				&err_g_fx_media0_semaphore_get);
	}

	/** Initialize the FileX Media Control Block g_fx_media0 for a USB Mass Storage device. */
	/** Get the pointer to a USBX Host Mass Storage Instance. */
	g_ux_host_class_storage0 = g_ux_new_host_storage_instance;

	/** Initialize the FileX Media Control Block g_fx_media0 for a USB Mass Storage device. */
	UINT err_g_fx_media0_get = ux_system_host_storage_fx_media_get(
			g_ux_new_host_storage_instance, &g_ux_host_class_storage0_media,
			&g_fx_media0_ptr);
	if (UX_SUCCESS != err_g_fx_media0_get) {
		g_fx_media0_err_callback_failed_to_get_fx_media(&g_fx_media0_ptr,
				&err_g_fx_media0_get);
	}
}
#if defined(__ICCARM__)
            #define g_sf_el_ux_dcd_fs_0_err_callback_WEAK_ATTRIBUTE
            #pragma weak g_sf_el_ux_dcd_fs_0_err_callback  = g_sf_el_ux_dcd_fs_0_err_callback_internal
            #elif defined(__GNUC__)
            #define g_sf_el_ux_dcd_fs_0_err_callback_WEAK_ATTRIBUTE   __attribute__ ((weak, alias("g_sf_el_ux_dcd_fs_0_err_callback_internal")))
            #endif
void g_sf_el_ux_dcd_fs_0_err_callback(void *p_instance, void *p_data)
g_sf_el_ux_dcd_fs_0_err_callback_WEAK_ATTRIBUTE;
#if (SF_EL_UX_CFG_FS_IRQ_IPL != BSP_IRQ_DISABLED)
            /* USBFS ISR vector registering. */
            #if !defined(SSP_SUPPRESS_ISR_g_sf_el_ux_dcd_fs_0) && !defined(SSP_SUPPRESS_ISR_USB)
            SSP_VECTOR_DEFINE_UNIT(usbfs_int_isr, USB, FS, INT, 0);
            #endif
            #endif

/* Prototype function for USBX DCD Initializer. */
void ux_dcd_initialize(void);

#undef SYNERGY_NOT_DEFINED
#define SYNERGY_NOT_DEFINED (1)
/*******************************************************************************************************************//**
 * @brief      This is a weak example initialization error function.  It should be overridden by defining a user  function 
 *             with the prototype below.
 *             - void g_sf_el_ux_dcd_fs_0_err_callback(void * p_instance, void * p_data)
 *
 * @param[in]  p_instance arguments used to identify which instance caused the error and p_data Callback arguments used to identify what error caused the callback.
 **********************************************************************************************************************/
void g_sf_el_ux_dcd_fs_0_err_callback_internal(void *p_instance, void *p_data);
void g_sf_el_ux_dcd_fs_0_err_callback_internal(void *p_instance, void *p_data) {
	/** Suppress compiler warning for not using parameters. */
	SSP_PARAMETER_NOT_USED(p_instance);
	SSP_PARAMETER_NOT_USED(p_data);

	/** An error has occurred. Please check function arguments for more information. */
	BSP_CFG_HANDLE_UNRECOVERABLE_ERROR(0);
}
#if ((SYNERGY_NOT_DEFINED != SYNERGY_NOT_DEFINED) && (SYNERGY_NOT_DEFINED != SYNERGY_NOT_DEFINED))
            /***********************************************************************************************************************
             * The definition of wrapper interface for USBX Synergy Port DCD Driver to get a transfer module instance.
             **********************************************************************************************************************/
            static UINT g_sf_el_ux_dcd_fs_0_initialize_transfer_support(ULONG dcd_io)
            {
                UX_DCD_SYNERGY_TRANSFER dcd_transfer;
                dcd_transfer.ux_synergy_transfer_tx = (transfer_instance_t *)&SYNERGY_NOT_DEFINED;
                dcd_transfer.ux_synergy_transfer_rx = (transfer_instance_t *)&SYNERGY_NOT_DEFINED;
                return (UINT)ux_dcd_synergy_initialize_transfer_support(dcd_io, (UX_DCD_SYNERGY_TRANSFER *)&dcd_transfer);
            }  /* End of function g_sf_el_ux_dcd_fs_0_initialize_transfer_support() */
            #endif

/***********************************************************************************************************************
 * Initializes USBX Device Controller Driver.
 **********************************************************************************************************************/
void ux_dcd_initialize(void) {
	UINT status;
	/* Initializes the USB device controller, enabling DMA transfer if transfer module instances are given. */
#if ((SYNERGY_NOT_DEFINED == SYNERGY_NOT_DEFINED) || (SYNERGY_NOT_DEFINED == SYNERGY_NOT_DEFINED))
	status = (UINT) ux_dcd_synergy_initialize(R_USBFS_BASE);
#else
                status = g_sf_el_ux_dcd_fs_0_initialize_transfer_support(R_USBFS_BASE);
                #endif
#undef SYNERGY_NOT_DEFINED
	if (UX_SUCCESS != status) {
		g_sf_el_ux_dcd_fs_0_err_callback(NULL, &status);
	}
} /* End of function ux_dcd_initialize() */
#if defined(__ICCARM__)
            #define ux_device_err_callback_WEAK_ATTRIBUTE
            #pragma weak ux_device_err_callback  = ux_device_err_callback_internal
            #elif defined(__GNUC__)
            #define ux_device_err_callback_WEAK_ATTRIBUTE   __attribute__ ((weak, alias("ux_device_err_callback_internal")))
            #endif
void ux_device_err_callback(void *p_instance, void *p_data)
ux_device_err_callback_WEAK_ATTRIBUTE;
/* Definition for the special linker section for USB */
#if defined(__GNUC__)
            extern uint32_t __usb_dev_descriptor_start_fs;
            extern uint32_t __usb_descriptor_end_fs;
            #if defined(UX_DCD_SYNERY_USE_USBHS)
            extern uint32_t __usb_dev_descriptor_start_hs;
            extern uint32_t __usb_descriptor_end_hs;
            #endif
            #endif

/* Counter to calculate number of Interfaces */
static uint8_t g_usbx_number_of_interface_count = 0;

/***********************************************************************************************************************
 * USB Device Descriptor for FS mode
 **********************************************************************************************************************/
#if defined(__GNUC__)
            static volatile const unsigned char device_framework_full_speed[] BSP_PLACE_IN_SECTION_V2(".usb_device_desc_fs") BSP_ALIGN_VARIABLE_V2(1)
            #else /* __ICCARM__ */
#pragma section = ".usb_device_desc_fs" 1
__root static const unsigned char device_framework_full_speed[] BSP_PLACE_IN_SECTION_V2(".usb_device_desc_fs")
#endif
= {
	0x12, /* 0 bLength */
	UX_DEVICE_DESCRIPTOR_ITEM, /* 1 bDescriptorType */
	0x00, /* 2 bcdUSB BCD(2.0) */
	0x02, /* 3 bcdUSB */
	0xEF, /* 4 bDeviceClass    : Device Class */
#if (0xEF == 0x00) || (0xEF == 0x02) || (0xEF == 0x03) || (0xEF == 0x08)
	0x00, /* 5 bDeviceSubClass : none */
	0x00, /* 6 bDeviceProtocol : none */
#elif (0xEF == 0xEF)
	0x02, /* 5 bDeviceSubClass : Common Class(0x02) */
	0x01, /* 6 bDeviceProtocol :Interface Association Descriptor(IAD) */
#elif (0xEF == 0xFF)
	0x00, /* 5 bDeviceSubClass : none */
	0x00, /* 6 bDeviceProtocol : none */
#endif
	0x40, /* 7 bMaxPacketSize0 */
	(uint8_t)(0x045B), /* 8 idVendor */
	(uint8_t)(0x045B >> 8), /* 9 idVendor */
	(uint8_t)(0x5300), /* 10 idProduct */
	(uint8_t)(0x5300 >> 8), /* 11 idProduct */
	(uint8_t)(0x0000), /* 12 bcdDevice */
	(uint8_t)(0x0000 >> 8), /* 13 bcdDevice */
	0x00, /* 14 iManufacturer */
	0x00, /* 15 iProduct */
	0x00, /* 16 iSerialNumber */
	0x01, /* 17 bNumConfigurations */
};

#if defined(UX_DCD_SYNERY_USE_USBHS)
/***********************************************************************************************************************
 * USB Device Descriptor for HS Mode
 **********************************************************************************************************************/
#if defined(__GNUC__)
static volatile const unsigned char device_framework_high_speed[] BSP_PLACE_IN_SECTION_V2(".usb_device_desc_hs") BSP_ALIGN_VARIABLE_V2(1)
#else /* __ICCARM__ */
#pragma section = ".usb_device_desc_hs" 1
__root static const unsigned char device_framework_high_speed[] BSP_PLACE_IN_SECTION_V2(".usb_device_desc_hs")
#endif
= {
	0x12, /* 0 bLength */
	UX_DEVICE_DESCRIPTOR_ITEM, /* 1 bDescriptorType */
	0x00, /* 2 bcdUSB BCD(2.0) */
	0x02, /* 3 bcdUSB */
	0xEF, /* 4 bDeviceClass    : Device Class */
#if (0xEF == 0x00) || (0xEF == 0x02) || (0xEF == 0x03) || (0xEF == 0x08)
	0x00, /* 5 bDeviceSubClass : none */
	0x00, /* 6 bDeviceProtocol : none */
#elif (0xEF == 0xEF) /* Composite device */
	0x02, /* 5 bDeviceSubClass : Common Class(0x02) */
	0x01, /* 6 bDeviceProtocol :Interface Association Descriptor(IAD) */
#elif (0xEF == 0xFF) /* Vendor specific device */
	0x00, /* 5 bDeviceSubClass : none */
	0x00, /* 6 bDeviceProtocol : none */
#endif
	0x40, /* 7 bMaxPacketSize0 */
	(uint8_t)(0x045B), /* 8 idVendor */
	(uint8_t)(0x045B >> 8), /* 9 idVendor */
	(uint8_t)(0x5300), /* 10 idProduct */
	(uint8_t)(0x5300 >> 8), /* 11 idProduct */
	(uint8_t)(0x0000), /* 12 bcdDevice */
	(uint8_t)(0x0000 >> 8), /* 13 bcdDevice */
	0x00, /* 14 iManufacturer */
	0x00, /* 15 iProduct */
	0x00, /* 16 iSerialNumber */
	0x01, /* 17 bNumConfigurations */
	/****************************************************************
	 * Device qualifier descriptor                                  *
	 ****************************************************************/
	0x0a, /* 0 bLength */
	UX_DEVICE_QUALIFIER_DESCRIPTOR_ITEM, /* 1 bDescriptorType */
	0x00, /* 2 bcdUSB BCD(2.0) */
	0x02, /* 3 bcdUSB */
	0xEF, /* 4 bDeviceClass    : Device Class */
#if (0xEF == 0x00) || (0xEF == 0x02) || (0xEF == 0x03) || (0xEF == 0x08)
	0x00, /* 5 bDeviceSubClass : none */
	0x00, /* 6 bDeviceProtocol : none */
#elif (0xEF == 0xEF)
	0x02, /* 5 bDeviceSubClass : Common Class(0x02) */
	0x00, /* 6 bDeviceProtocol : none */
#elif (0xEF == 0xFF)
	0x00, /* 5 bDeviceSubClass : none */
	0x00, /* 6 bDeviceProtocol : none */
#endif
	0x40, /* 7 bMaxPacketSize0 */
	0x01, /* 8 bNumConfigs : 1 */
	0x00, /* 9 Reserve, fixed to 0 */
};
#endif

/***********************************************************************************************************************
 * Configuration Value Auto-calculation for USB Configuration Descriptor
 **********************************************************************************************************************/
#ifndef USB_IFD_NUM_0
#define USB_IFD_NUM_0  (0)
#define USB_IFD_SIZE_0 (0)
#endif
#ifndef USB_IFD_NUM_1
#define USB_IFD_NUM_1  (0)
#define USB_IFD_SIZE_1 (0)
#endif
#ifndef USB_IFD_NUM_2
#define USB_IFD_NUM_2  (0)
#define USB_IFD_SIZE_2 (0)
#endif
#ifndef USB_IFD_NUM_3
#define USB_IFD_NUM_3  (0)
#define USB_IFD_SIZE_3 (0)
#endif
#ifndef USB_IFD_NUM_4
#define USB_IFD_NUM_4  (0)
#define USB_IFD_SIZE_4 (0)
#endif
#ifndef USB_IFD_NUM_5
#define USB_IFD_NUM_5  (0)
#define USB_IFD_SIZE_5 (0)
#endif
#ifndef USB_IFD_NUM_6
#define USB_IFD_NUM_6  (0)
#define USB_IFD_SIZE_6 (0)
#endif
#ifndef USB_IFD_NUM_7
#define USB_IFD_NUM_7  (0)
#define USB_IFD_SIZE_7 (0)
#endif
#ifndef USB_IFD_NUM_8
#define USB_IFD_NUM_8  (0)
#define USB_IFD_SIZE_8 (0)
#endif
#ifndef USB_IFD_NUM_9
#define USB_IFD_NUM_9  (0)
#define USB_IFD_SIZE_9 (0)
#endif

#define USB_NUMBER_OF_INTERFACE USB_IFD_NUM_0 + USB_IFD_NUM_1 + USB_IFD_NUM_2 + USB_IFD_NUM_3 + USB_IFD_NUM_4 + USB_IFD_NUM_5 + USB_IFD_NUM_6 + USB_IFD_NUM_7 + USB_IFD_NUM_8 + USB_IFD_NUM_9
#define USB_TOTAL_LENGTH        USB_IFD_SIZE_0 + USB_IFD_SIZE_1 + USB_IFD_SIZE_2 + USB_IFD_SIZE_3 + USB_IFD_SIZE_4 + USB_IFD_SIZE_5 + USB_IFD_SIZE_6 + USB_IFD_SIZE_7 + USB_IFD_SIZE_8 + USB_IFD_SIZE_9

#if (0 != 0x00)
#define USB_CFG_DESCRIPTOR_bNumInterfaces     (0x00)
#else
#define USB_CFG_DESCRIPTOR_bNumInterfaces     (USB_NUMBER_OF_INTERFACE)
#endif
#if (0 != 0x00)
#define USB_CFG_DESCRIPTOR_wTotalLength       (0x00)
#else
#define USB_CFG_DESCRIPTOR_wTotalLength       (0x09 + USB_TOTAL_LENGTH)
#endif

/***********************************************************************************************************************
 * Consolidated USB Device Descriptor Framework on RAM for IAR build
 **********************************************************************************************************************/
#ifdef __ICCARM__
/* Memory area to complete USB device Descriptor Framework. */
static uint8_t device_framework_full_speed_ram[0x12 + USB_CFG_DESCRIPTOR_wTotalLength];
#if defined(UX_DCD_SYNERY_USE_USBHS)
static uint8_t device_framework_high_speed_ram[0x12 + 0x0a + USB_CFG_DESCRIPTOR_wTotalLength];
#endif
#endif

/***********************************************************************************************************************
 * USB Configuration Descriptor for FS mode
 **********************************************************************************************************************/
#if defined(__GNUC__)
static volatile const unsigned char ux_cfg_descriptor_full_speed[] BSP_PLACE_IN_SECTION_V2(".usb_config_desc_fs") BSP_ALIGN_VARIABLE_V2(1)
#else /* __ICCARM__ */
#pragma section = ".usb_config_desc_fs" 1
__root static const unsigned char ux_cfg_descriptor_full_speed[] BSP_PLACE_IN_SECTION_V2(".usb_config_desc_fs")
#endif
= {
	0x09, /* 0 bLength */
	UX_CONFIGURATION_DESCRIPTOR_ITEM, /* 1 bDescriptorType */
	(uint8_t)(USB_CFG_DESCRIPTOR_wTotalLength), /* 2 wTotalLength : This will be calculated at run-time. */
	(uint8_t)(USB_CFG_DESCRIPTOR_wTotalLength >> 8), /* 3 wTotalLength : This will be calculated at run-time. */
	(uint8_t)(USB_CFG_DESCRIPTOR_bNumInterfaces), /* 4 bNumInterfaces */
	0x01, /* 5 bConfigurationValue : Fixed to 1 since only one configuration is supported. */
	0x00, /* 6 iConfiguration */
	0x80 | (1 << 6) | (0 << 5), /* 7 bmAttributes */
	50, /* 8 bMaxPower */
};

#if defined(UX_DCD_SYNERY_USE_USBHS)
/***********************************************************************************************************************
 * USB Configuration Descriptor for HS mode
 **********************************************************************************************************************/
#if defined(__GNUC__)
static volatile const unsigned char ux_cfg_descriptor_high_speed[] BSP_PLACE_IN_SECTION_V2(".usb_config_desc_hs") BSP_ALIGN_VARIABLE_V2(1)
#else /* __ICCARM__ */
#pragma section = ".usb_config_desc_hs" 1
__root static const unsigned char ux_cfg_descriptor_high_speed[] BSP_PLACE_IN_SECTION_V2(".usb_config_desc_hs")
#endif
= {
	0x09, /* 0 bLength */
	UX_CONFIGURATION_DESCRIPTOR_ITEM, /* 1 bDescriptorType */
	(uint8_t)(USB_CFG_DESCRIPTOR_wTotalLength), /* 2 wTotalLength : This will be calculated at run-time. */
	(uint8_t)(USB_CFG_DESCRIPTOR_wTotalLength >> 8), /* 3 wTotalLength : This will be calculated at run-time. */
	(uint8_t)(USB_CFG_DESCRIPTOR_bNumInterfaces), /* 4 bNumInterfaces */
	0x01, /* 5 bConfigurationValue : Fixed to 1 since only one configuration is supported. */
	0x00, /* 6 iConfiguration */
	0x80 | (1 << 6) | (0 << 5), /* 7 bmAttributes */
	50, /* 8 bMaxPower */
};
#endif

/***********************************************************************************************************************
 * USB Language Framework (Default setting for US English)
 **********************************************************************************************************************/
#if(0 == 0) /* Create USB Language Framework with default setting if not defined by user. */
static volatile const UCHAR language_id_framework_default[] = { (uint8_t)(
		0x0409), /* Supported Language Code */
(uint8_t)(0x0409 >> 8) /* US English as the default */
};
#endif

/***********************************************************************************************************************
 * Function to get size of USB String Descriptor
 **********************************************************************************************************************/
#if (0)
            extern const UCHAR NULL[];
            static ULONG ux_device_string_descriptor_size_get(void)
            {
                ULONG  size = 0;
                UCHAR   * ptr = (UCHAR *)NULL;
                if(NULL != ptr)
                {
                    for(INT i = 0; i < 0; i++)
                    {
                        ptr = ptr + 3;      /* bLength at byte offset 3 */
                        /* Counts bLength + Language code(2bytes) + bLength(1byte) */
                        size = size + *ptr + 4;
                        ptr = ptr + (*ptr) + 1;
                    }
                }
                return size;
            }
            #endif

#if (0)
            extern const UCHAR NULL[];
            #endif
/*******************************************************************************************************************//**
 * @brief      This is a weak example initialization error function.  It should be overridden by defining a user  function 
 *             with the prototype below.
 *             - void ux_device_err_callback(void * p_instance, void * p_data)
 *
 * @param[in]  p_instance arguments used to identify which instance caused the error and p_data Callback arguments used to identify what error caused the callback.
 **********************************************************************************************************************/
void ux_device_err_callback_internal(void *p_instance, void *p_data);
void ux_device_err_callback_internal(void *p_instance, void *p_data) {
	/** Suppress compiler warning for not using parameters. */
	SSP_PARAMETER_NOT_USED(p_instance);
	SSP_PARAMETER_NOT_USED(p_data);

	/** An error has occurred. Please check function arguments for more information. */
	BSP_CFG_HANDLE_UNRECOVERABLE_ERROR(0);
}

#ifdef __ICCARM__

            /*******************************************************************************************************************//**
              * @brief      This function is called during USBX device initialization to remove any padding inserted by the compiler.
              **********************************************************************************************************************/
            void ux_device_remove_compiler_padding(unsigned char * p_device_framework, UINT length)
            {
                INT remaining, count = 0;
                unsigned char *index, *temp;
                remaining = length;
                index = p_device_framework;

                /* Parse the device framework and remove any padding inserted  by the compiler.  */
                while(index < (p_device_framework + length))

                {
                    temp = index;

                    if(*temp == 0)
                    {
                        /* Remove, if any padding inserted by the compiler. */ 
                        count = 0;
                        while( (*temp == 0 ) && (temp < (p_device_framework + length)) )
                        {
                            temp++;
                            count++;
                        }

                        remaining -= count;
                        if(remaining > 0)
                        {
                            memcpy(index, temp, remaining);
                            count = 0;
                        }

                        else
                        {
                            index = p_device_framework + length;
                        }
                    }

                    else
                    {
                        /* Jump to next offset of the descriptors. */
                        remaining = remaining - (*index);
                        index = index + (*index);

                    }
                }                   
            }

            #endif
/*******************************************************************************************************************//**
 * @brief     Initialization function that the user can choose to have called automatically during thread entry.
 *            The user can call this function at a later time if desired using the prototype below.
 *            - void ux_device_init0(void)
 **********************************************************************************************************************/
void ux_device_init0(void) {

	UINT status_ux_device_init;

	/** Calculate the size of USBX String Framework. */
	ULONG string_framework_size;
	UCHAR *p_string_framework;

#if(0) /* Check if the number of index in USBX String Framework is greater than zero. */
                {
                    p_string_framework    = (UCHAR *)NULL;
                    string_framework_size = ux_device_string_descriptor_size_get();
                }
                #else
	{
		p_string_framework = NULL;
		string_framework_size = 0;
	}
#endif

	/** Calculate the size of USB Language Framework. */
	ULONG language_framework_size;
	UCHAR *p_language_framework;

#if(0) /* Check if the number of index in USB Language Framework is greater than zero. */
                {
                    p_language_framework  = (UCHAR *)NULL;
                    language_framework_size = 0 * 2;
                }
                #else
	{
		p_language_framework = (UCHAR*) language_id_framework_default;
		language_framework_size = 2;
	}
#endif

	/** Initialize the USB Device stack. */
#ifdef __GNUC__
                status_ux_device_init = ux_device_stack_initialize(
                #if defined(UX_DCD_SYNERY_USE_USBHS)
                                                 (UCHAR *)device_framework_high_speed, ((uint32_t)&__usb_descriptor_end_hs - (uint32_t)&__usb_dev_descriptor_start_hs),
                #else /* If USB DCD runs in FS mode, USB Device Descriptor for HS mode is not required. */
                                                 NULL, 0,
                #endif
                                                 (UCHAR *)device_framework_full_speed, ((uint32_t)&__usb_descriptor_end_fs - (uint32_t)&__usb_dev_descriptor_start_fs),
                                                 p_string_framework, string_framework_size,
                                                 p_language_framework, language_framework_size, NULL);
                #else /* __ICCARM__ */

	uint8_t *p_device_framework;
	unsigned char *start, *end, interface_desc_fs_hs[0x12 + 0x0a
			+ USB_CFG_DESCRIPTOR_wTotalLength + 0x14];
	UINT length = 0;
#if defined(UX_DCD_SYNERY_USE_USBHS)
                start = (unsigned char *)(__section_begin (".usb_interface_desc_hs"));
                end = (unsigned char *)(__section_end (".usb_interface_desc_hs"));
                length = abs(end - start);
                memset(interface_desc_fs_hs, 0, sizeof(interface_desc_fs_hs));
                p_device_framework = (uint8_t *)interface_desc_fs_hs;
                /* Creates Device Descriptor Frameworks copying Descriptors from ROM. */
                memcpy (p_device_framework, device_framework_high_speed, sizeof(device_framework_high_speed));
                p_device_framework = p_device_framework + sizeof(device_framework_high_speed);
                /* Copy Configuration Descriptor */
                memcpy (p_device_framework, ux_cfg_descriptor_high_speed, sizeof(ux_cfg_descriptor_high_speed));
                p_device_framework = p_device_framework + sizeof(ux_cfg_descriptor_high_speed);
                /* Copy Interface Descriptors */
                memcpy (p_device_framework, (uint8_t *) __section_begin (".usb_interface_desc_hs"), length);
                ux_device_remove_compiler_padding(p_device_framework, length);
                /* Copy device frame work descriptor after removal of padding. */
                memcpy(device_framework_high_speed_ram, interface_desc_fs_hs, sizeof(device_framework_high_speed_ram));
                #endif

	start = (unsigned char*) (__section_begin(".usb_interface_desc_fs"));
	end = (unsigned char*) (__section_end(".usb_interface_desc_fs"));
	length = abs(end - start);
	memset(interface_desc_fs_hs, 0, sizeof(interface_desc_fs_hs));
	p_device_framework = (uint8_t*) interface_desc_fs_hs;
	/* Creates Device Descriptor Frameworks copying Descriptors from ROM. */
	memcpy(p_device_framework, device_framework_full_speed,
			sizeof(device_framework_full_speed));
	p_device_framework = p_device_framework
			+ sizeof(device_framework_full_speed);
	/* Copy Configuration Descriptor */
	memcpy(p_device_framework, ux_cfg_descriptor_full_speed,
			sizeof(ux_cfg_descriptor_full_speed));
	p_device_framework = p_device_framework
			+ sizeof(ux_cfg_descriptor_full_speed);
	/* Copy Interface Descriptors */
	memcpy(p_device_framework,
			(uint8_t*) __section_begin(".usb_interface_desc_fs"), length);
	ux_device_remove_compiler_padding(p_device_framework, length);
	/* Copy device framework descriptor after removal of padding. */
	memcpy(device_framework_full_speed_ram, interface_desc_fs_hs,
			sizeof(device_framework_full_speed_ram));

	status_ux_device_init =
			ux_device_stack_initialize(
#if defined(UX_DCD_SYNERY_USE_USBHS)
                                                 (UCHAR *)device_framework_high_speed_ram, (0x12 + 0x0a + USB_CFG_DESCRIPTOR_wTotalLength),
                #else /* If USB DCD runs in FS mode, USB Device Descriptor for HS mode is not required. */
					NULL, 0,
#endif
					(UCHAR*) device_framework_full_speed_ram,
					(0x12 + USB_CFG_DESCRIPTOR_wTotalLength),
					p_string_framework, string_framework_size,
					p_language_framework, language_framework_size, NULL);
#endif
	if (UX_SUCCESS != status_ux_device_init) {
		ux_device_err_callback(NULL, &status_ux_device_init);
	}

}
/***********************************************************************************************************************
 * Function prototypes for USBX CDC-ACM
 **********************************************************************************************************************/
static void g_ux_device_class_cdc_acm0_setup(void);

/***********************************************************************************************************************
 * USBX CDC-ACM Parameter Setup Function.
 **********************************************************************************************************************/
static UX_SLAVE_CLASS_CDC_ACM_PARAMETER g_ux_device_class_cdc_acm0_parameter;
void g_ux_device_class_cdc_acm0_setup(void) {
	g_ux_device_class_cdc_acm0_parameter.ux_slave_class_cdc_acm_instance_activate =
			ux_cdc_device0_instance_activate;
	g_ux_device_class_cdc_acm0_parameter.ux_slave_class_cdc_acm_instance_deactivate =
			ux_cdc_device0_instance_deactivate;
	g_ux_device_class_cdc_acm0_parameter.ux_slave_class_cdc_acm_parameter_change =
			NULL;
	/* Initializes the device cdc class. */
	ux_device_stack_class_register(_ux_system_slave_class_cdc_acm_name,
			ux_device_class_cdc_acm_entry, 1, 0x00,
			(VOID*) &g_ux_device_class_cdc_acm0_parameter);

	/* Counts up the number of Interfaces. */
	g_usbx_number_of_interface_count = (uint8_t)(
			g_usbx_number_of_interface_count + 2);
}
/*******************************************************************************************************************//**
 * @brief     Initialization function that the user can choose to have called automatically during thread entry.
 *            The user can call this function at a later time if desired using the prototype below.
 *            - void ux_device_class_cdc_acm_init0(void)
 **********************************************************************************************************************/
void ux_device_class_cdc_acm_init0(void) {
	/* Setups USB CDC-ACM Class to create the instance */
	g_ux_device_class_cdc_acm0_setup();

	/* USB DCD initialization (Only executed once after all the class registration completed). */
	if (g_usbx_number_of_interface_count == USB_NUMBER_OF_INTERFACE) {
		/* Initializes USBX Device Control Driver */
		ux_dcd_initialize();
	}
}
/*******************************************************************************************************************//**
 * @brief     Initialize the USB Device stack and register the class into slave stack
 **********************************************************************************************************************/
void g_ux_device_class_cdc_acm0_ux_device_open_init(void) {
	/* Initialize the USB Device stack */
	ux_device_init0();

	/* Register the class into slave stack */
	ux_device_class_cdc_acm_init0();
}
const ioport_instance_t g_ioport =
		{ .p_api = &g_ioport_on_ioport, .p_cfg = NULL };
const elc_instance_t g_elc = { .p_api = &g_elc_on_elc, .p_cfg = NULL };
const cgc_instance_t g_cgc = { .p_api = &g_cgc_on_cgc, .p_cfg = NULL };
/* Instance structure to use this module. */
const fmi_instance_t g_fmi = { .p_api = &g_fmi_on_fmi };
void g_common_init(void) {
	/** Call initialization function if user has selected to do so. */
#if (1)
	sf_message_init0();
#endif
	/** Call initialization function if user has selected to do so. */
#if FX_COMMON_INITIALIZE
	fx_common_init0();
#endif

	/** Call initialization function if user has selected to do so. */
#if (1)
	ux_common_init0();
#endif
	/** Call initialization function if user has selected to do so. */
#if (1)
	ux_host_init0();
#endif

#if (0)
            /** Call FileX Media Control Block initialization function. Can call at later time as well. */
            fx_media_init_function0();
#endif
	/** Call initialization function if user has selected to do so. */
#if (1)
	ux_device_init0();
#endif
	/** Call initialization function if user has selected to do so. */
#if (1)
	ux_device_class_cdc_acm_init0();
#endif
}
