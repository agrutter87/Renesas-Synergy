/* generated thread header file - do not edit */
#ifndef GUI_THREAD_H_
#define GUI_THREAD_H_
#include "bsp_api.h"
#include "tx_api.h"
#include "hal_data.h"
#ifdef __cplusplus
                extern "C" void gui_thread_entry(void);
                #else
extern void gui_thread_entry(void);
#endif
#include "r_icu.h"
#include "r_external_irq_api.h"
#include "sf_external_irq.h"
#include "sf_i2c.h"
#include "sf_i2c_api.h"
#include "sf_touch_panel_chip_ft5x06.h"
#include "sf_touch_panel_v2.h"
#include "sf_touch_panel_v2_api.h"
#ifdef __cplusplus
extern "C" {
#endif
/* External IRQ on ICU Instance. */
extern const external_irq_instance_t g_external_irq0;
#ifndef NULL
void NULL(external_irq_callback_args_t *p_args);
#endif
/** SF External IRQ on SF External IRQ Instance. */
extern const sf_external_irq_instance_t g_sf_external_irq0;
/* SF I2C on SF I2C Instance. */
extern const sf_i2c_instance_t g_sf_i2c_device0;
extern const sf_touch_panel_chip_instance_t g_touch_panel_chip_ft5x06;
extern const sf_touch_panel_chip_api_t g_sf_touch_panel_chip_ft5x06;
/** SF Touch Panel V2 on SF Touch Panel V2 Instance. */
extern const sf_touch_panel_v2_instance_t g_sf_touch_panel;
void g_sf_touch_panel_err_callback(void *p_instance, void *p_data);
#ifndef touch_callback
void touch_callback(sf_touchpanel_v2_callback_args_t *p_args);
#endif			
void sf_touch_panel_init(void);
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* GUI_THREAD_H_ */
