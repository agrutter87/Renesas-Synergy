/* generated thread header file - do not edit */
#ifndef USB_THREAD_H_
#define USB_THREAD_H_
#include "bsp_api.h"
#include "tx_api.h"
#include "hal_data.h"
#ifdef __cplusplus
                extern "C" void usb_thread_entry(void);
                #else
extern void usb_thread_entry(void);
#endif
#ifdef __cplusplus
extern "C" {
#endif
extern TX_EVENT_FLAGS_GROUP g_device_insert_eventflag;
extern TX_QUEUE g_jpeg_queue;
extern TX_SEMAPHORE g_data_received_semaphore;
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* USB_THREAD_H_ */
