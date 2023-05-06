/*-------------------------------------------------------------------------*
 * File:  cli.c
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

/* Command list for root menu*/
const sf_console_command_t g_sf_console_root_menu_commands[] =
{{
   .command = (uint8_t *) ("init"),
   .help = (uint8_t *) ("Initializes a peripheral"),
   .callback = cli_init_callback,
   .context = NULL
}};

/* Command list for init menu*/
const sf_console_command_t g_sf_console_init_menu_commands[] =
{{
   .command = (uint8_t *) ("adc"),
   .help = (uint8_t *) ("Initializes ADC"),
   .callback = cli_init_adc_callback,
   .context = NULL
 },{
    .command = (uint8_t *) ("riic"),
    .help = (uint8_t *) ("Initializes RIIC"),
    .callback = cli_init_riic_callback,
    .context = NULL
},{
    .command = (uint8_t *) ("sci_spi"),
    .help = (uint8_t *) ("Initializes SCI SPI"),
    .callback = cli_init_sci_spi_callback,
    .context = NULL
}};

/* Root menu for debug CLI */
const sf_console_menu_t g_sf_console_root_menu =
{
  .menu_prev = NULL,
  .menu_name = (uint8_t *) ("root"),
  .num_commands = sizeof(g_sf_console_root_menu_commands) / sizeof(sf_console_command_t),
  .command_list = g_sf_console_root_menu_commands
};

const sf_console_menu_t g_sf_console_init_menu =
{
  .menu_prev = &g_sf_console_root_menu,
  .menu_name = (uint8_t *) ("root\\init"),
  .num_commands = sizeof(g_sf_console_init_menu_commands) / sizeof(sf_console_command_t),
  .command_list = g_sf_console_init_menu_commands
};

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Function: cli_init_callback
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
void cli_init_callback(sf_console_callback_args_t * p_args)
{
    SSP_PARAMETER_NOT_USED(p_args);
    sf_console_instance_ctrl_t * p_ctrl = p_args->p_ctrl;

    /* If there are any arguments, process them */
    if (*(p_args->p_remaining_string))
    {
        g_sf_console.p_api->parse (p_ctrl, &g_sf_console_init_menu, p_args->p_remaining_string,
                                   strlen ((char *)p_args->p_remaining_string));
    }
    else
    {
        p_ctrl->p_current_menu = &g_sf_console_init_menu;
    }
}

