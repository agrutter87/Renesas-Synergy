/*-------------------------------------------------------------------------*
 * File:  marsgro_system_thread_entry.c
 *-------------------------------------------------------------------------*
 * Description:
 */
 /**
 *    @addtogroup marsgro_system
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
#include <marsgro_system.h>

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
 * Function: marsgro_system_thread_entry
 *---------------------------------------------------------------------------*/
/** TODO: Description of Function
 *
 *  @param [in] p_args      TODO: Pointer to arguments provided by SSP
 *  @par Example Code:
 *  @code
 *  //TODO: provide example or remove if not needed
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
/* MarsGro System Thread entry function */
void marsgro_system_thread_entry(void)
{
    /* SSP error variable used for tracking the status of Synergy API functions */
    ssp_err_t ssp_err = SSP_SUCCESS;

    /* Initialize ADC */
    ssp_err = g_adc0.p_api->open(g_adc0.p_ctrl, g_adc0.p_cfg);
    if(ssp_err)
    {
        // TODO: Error handling
        debug_print("\r\nError at marsgro_system_thread_entry::g_adc0.p_api->open\r\n");
    }

    /* Configure ADC */
    ssp_err = g_adc0.p_api->scanCfg(g_adc0.p_ctrl, g_adc0.p_channel_cfg);
    if(ssp_err)
    {
        // TODO: Error handling
        debug_print("\r\nError at marsgro_system_thread_entry::g_adc0.p_api->scanCfg\r\n");
    }

    /* Set flag to allow threads which need the ADC to run after common drivers are initialized */
    tx_event_flags_set(&g_marsgro_system_event_flags, MARSGRO_SYSTEM_EVENT_ADC_ENABLED, TX_OR);
    debug_print("\r\nMARSGRO_SYSTEM_EVENT_ADC_ENABLED\r\n");
}
