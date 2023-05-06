/* generated thread header file - do not edit */
#ifndef ACCELEROMETER_THREAD_H_
#define ACCELEROMETER_THREAD_H_
#include "bsp_api.h"
#include "tx_api.h"
#include "hal_data.h"
#ifdef __cplusplus
                extern "C" void accelerometer_thread_entry(void);
                #else
extern void accelerometer_thread_entry(void);
#endif
#include "r_dtc.h"
#include "r_transfer_api.h"
#include "r_sci_spi.h"
#include "r_spi_api.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Transfer on DTC Instance. */
extern const transfer_instance_t g_transfer3;
#ifndef NULL
void NULL(transfer_callback_args_t *p_args);
#endif
/* Transfer on DTC Instance. */
extern const transfer_instance_t g_transfer2;
#ifndef NULL
void NULL(transfer_callback_args_t *p_args);
#endif
extern const spi_cfg_t g_sci_spi8_cfg;
/** SPI on SCI Instance. */
extern const spi_instance_t g_sci_spi8;
extern sci_spi_instance_ctrl_t g_sci_spi8_ctrl;
extern const sci_spi_extended_cfg g_sci_spi8_cfg_extend;

#ifndef sci_spi8_callback
void sci_spi8_callback(spi_callback_args_t *p_args);
#endif

#define SYNERGY_NOT_DEFINED (1)            
#if (SYNERGY_NOT_DEFINED == g_transfer2)
    #define g_sci_spi8_P_TRANSFER_TX (NULL)
#else
#define g_sci_spi8_P_TRANSFER_TX (&g_transfer2)
#endif
#if (SYNERGY_NOT_DEFINED == g_transfer3)
    #define g_sci_spi8_P_TRANSFER_RX (NULL)
#else
#define g_sci_spi8_P_TRANSFER_RX (&g_transfer3)
#endif
#undef SYNERGY_NOT_DEFINED

#define g_sci_spi8_P_EXTEND (&g_sci_spi8_cfg_extend)
extern TX_EVENT_FLAGS_GROUP g_sci_spi8_event_flags;
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* ACCELEROMETER_THREAD_H_ */
