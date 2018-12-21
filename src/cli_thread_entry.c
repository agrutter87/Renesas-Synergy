/*-------------------------------------------------------------------------*
 * File:  cli_thread_entry.c
 *-------------------------------------------------------------------------*
 * Description:
 */
/**
 *    @addtogroup cli
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
#include <cli.h>

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
 * Function: cli_thread_entry
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
void cli_thread_entry(void)
{
    ssp_err_t ssp_err = SSP_SUCCESS;

    /* Set event flag to allow debug thread to process debug messages */
    tx_event_flags_set (&g_marsgro_system_event_flags, MARSGRO_SYSTEM_EVENT_CLI_ENABLED, TX_OR);

    while (1)
    {
        /* Prompt for user input */
        ssp_err = g_sf_console.p_api->prompt (g_sf_console.p_ctrl,
                                              ((sf_console_instance_ctrl_t *) (g_sf_console.p_ctrl))->p_current_menu,
                                              TX_NO_WAIT);
        if (ssp_err)
        {
            // TODO: Error handling
        }
    }
}
