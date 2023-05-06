/*-------------------------------------------------------------------------*
 * File:  debug_thread_entry.c
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
 * Function: debug_thread_entry
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
/* Debug Thread entry function */
void debug_thread_entry(void)
{
    UINT tx_status = TX_SUCCESS;
    ssp_err_t ssp_err = SSP_SUCCESS;
    uint8_t debug_message[SF_CONSOLE_MAX_WRITE_LENGTH] = {0};
    uint32_t flags_state = 0;

    /* Get event flag to determine whether debug thread can process debug messages.
     * This will put the thread to sleep indefinitely if the CLI is never enabled.
     * This method doesn't support being unplugged during use */
    tx_status = tx_event_flags_get(&g_main_system_event_flags, MAIN_SYSTEM_EVENT_CLI_ENABLED, TX_OR, &flags_state, TX_WAIT_FOREVER);

    while(1)
    {
        /* Check for debug messages from other threads */
        tx_status = tx_queue_receive(&g_debug_queue, debug_message, TX_WAIT_FOREVER);
        if(tx_status)
        {
            // TODO: Error handling
        }

        /* Write debug message to console */
        ssp_err = g_sf_debug_comms.p_api->write(g_sf_debug_comms.p_ctrl, (uint8_t *)debug_message, strlen((char *)debug_message), 1);
        if(ssp_err)
        {
            // TODO: Error handling
        }
    }
}
