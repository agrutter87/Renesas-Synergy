/*-------------------------------------------------------------------------*
 * File:  cli_adc.c
 *-------------------------------------------------------------------------*
 * Description:
 *-------------------------------------------------------------------------*/

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
 * Function: cli_init_adc_callback
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
void cli_init_adc_callback(sf_console_callback_args_t * p_args)
{
    ssp_err_t ssp_err = SSP_SUCCESS;
    int32_t arg_index = 0;
    int32_t arg_data = 0;
    bool invalid_arg = false;

    adc_instance_t * p_adc = NULL;

    adc_cfg_t adc_cfg_temp =
    {
      .unit = 0,
      .mode = ADC_MODE_SINGLE_SCAN,
      .resolution = ADC_RESOLUTION_12_BIT,
      .alignment = ADC_ALIGNMENT_RIGHT,
      .add_average_count = ADC_ADD_OFF,
      .clearing = ADC_CLEAR_AFTER_READ_ON,
      .trigger = ADC_TRIGGER_SOFTWARE,
      .trigger_group_b = ADC_TRIGGER_SYNC_ELC,
      .p_callback = adc_callback,
      .p_context = &g_adc0,
      .scan_end_ipl = (6),
      .scan_end_b_ipl = (6),
    };

    adc_channel_cfg_t adc_channel_cfg_temp =
    {
      .scan_mask = (uint32_t) (((uint64_t) ADC_MASK_CHANNEL_0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
            | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
            | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
            | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
            | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
            | ((uint64_t) 0) | (0)),
      /** Group B channel mask is right shifted by 32 at the end to form the proper mask */
      .scan_mask_group_b = (uint32_t) ((((uint64_t) ADC_MASK_CHANNEL_0) | ((uint64_t) 0) | ((uint64_t) 0)
              | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
              | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
              | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
              | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
              | ((uint64_t) 0) | ((uint64_t) 0) | (0)) >> 32),
      .priority_group_a = ADC_GROUP_A_PRIORITY_OFF,
      .add_mask = (uint32_t) ((0) | (0) | (0) | (0) | (0) | (0) | (0)
              | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0)
              | (0) | (0) | (0) | (0) | (0) | (0)),
      .sample_hold_mask = (uint32_t) ((0) | (0) | (0)),
      .sample_hold_states = 24,
    };

    /* If there is any arguments, process them */
    if (*(p_args->p_remaining_string))
    {
        ssp_err = g_sf_console.p_api->argumentFind ((uint8_t *) "unit", p_args->p_remaining_string, &arg_index, &arg_data);
        if (arg_index > -1)
        {
            switch(arg_data)
            {
                case 0:
                    p_adc = &g_adc0;
                    adc_cfg_temp.unit = 0;
                    break;
                default:
                    debug_print ("\r\nInvalid unit entered\r\n");
                    invalid_arg = true;
            }
        }

        ssp_err = g_sf_console.p_api->argumentFind ((uint8_t *) "mode", p_args->p_remaining_string, &arg_index, &arg_data);
        if (arg_index > -1)
        {
            switch(arg_data)
            {
                case ADC_MODE_SINGLE_SCAN:
                    adc_cfg_temp.mode = arg_data;
                    adc_cfg_temp.p_callback = adc_callback;
                    adc_cfg_temp.scan_end_b_ipl = BSP_IRQ_DISABLED;
                    break;
                case ADC_MODE_GROUP_SCAN:
                    adc_cfg_temp.mode = arg_data;
                    adc_cfg_temp.p_callback = NULL;
                    break;
                case ADC_MODE_CONTINUOUS_SCAN:
                    adc_cfg_temp.mode = arg_data;
                    adc_cfg_temp.p_callback = NULL;
                    adc_cfg_temp.scan_end_ipl = BSP_IRQ_DISABLED;
                    adc_cfg_temp.scan_end_b_ipl = BSP_IRQ_DISABLED;
                    break;
                default:
                    debug_print ("\r\nInvalid mode entered\r\n");
                    invalid_arg = true;
            }
        }

        ssp_err = g_sf_console.p_api->argumentFind ((uint8_t *) "resolution", p_args->p_remaining_string, &arg_index, &arg_data);
        if (arg_index > -1)
        {
            switch(arg_data)
            {
                case ADC_RESOLUTION_12_BIT:
#if defined(BSP_MCU_GROUP_S7G2)
                case ADC_RESOLUTION_10_BIT:
                case ADC_RESOLUTION_8_BIT:
#endif
#if defined(BSP_MCU_GROUP_S124) || defined(BSP_MCU_GROUP_S3A7)
                case ADC_RESOLUTION_14_BIT:
#endif
                    adc_cfg_temp.resolution = arg_data;
                    break;
                default:
                    debug_print ("\r\nInvalid resolution entered\r\n");
                    invalid_arg = true;
            }
        }
    }

    if(!invalid_arg)
    {
        if(p_adc == NULL)
        {
            p_adc = &g_adc0;
        }

        debug_print ("\r\nReconfiguring ADC...\r\n");

        tx_event_flags_set (&g_main_system_event_flags, MAIN_SYSTEM_EVENT_ADC_ENABLED, TX_AND);
        debug_print ("\r\nMAIN_SYSTEM_EVENT_ADC_DISABLED\r\n");

        ssp_err = p_adc->p_api->close(p_adc->p_ctrl);
        if(ssp_err)
        {
            // TODO: Error handling
            debug_print("\r\nError at cli_init_adc_callback::p_adc->p_api->close\r\n");
        }

        ssp_err = p_adc->p_api->open(p_adc->p_ctrl, &adc_cfg_temp);
        if(ssp_err)
        {
            // TODO: Error handling
            debug_print("\r\nError at cli_init_adc_callback::p_adc->p_api->open\r\n");
        }

        ssp_err = p_adc->p_api->scanCfg(p_adc->p_ctrl, &adc_channel_cfg_temp);
        if(ssp_err)
        {
            // TODO: Error handling
            debug_print("\r\nError at cli_init_adc_callback::p_adc->p_api->scanCfg\r\n");
        }

        tx_event_flags_set (&g_main_system_event_flags, MAIN_SYSTEM_EVENT_ADC_ENABLED, TX_OR);
        debug_print ("\r\nMAIN_SYSTEM_EVENT_ADC_ENABLED\r\n");
    }

}


/*-------------------------------------------------------------------------*
 * End of File:  cli_adc.c
 *-------------------------------------------------------------------------*/
