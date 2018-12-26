/*-------------------------------------------------------------------------*
 * File:  debug.c
 *-------------------------------------------------------------------------*
 * Description:
 */
 /**
 *    @addtogroup debug
 *  @{
 *  @brief     TODO: Description
 *
 * @par Example code:
 * TODO: Description of overall example code
 * @par
 * @code
 * TODO: Insert example code
 * @endcode
 */
/*-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * Created by: Alex Grutter
 *--------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <debug.h>
#include <stdarg.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Function: debug_print
 *---------------------------------------------------------------------------*/
/** TODO: Description of Function
 *
 *  @param [in] p_args      Pointer to arguments provided by SSP
 *  @par Example Code:
 *  @code
 *  //TODO: provide example or remove if not needed
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
UINT debug_print(char * message, ...)
{
    UINT tx_err = TX_SUCCESS;
    va_list args_list;
    char message_buffer[SF_CONSOLE_MAX_WRITE_LENGTH];

    va_start(args_list, message);
    vsnprintf(message_buffer, sizeof(message_buffer), message, args_list);
    va_end(args_list);

    /* Write the debug message to the debug queue. If the CLI is disabled,
     * this will eventually return error TX_QUEUE_FULL. This will only matter
     * if the error code is checked by the function calling debug_print */
    tx_err = tx_queue_send(&g_debug_queue, message_buffer, 1);

    return tx_err;
}
