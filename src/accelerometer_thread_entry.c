/*-------------------------------------------------------------------------*
 * File:  accelerometer_thread_entry.c
 *-------------------------------------------------------------------------*
 * Description:
 */
/**
 *    @addtogroup accelerometer
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
#include <accelerometer.h>

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
 * Function: accelerometer_thread_entry
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
void accelerometer_thread_entry(void)
{
    /* SSP error variable used for tracking the status of Synergy API functions */
    ssp_err_t ssp_err = SSP_SUCCESS;

    /* Configure SCI_SPI8 */
    ssp_err = g_sci_spi8.p_api->open (g_sci_spi8.p_ctrl, g_sci_spi8.p_cfg);
    if (ssp_err)
    {
        // TODO: Error handling
        debug_print ((uint8_t *) "\r\nError at accelerometer_thread_entry::g_sci_spi8.p_api->open\r\n");
    }

    /* Set flag to allow threads which need the SCI_SPI8 to run after common drivers are initialized */
    tx_event_flags_set (&g_marsgro_system_event_flags, MARSGRO_SYSTEM_EVENT_SCI_SPI8_ENABLED, TX_OR);
    debug_print ((uint8_t *) "\r\nMARSGRO_SYSTEM_EVENT_SCI_SPI8_ENABLED\r\n");
}
