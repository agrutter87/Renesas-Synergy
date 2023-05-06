/* generated thread header file - do not edit */
#ifndef MAIN_SYSTEM_THREAD_H_
#define MAIN_SYSTEM_THREAD_H_
#include "bsp_api.h"
#include "tx_api.h"
#include "hal_data.h"
#ifdef __cplusplus
                extern "C" void main_system_thread_entry(void);
                #else
extern void main_system_thread_entry(void);
#endif
#include "r_adc.h"
#include "r_adc_api.h"
#ifdef __cplusplus
extern "C" {
#endif
/** ADC on ADC Instance. */
extern const adc_instance_t g_adc0;
#ifndef adc_callback
void adc_callback(adc_callback_args_t *p_args);
#endif
extern TX_EVENT_FLAGS_GROUP g_main_system_event_flags;
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* MAIN_SYSTEM_THREAD_H_ */
