/* generated thread source file - do not edit */
#include "cli_thread.h"

TX_THREAD cli_thread;
void cli_thread_create(void);
static void cli_thread_func(ULONG thread_input);
static uint8_t cli_thread_stack[1024] BSP_PLACE_IN_SECTION_V2(".stack.cli_thread") BSP_ALIGN_VARIABLE_V2(BSP_STACK_ALIGNMENT);
void tx_startup_err_callback(void *p_instance, void *p_data);
void tx_startup_common_init(void);
#if defined(__ICCARM__)
                  #define g_sf_debug_comms_err_callback_WEAK_ATTRIBUTE
                  #pragma weak g_sf_debug_comms_err_callback  = g_sf_debug_comms_err_callback_internal
                  #define ux_cdc_device0_instance_deactivate_WEAK_ATTRIBUTE
                  #pragma weak ux_cdc_device0_instance_deactivate  = ux_cdc_device0_instance_deactivate_internal
                  #elif defined(__GNUC__)
                  #define g_sf_debug_comms_err_callback_WEAK_ATTRIBUTE   __attribute__ ((weak, alias("g_sf_debug_comms_err_callback_internal")))
                  #define ux_cdc_device0_instance_deactivate_WEAK_ATTRIBUTE   __attribute__ ((weak, alias("ux_cdc_device0_instance_deactivate_internal")))
                  #endif
void g_sf_debug_comms_err_callback(void *p_instance, void *p_data)
g_sf_debug_comms_err_callback_WEAK_ATTRIBUTE;
void ux_cdc_device0_instance_deactivate(VOID *cdc_instance)
ux_cdc_device0_instance_deactivate_WEAK_ATTRIBUTE;
/***********************************************************************************************************************
 * USB Communications Framework (SF_EL_UX_COMMS) Instance for g_sf_debug_comms
 **********************************************************************************************************************/
sf_el_ux_comms_instance_ctrl_t g_sf_debug_comms_instance_ctrl =
		{ .p_cdc = NULL, };

/* Instance structure to use this module. */
const sf_comms_instance_t g_sf_debug_comms = { .p_ctrl =
		&g_sf_debug_comms_instance_ctrl, .p_cfg = NULL, .p_api =
		&g_sf_el_ux_comms_on_sf_comms };

/***********************************************************************************************************************
 * USBX Device CDC-ACM Instance Activate Callback function required for g_sf_debug_comms
 **********************************************************************************************************************/
VOID ux_cdc_device0_instance_activate(VOID *cdc_instance) {
	/* Save the CDC instance for g_sf_debug_comms. */
	g_sf_debug_comms_instance_ctrl.p_cdc =
			(UX_SLAVE_CLASS_CDC_ACM*) cdc_instance;

	/* Inform the CDC instance activation event for g_sf_debug_comms. */
	if (NULL != g_sf_debug_comms.p_ctrl) {
		sf_el_ux_comms_instance_ctrl_t *p_ux_comms_ctrl =
				(sf_el_ux_comms_instance_ctrl_t*) g_sf_debug_comms.p_ctrl;

		/* Put a semaphore if the instance for g_sf_debug_comms to inform CDC instance is ready. */
		tx_semaphore_ceiling_put(&p_ux_comms_ctrl->semaphore, 1);
	}
	return;
}
/***********************************************************************************************************************
 * @brief      This is a weak USBX Device CDC-ACM Instance Deactivate Callback function required for g_sf_debug_comms
 *             It should be overridden by defining a user function with the prototype mentioned below.
 *             - VOID ux_cdc_device0_instance_deactivate (VOID *cdc_instance)
 *
 * @param[in]  cdc_instance argument is used to identify the CDC-ACM Instance
 **********************************************************************************************************************/
VOID ux_cdc_device0_instance_deactivate_internal(VOID *cdc_instance) {
	SSP_PARAMETER_NOT_USED(cdc_instance);

	/* Reset the CDC instance for g_sf_debug_comms. */
	g_sf_debug_comms_instance_ctrl.p_cdc = UX_NULL;

	return;
}
/*******************************************************************************************************************//**
 * @brief      This is a weak example initialization error function.  It should be overridden by defining a user  function 
 *             with the prototype below.
 *             - voidg_sf_debug_comms_err_callback(void * p_instance, void * p_data)
 *
 * @param[in]  p_instance arguments used to identify which instance caused the error and p_data Callback arguments used to identify what error caused the callback.
 **********************************************************************************************************************/
void g_sf_debug_comms_err_callback_internal(void *p_instance, void *p_data);
void g_sf_debug_comms_err_callback_internal(void *p_instance, void *p_data) {
	/** Suppress compiler warning for not using parameters. */
	SSP_PARAMETER_NOT_USED(p_instance);
	SSP_PARAMETER_NOT_USED(p_data);

	/** An error has occurred. Please check function arguments for more information. */
	BSP_CFG_HANDLE_UNRECOVERABLE_ERROR(0);
}
/*******************************************************************************************************************//**
 * @brief     Initialization function that the user can choose to have called automatically during thread entry.
 *            The user can call this function at a later time if desired using the prototype below.
 *            - void sf_debug_comms_init(void)
 **********************************************************************************************************************/
void sf_debug_comms_init(void) {
	ssp_err_t ssp_err_g_sf_debug_comms;
	/** Open USB Communications Framework */
	ssp_err_g_sf_debug_comms = g_sf_debug_comms.p_api->open(
			g_sf_debug_comms.p_ctrl, g_sf_debug_comms.p_cfg);
	if (UX_SUCCESS != ssp_err_g_sf_debug_comms) {
		g_sf_debug_comms_err_callback((void*) &g_sf_debug_comms,
				&ssp_err_g_sf_debug_comms);
	}
}
/*******************************************************************************************************************//**
 * @brief      USBX uninitialize the USB controller
 *
 * @retval     SSP_SUCCESS                   USB controller Uninitialized successfully.
 * @retval     UX_CONTROLLER_UNINIT_FAILED   Failed to Un-initialize the USB controller.
 **********************************************************************************************************************/
UINT sf_el_ux_comms_device_close(void) {
	UINT status;

	/** Disconnect the USB by resetting the USB controller */
	ux_dcd_synergy_disconnect();

	/** Free the device resources */
	ux_device_stack_disconnect();

	/** unregisters a slave class to the slave stack */
	ux_device_stack_class_unregister(_ux_system_slave_class_cdc_acm_name,
			ux_device_class_cdc_acm_entry);

	/** uninitializes the stack of the device side of USBX */
	ux_device_stack_uninitialize();

	/** uninitialize the USB DCD driver */
	status = ux_dcd_synergy_uninitialize(
			_ux_system_slave->ux_system_slave_dcd.ux_slave_dcd_io);

	/** return the error, if USB DCD drver uninitialization is failed */
	if (UX_SUCCESS != status) {
		return status;
	}
	return (UINT) UX_SUCCESS;
}
/*******************************************************************************************************************//**
 * @brief      USBX initialize the USB controller in device mode
 **********************************************************************************************************************/
void sf_el_ux_comms_device_open(void) {
	/** Initialize the USB controller, if it is not used */
	if (_ux_system_slave->ux_system_slave_dcd.ux_slave_dcd_controller_hardware
			== UX_NULL) {
		/** Initialize the USB Device stack and register the class into slave stack */
		g_ux_device_class_cdc_acm0_ux_device_open_init();

		/** Initialize the USB DCD driver */
		ux_dcd_initialize();
	}
}
#if defined(__ICCARM__)
              #define g_sf_console_err_callback_WEAK_ATTRIBUTE
              #pragma weak g_sf_console_err_callback  = g_sf_console_err_callback_internal
              #elif defined(__GNUC__)
              #define g_sf_console_err_callback_WEAK_ATTRIBUTE   __attribute__ ((weak, alias("g_sf_console_err_callback_internal")))
              #endif
void g_sf_console_err_callback(void *p_instance, void *p_data)
g_sf_console_err_callback_WEAK_ATTRIBUTE;
sf_console_instance_ctrl_t g_sf_console_ctrl;
const sf_console_cfg_t g_sf_console_cfg = { .p_comms = &g_sf_debug_comms,
		.p_initial_menu = &g_sf_console_root_menu, .echo = true, .autostart =
				false, };
/* Instance structure to use this module. */
const sf_console_instance_t g_sf_console = { .p_ctrl = &g_sf_console_ctrl,
		.p_cfg = &g_sf_console_cfg, .p_api = &g_sf_console_on_sf_console };
/*******************************************************************************************************************//**
 * @brief      This is a weak example initialization error function.  It should be overridden by defining a user  function 
 *             with the prototype below.
 *             - void g_sf_console_err_callback(void * p_instance, void * p_data)
 *
 * @param[in]  p_instance arguments used to identify which instance caused the error and p_data Callback arguments used to identify what error caused the callback.
 **********************************************************************************************************************/
void g_sf_console_err_callback_internal(void *p_instance, void *p_data);
void g_sf_console_err_callback_internal(void *p_instance, void *p_data) {
	/** Suppress compiler warning for not using parameters. */
	SSP_PARAMETER_NOT_USED(p_instance);
	SSP_PARAMETER_NOT_USED(p_data);

	/** An error has occurred. Please check function arguments for more information. */
	BSP_CFG_HANDLE_UNRECOVERABLE_ERROR(0);
}

/*******************************************************************************************************************//**
 * @brief     Initialization function that the user can choose to have called automatically during thread entry.
 *            The user can call this function at a later time if desired using the prototype below.
 *            - void sf_console_init(void)
 **********************************************************************************************************************/
void sf_console_init(void) {
	ssp_err_t ssp_err_g_sf_console;
	ssp_err_g_sf_console = g_sf_console.p_api->open(g_sf_console.p_ctrl,
			g_sf_console.p_cfg);
	if (SSP_SUCCESS != ssp_err_g_sf_console) {
		g_sf_console_err_callback((void*) &g_sf_console, &ssp_err_g_sf_console);
	}
}
extern bool g_ssp_common_initialized;
extern uint32_t g_ssp_common_thread_count;
extern TX_SEMAPHORE g_ssp_common_initialized_semaphore;

void cli_thread_create(void) {
	/* Increment count so we will know the number of ISDE created threads. */
	g_ssp_common_thread_count++;

	/* Initialize each kernel object. */

	UINT err;
	err = tx_thread_create(&cli_thread, (CHAR*) "CLI Thread", cli_thread_func,
			(ULONG) NULL, &cli_thread_stack, 1024, 9, 9, 1, TX_AUTO_START);
	if (TX_SUCCESS != err) {
		tx_startup_err_callback(&cli_thread, 0);
	}
}

static void cli_thread_func(ULONG thread_input) {
	/* Not currently using thread_input. */
	SSP_PARAMETER_NOT_USED(thread_input);

	/* Initialize common components */
	tx_startup_common_init();

	/* Initialize each module instance. */
	/** Call initialization function if user has selected to do so. */
#if (1)
	sf_console_init();
#endif

	/* Enter user code for this thread. */
	cli_thread_entry();
}
