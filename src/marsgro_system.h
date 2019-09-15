/*-------------------------------------------------------------------------*
 * File:  marsgro_system.h
 *-------------------------------------------------------------------------*
 * Description:
 */
 /**
 *	@file 	marsgro_system.h
 *  @brief 	TODO: Short description
 *
 *	TODO: Description of file
 */
/*-------------------------------------------------------------------------*/
#ifndef MARSGRO_SYSTEM_H_
#define MARSGRO_SYSTEM_H_

/*--------------------------------------------------------------------------
 * Created by: Alex Grutter
 *--------------------------------------------------------------------------
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <marsgro_system_thread.h>

#include <accelerometer.h>
#include <cli.h>
#include <debug.h>
#include <gui.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
/* MarsGro System Event Flags Definitions */
#define MARSGRO_SYSTEM_EVENT_ADC_ENABLED        (1 << 0)
#define MARSGRO_SYSTEM_EVENT_CLI_ENABLED        (1 << 1)
#define MARSGRO_SYSTEM_EVENT_IIC0_ENABLED       (1 << 2)
#define MARSGRO_SYSTEM_EVENT_SCI_SPI8_ENABLED   (1 << 3)

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/

/** TODO: Description, one for each prototype
 *
 *  @param [in] val1        TODO: param1
 *  @param [in] val2        TODO: param1
 *  @return                 TODO: Return Value
 */

#ifdef __cplusplus
}
#endif

#endif // MARSGRO_SYSTEM_H_
/*-------------------------------------------------------------------------*
 * End of File:  marsgro_system.h
 *-------------------------------------------------------------------------*/
