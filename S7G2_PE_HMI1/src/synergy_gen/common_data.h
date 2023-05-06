/* generated common header file - do not edit */
#ifndef COMMON_DATA_H_
#define COMMON_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "r_dac.h"
#include "r_dac_api.h"
#include "r_gpt.h"
#include "r_timer_api.h"
#include "r_dtc.h"
#include "r_transfer_api.h"
#include "sf_audio_playback_hw_dac.h"
#include "sf_message.h"
#include "sf_message_payloads.h"
#include "sf_audio_playback.h"
#include "fx_api.h"
#include "r_dmac.h"
#include "r_transfer_api.h"
#include "r_riic.h"
#include "r_i2c_api.h"
#include "r_i2c_api.h"
#include "sf_i2c.h"
#include "sf_i2c_api.h"

#include "r_jpeg_decode.h"
#include "sf_jpeg_decode.h"
#include "r_glcd.h"
#include "r_display_api.h"
#include "sf_el_gx.h"
#include "ux_api.h"
#include "ux_host_class_storage.h"
#include "r_ioport.h"
#include "r_ioport_api.h"
#include "r_elc.h"
#include "r_elc_api.h"
#include "r_cgc.h"
#include "r_cgc_api.h"
#include "r_fmi.h"
#include "r_fmi_api.h"
#ifdef __cplusplus
extern "C" {
#endif
/** DAC on DAC Instance. */
extern const dac_instance_t g_audio_dac;
/** Timer on GPT Instance. */
extern const timer_instance_t g_audio_timer;
#ifndef NULL
void NULL(timer_callback_args_t *p_args);
#endif
/* Transfer on DTC Instance. */
extern const transfer_instance_t g_audio_transfer;
#ifndef NULL
void NULL(transfer_callback_args_t *p_args);
#endif
extern const sf_audio_playback_hw_instance_t g_sf_audio_playback_hw;
extern void g_message_init(void);

/* SF Message on SF Message Instance. */
extern const sf_message_instance_t g_sf_message;
void g_sf_message_err_callback(void *p_instance, void *p_data);
void sf_message_init0(void);
void fx_common_init0(void);
/* Transfer on DMAC Instance. */
extern const transfer_instance_t g_transfer10;
#ifndef NULL
void NULL(transfer_callback_args_t *p_args);
#endif
/* Transfer on DMAC Instance. */
extern const transfer_instance_t g_transfer9;
#ifndef NULL
void NULL(transfer_callback_args_t *p_args);
#endif
#include "ux_api.h"
#include "ux_hcd_synergy.h"
#include "sf_el_ux_hcd_hs_cfg.h"
/* Transfer on DTC Instance. */
extern const transfer_instance_t g_transfer8;
#ifndef NULL
void NULL(transfer_callback_args_t *p_args);
#endif
/* Transfer on DTC Instance. */
extern const transfer_instance_t g_transfer7;
#ifndef NULL
void NULL(transfer_callback_args_t *p_args);
#endif
extern const i2c_cfg_t g_i2c0_cfg;
/** I2C on RIIC Instance. */
extern const i2c_master_instance_t g_i2c0;
#ifndef NULL
void NULL(i2c_callback_args_t *p_args);
#endif

extern riic_instance_ctrl_t g_i2c0_ctrl;
extern const riic_extended_cfg g_i2c0_extend;
#define SYNERGY_NOT_DEFINED (1)            
#if (SYNERGY_NOT_DEFINED == g_transfer7)
    #define g_i2c0_P_TRANSFER_TX (NULL)
#else
#define g_i2c0_P_TRANSFER_TX (&g_transfer7)
#endif
#if (SYNERGY_NOT_DEFINED == g_transfer8)
    #define g_i2c0_P_TRANSFER_RX (NULL)
#else
#define g_i2c0_P_TRANSFER_RX (&g_transfer8)
#endif
#undef SYNERGY_NOT_DEFINED
#define g_i2c0_P_EXTEND (&g_i2c0_extend)
extern sf_i2c_bus_t g_sf_i2c_bus0;
extern i2c_api_master_t const g_i2c_master_on_riic;

#define g_sf_i2c_bus0_CHANNEL        (1)
#define g_sf_i2c_bus0_RATE           (I2C_RATE_FAST)
#define g_sf_i2c_bus0_SLAVE          (0)
#define g_sf_i2c_bus0_ADDR_MODE      (I2C_ADDR_MODE_7BIT)          
#define g_sf_i2c_bus0_SDA_DELAY      (300)  
#define g_sf_i2c_bus0_P_CALLBACK     (NULL)
#define g_sf_i2c_bus0_P_CONTEXT      (&g_i2c0)
#define g_sf_i2c_bus0_RXI_IPL        ((3))
#define g_sf_i2c_bus0_TXI_IPL        ((3))
#define g_sf_i2c_bus0_TEI_IPL        ((3))            
#define g_sf_i2c_bus0_ERI_IPL        ((3))

/** These are obtained by macros in the I2C driver XMLs. */
#define g_sf_i2c_bus0_P_TRANSFER_TX  (g_i2c0_P_TRANSFER_TX)
#define g_sf_i2c_bus0_P_TRANSFER_RX  (g_i2c0_P_TRANSFER_RX)            
#define g_sf_i2c_bus0_P_EXTEND       (g_i2c0_P_EXTEND)

extern const jpeg_decode_instance_t g_jpeg_decode0;
extern const jpeg_decode_cfg_t g_jpeg_decode0_cfg;
#ifndef NULL
void NULL(jpeg_decode_callback_args_t *p_args);
#endif
extern const sf_jpeg_decode_instance_t g_sf_jpeg_decode0;
/* Display on GLCD Instance. */
extern const display_instance_t g_display0;
extern display_runtime_cfg_t g_display0_runtime_cfg_fg;
extern display_runtime_cfg_t g_display0_runtime_cfg_bg;
#if (false)
            extern display_clut_cfg_t g_display0_clut_cfg_glcd;
            #endif
#ifndef NULL
void NULL(display_callback_args_t *p_args);
#endif
#if (true)
            #define DISPLAY_IN_FORMAT_16BITS_RGB565_0
            #if defined (DISPLAY_IN_FORMAT_32BITS_RGB888_0) || defined (DISPLAY_IN_FORMAT_32BITS_ARGB8888_0)
            #define DISPLAY_BITS_PER_PIXEL_INPUT0 (32)
            #elif defined (DISPLAY_IN_FORMAT_16BITS_RGB565_0) || defined (DISPLAY_IN_FORMAT_16BITS_ARGB1555_0) || defined (DISPLAY_IN_FORMAT_16BITS_ARGB4444_0)
            #define DISPLAY_BITS_PER_PIXEL_INPUT0 (16)
            #elif defined (DISPLAY_IN_FORMAT_CLUT8_0)
            #define DISPLAY_BITS_PER_PIXEL_INPUT0 (8)
            #elif defined (DISPLAY_IN_FORMAT_CLUT4_0)
            #define DISPLAY_BITS_PER_PIXEL_INPUT0 (4)
            #else
            #define DISPLAY_BITS_PER_PIXEL_INPUT0 (1)
            #endif
            extern uint8_t g_display0_fb_background[2][((800 * 480) * DISPLAY_BITS_PER_PIXEL_INPUT0) >> 3];
            #endif
#if (false)
            #define DISPLAY_IN_FORMAT_16BITS_RGB565_1
            #if defined (DISPLAY_IN_FORMAT_32BITS_RGB888_1) || defined (DISPLAY_IN_FORMAT_32BITS_ARGB8888_1)
            #define DISPLAY_BITS_PER_PIXEL_INPUT1 (32)
            #elif defined (DISPLAY_IN_FORMAT_16BITS_RGB565_1) || defined (DISPLAY_IN_FORMAT_16BITS_ARGB1555_1) || defined (DISPLAY_IN_FORMAT_16BITS_ARGB4444_1)
            #define DISPLAY_BITS_PER_PIXEL_INPUT1 (16)
            #elif defined (DISPLAY_IN_FORMAT_CLUT8_1)
            #define DISPLAY_BITS_PER_PIXEL_INPUT1 (8)
            #elif defined (DISPLAY_IN_FORMAT_CLUT4_1)
            #define DISPLAY_BITS_PER_PIXEL_INPUT1 (4)
            #else
            #define DISPLAY_BITS_PER_PIXEL_INPUT1 (1)
            #endif
            extern uint8_t g_display0_fb_foreground[2][((800 * 480) * DISPLAY_BITS_PER_PIXEL_INPUT1) >> 3];
            #endif
#ifndef NULL
extern void NULL(sf_el_gx_callback_args_t *p_arg);
#endif
extern sf_el_gx_instance_t g_sf_el_gx;

#include "ux_api.h"

/* USBX Host Stack initialization error callback function. User can override the function if needed. */
void ux_v2_err_callback(void *p_instance, void *p_data);

#if !defined(ux_host_event_callback)
/* User Callback for Host Event Notification (Only valid for USB Host). */
extern UINT ux_host_event_callback(ULONG event, UX_HOST_CLASS *host_class,
		VOID *instance);
#endif

#if !defined(NULL)
/* User Callback for Device Event Notification (Only valid for USB Device). */
extern UINT NULL(ULONG event);
#endif

#ifdef UX_HOST_CLASS_STORAGE_H
            /* Utility function to get the pointer to a FileX Media Control Block for a USB Mass Storage device. */
            UINT ux_system_host_storage_fx_media_get(UX_HOST_CLASS_STORAGE * instance, UX_HOST_CLASS_STORAGE_MEDIA ** p_storage_media, FX_MEDIA ** p_fx_media);
#endif
void ux_common_init0(void);

/* Function prototype for the function to register the USBX Host Class Mass Storage. */
void ux_host_stack_class_register_storage(void);
/* Function prototype for the function to notify a USB event from the USBX Host system. */
UINT ux_system_host_change_function(ULONG event, UX_HOST_CLASS *host_class,
		VOID *instance);
/* memory pool allocation used by USBX system. */
extern CHAR g_ux_pool_memory[];
void g_ux_host_0_err_callback(void *p_instance, void *p_data);
void ux_host_init0(void);
#include "ux_api.h"
#include "ux_host_class_video.h"

/* Pointer to a USBX Host Video Class Instance */
extern UX_HOST_CLASS_VIDEO *g_ux_host_class_video0;
#include "ux_api.h"
#include "ux_host_class_hub.h"
#include "ux_host_class_hid.h"
#include "ux_host_class_hid_keyboard.h"
#include "ux_host_class_hid_mouse.h"
#include "ux_host_class_hid_remote_control.h"

void g_ux_host_class_hid0_err_callback(void *p_instance, void *p_data);
#include "ux_api.h"
#include "ux_host_class_storage.h"

/* Pointer to a USBX Host Mass Storage Class Instance */
extern UX_HOST_CLASS_STORAGE *g_ux_host_class_storage0;
/* A pointer to FileX Media Control Block for a USB flash device */
extern FX_MEDIA *g_fx_media0_ptr;

/* Pointer to a USBX Host Mass Storage Class Media */
extern UX_HOST_CLASS_STORAGE_MEDIA *g_ux_host_class_storage0_media;

/* Macro to dereference a pointer to FileX Media Control Block for a USB flash device */
#define  g_fx_media0 (*(FX_MEDIA*)g_fx_media0_ptr)

/* The function prototype of getting g_fx_media0 for a USB Mass Storage device. */
void fx_media_init_function0(void);

/* FileX media initialization error callback function for USBX Host Mass Storage. User can override the function if needed. */
void g_fx_media0_err_callback_failed_to_get_fx_media(void *p_instance,
		void *p_data);
#include "ux_api.h"
#include "ux_dcd_synergy.h"
#include "sf_el_ux_dcd_fs_cfg.h"
void g_sf_el_ux_dcd_fs_0_err_callback(void *p_instance, void *p_data);
#include "ux_api.h"
#include "ux_dcd_synergy.h"

/* USBX Device Stack initialization error callback function. User can override the function if needed. */
void ux_device_err_callback(void *p_instance, void *p_data);
void ux_device_init0(void);
void ux_device_remove_compiler_padding(unsigned char *p_device_framework,
		UINT length);
/* Header section starts for g_ux_device_class_cdc_acm0 */
#include "ux_api.h"
#include "ux_device_class_cdc_acm.h"
/* USBX CDC-ACM Instance Activate User Callback Function */
extern VOID ux_cdc_device0_instance_activate(VOID *cdc_instance);
/* USBX CDC-ACM Instance Deactivate User Callback Function */
extern VOID ux_cdc_device0_instance_deactivate(VOID *cdc_instance);
/* USBX CDC-ACM Instance parameter change Function */
#ifndef NULL
extern VOID NULL(VOID *cdc_instance);
#endif
/* Header section ends for g_ux_device_class_cdc_acm0 */
void ux_device_class_cdc_acm_init0(void);
void g_ux_device_class_cdc_acm0_ux_device_open_init(void);
/** IOPORT Instance */
extern const ioport_instance_t g_ioport;
/** ELC Instance */
extern const elc_instance_t g_elc;
/** CGC Instance */
extern const cgc_instance_t g_cgc;
/** FMI on FMI Instance. */
extern const fmi_instance_t g_fmi;
void g_common_init(void);
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* COMMON_DATA_H_ */
