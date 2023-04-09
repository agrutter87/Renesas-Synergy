/* generated thread header file - do not edit */
#ifndef DEBUG_THREAD_H_
#define DEBUG_THREAD_H_
#include "bsp_api.h"
#include "tx_api.h"
#include "hal_data.h"
#ifdef __cplusplus
                extern "C" void debug_thread_entry(void);
                #else
extern void debug_thread_entry(void);
#endif
#ifdef __cplusplus
extern "C" {
#endif
extern TX_QUEUE g_debug_queue;
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* DEBUG_THREAD_H_ */
