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
static bool g_cli_init_error = false;

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

    /* This error flag is set if the USB is disconnected when the application
     * starts. The timeout value is SF_EL_UX_COMMS_CFG_BUFFER_TIMEOUT_COUNT
     * and is set within the configurator */
    if(!g_cli_init_error)
    {
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
}

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
void g_sf_console_err_callback(void * p_instance, void * p_data)
{
    /** Suppress compiler warning for not using parameters. */
    SSP_PARAMETER_NOT_USED (p_instance);

    ssp_err_t * ssp_err = p_data;

    switch(*ssp_err)
    {
        case SSP_ERR_TIMEOUT:
            /* A timeout has occurred, likely because the USB was not connected
             * before starting the application */
            g_cli_init_error = true;
        break;

        default:
            /* An unhandled error has occurred. Check ssp_err and handle new case */
            BSP_CFG_HANDLE_UNRECOVERABLE_ERROR (0);
        break;
    }
}
