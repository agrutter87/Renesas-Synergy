/* generated thread header file - do not edit */
#ifndef CLI_THREAD_H_
#define CLI_THREAD_H_
#include "bsp_api.h"
#include "tx_api.h"
#include "hal_data.h"
#ifdef __cplusplus
                extern "C" void cli_thread_entry(void);
                #else
extern void cli_thread_entry(void);
#endif
#include "sf_el_ux_comms_v2.h"
#include "sf_comms_api.h"
#include "sf_console.h"
#include "sf_console_api.h"
#ifdef __cplusplus
extern "C" {
#endif
/* USB Communication Framework Instance */
extern const sf_comms_instance_t g_sf_debug_comms;
/* USBX CDC-ACM Instance Activate User Callback Function */
VOID ux_cdc_device0_instance_activate(VOID *cdc_instance);
/* USBX CDC-ACM Instance Deactivate User Callback Function */
VOID ux_cdc_device0_instance_deactivate_internal(VOID *cdc_instance);
void g_sf_debug_comms_err_callback(void *p_instance, void *p_data);
void sf_debug_comms_init(void);
extern void ux_dcd_initialize(void);
extern void g_ux_device_class_cdc_acm0_ux_device_open_init(void);
UINT sf_el_ux_comms_device_close(void);
void sf_el_ux_comms_device_open(void);
extern const sf_console_instance_t g_sf_console;
extern const sf_console_menu_t g_sf_console_root_menu;
void g_sf_console_err_callback(void *p_instance, void *p_data);
void sf_console_init(void);
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* CLI_THREAD_H_ */
