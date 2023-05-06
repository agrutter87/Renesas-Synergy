/*-------------------------------------------------------------------------*
 * File:  gui.h
 *-------------------------------------------------------------------------*
 * Description:
 */
 /**
 *  @file   gui
 *  @brief  TODO: Short description
 *
 *  TODO: Description of file
 */
/*-------------------------------------------------------------------------*/
#ifndef GUI_H_
#define GUI_H_

/*--------------------------------------------------------------------------
 * Created by: Alex Grutter
 *--------------------------------------------------------------------------
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include "gx_common_include.h"

#if defined(BSP_BOARD_S7G2_SK)
#include <lcd/ILI9341V/lcd.h>
#endif

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define MEMORY_POOL_SIZE_BYTES (15000000)

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
void guix_init(void);

/** TODO: Description, one for each prototype
 *
 *  @param [in] val1        TODO: param1
 *  @param [in] val2        TODO: param1
 *  @return                 TODO: Return Value
 */
void guix_init(void);

/** TODO: Description, one for each prototype
 *
 *  @param [in] val1        TODO: param1
 *  @param [in] val2        TODO: param1
 *  @return                 TODO: Return Value
 */
UINT guix_show_window(GX_WINDOW * p_new, GX_WIDGET * p_widget, bool detach_old);

#ifdef __cplusplus
}
#endif

#endif /* GUI_H_ */
/*-------------------------------------------------------------------------*
 * End of File:  gui.h
 *-------------------------------------------------------------------------*/
