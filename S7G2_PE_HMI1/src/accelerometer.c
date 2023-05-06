/*-------------------------------------------------------------------------*
 * File:  accelerometer.c
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
 * Function: sci_spi8_callback
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
void sci_spi8_callback(spi_callback_args_t *p_args)
{
    switch (p_args->event)
    {
        case SPI_EVENT_TRANSFER_COMPLETE:
            tx_event_flags_set (&g_sci_spi8_event_flags, SCI_SPI8_EVENT_TRANSFER_COMPLETE, TX_OR);
        break;
        case SPI_EVENT_TRANSFER_ABORTED:
            tx_event_flags_set (&g_sci_spi8_event_flags, SCI_SPI8_EVENT_TRANSFER_ABORTED, TX_OR);
        break;
        case SPI_EVENT_ERR_MODE_FAULT:
            tx_event_flags_set (&g_sci_spi8_event_flags, SCI_SPI8_EVENT_ERR_MODE_FAULT, TX_OR);
        break;
        case SPI_EVENT_ERR_READ_OVERFLOW:
            tx_event_flags_set (&g_sci_spi8_event_flags, SCI_SPI8_EVENT_ERR_READ_OVERFLOW, TX_OR);
        break;
        case SPI_EVENT_ERR_PARITY:
            tx_event_flags_set (&g_sci_spi8_event_flags, SCI_SPI8_EVENT_ERR_PARITY, TX_OR);
        break;
        case SPI_EVENT_ERR_OVERRUN:
            tx_event_flags_set (&g_sci_spi8_event_flags, SCI_SPI8_EVENT_ERR_OVERRUN, TX_OR);
        break;
        case SPI_EVENT_ERR_FRAMING:
            tx_event_flags_set (&g_sci_spi8_event_flags, SCI_SPI8_EVENT_ERR_FRAMING, TX_OR);
        break;
        case SPI_EVENT_ERR_MODE_UNDERRUN:
            tx_event_flags_set (&g_sci_spi8_event_flags, SCI_SPI8_EVENT_ERR_MODE_UNDERRUN, TX_OR);
        break;
        default:
            // TODO: Error handling
            debug_print ("\r\nError at sci_spi8_callback\r\n");
        break;
    }
}
