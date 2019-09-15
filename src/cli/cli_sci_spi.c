/*-------------------------------------------------------------------------*
 * File:  cli_sci_spi.c
 *-------------------------------------------------------------------------*
 * Description:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include "cli.h"

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
 * Function: cli_init_sci_spi_callback
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
void cli_init_sci_spi_callback(sf_console_callback_args_t * p_args)
{
    ssp_err_t ssp_err = SSP_SUCCESS;
    int32_t arg_index = 0;
    int32_t arg_data = 0;
    bool invalid_arg = false;

    spi_instance_t * p_spi = NULL;

    spi_cfg_t spi_cfg_temp =
    {
      .channel = 8,
      .rxi_ipl = 12,
      .txi_ipl = 12,
      .tei_ipl = 12,
      .eri_ipl = 12,
      .operating_mode = SPI_MODE_MASTER,
      .clk_phase = SPI_CLK_PHASE_EDGE_ODD,
      .clk_polarity = SPI_CLK_POLARITY_LOW,
      .mode_fault = SPI_MODE_FAULT_ERROR_DISABLE,
      .bit_order = SPI_BIT_ORDER_MSB_FIRST,
      .bitrate = 100000,
      .p_transfer_tx = NULL,
      .p_transfer_rx = NULL,
      .p_callback = sci_spi8_callback,
      .p_context = (void *) &g_sci_spi8,
      .p_extend = &g_sci_spi8_cfg_extend,
    };

    /* If there is any arguments, process them */
    if (*(p_args->p_remaining_string))
    {
        ssp_err = g_sf_console.p_api->argumentFind ((uint8_t *) "channel", p_args->p_remaining_string, &arg_index, &arg_data);
        if (arg_index > -1)
        {
            switch(arg_data)
            {
                case 8:
                    p_spi = &g_sci_spi8;
                    spi_cfg_temp.channel = 8;
                    spi_cfg_temp.p_context = (void *) &g_sci_spi8;
                    spi_cfg_temp.p_extend = &g_sci_spi8_cfg_extend;
                    break;
                default:
                    debug_print ("\r\nInvalid channel entered\r\n");
                    invalid_arg = true;
            }
        }

        ssp_err = g_sf_console.p_api->argumentFind ((uint8_t *) "clk_phase", p_args->p_remaining_string, &arg_index, &arg_data);
        if (arg_index > -1)
        {
            switch(arg_data)
            {
                case SPI_CLK_PHASE_EDGE_ODD:
                case SPI_CLK_PHASE_EDGE_EVEN:
                    spi_cfg_temp.clk_phase = arg_data;
                    break;
                default:
                    debug_print ("\r\nInvalid clk_phase entered\r\n");
                    invalid_arg = true;
            }
        }

        ssp_err = g_sf_console.p_api->argumentFind ((uint8_t *) "clk_polarity", p_args->p_remaining_string, &arg_index, &arg_data);
        if (arg_index > -1)
        {
            switch(arg_data)
            {
                case SPI_CLK_POLARITY_LOW:
                case SPI_CLK_POLARITY_HIGH:
                case ADC_RESOLUTION_8_BIT:
                    spi_cfg_temp.clk_polarity = arg_data;
                    break;
                default:
                    debug_print ("\r\nInvalid clk_polarity entered\r\n");
                    invalid_arg = true;
            }
        }
    }

    if(!invalid_arg)
    {
        if(p_spi == NULL)
        {
            p_spi = &g_sci_spi8;
        }

        debug_print ("\r\nReconfiguring SCI_SPI8...\r\n");

        tx_event_flags_set (&g_marsgro_system_event_flags, MARSGRO_SYSTEM_EVENT_SCI_SPI8_ENABLED, TX_AND);
        debug_print ("\r\nMARSGRO_SYSTEM_EVENT_SCI_SPI8_ENABLED\r\n");

        ssp_err = p_spi->p_api->close(p_spi->p_ctrl);
        if(ssp_err)
        {
            // TODO: Error handling
            debug_print("\r\nError at cli_init_sci_spi_callback::p_spi->p_api->close\r\n");
        }

        ssp_err = p_spi->p_api->open(p_spi->p_ctrl, &spi_cfg_temp);
        if(ssp_err)
        {
            // TODO: Error handling
            debug_print("\r\nError at cli_init_sci_spi_callback::p_spi->p_api->open\r\n");
        }

        tx_event_flags_set (&g_marsgro_system_event_flags, MARSGRO_SYSTEM_EVENT_SCI_SPI8_ENABLED, TX_OR);
        debug_print ("\r\nMARSGRO_SYSTEM_EVENT_SCI_SPI8_ENABLED\r\n");
    }

}

/*-------------------------------------------------------------------------*
 * End of File:  cli_sci_spi.c
 *-------------------------------------------------------------------------*/
