/*-------------------------------------------------------------------------*
 * File:  gui_thread_entry.c
 *-------------------------------------------------------------------------*
 * Description:
 */
 /**
 *    @addtogroup gui_thread_entry
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
#include <gui.h>

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
static bool ssp_touch_event_to_guix_event(sf_touch_panel_v2_payload_t * p_touch_payload, GX_EVENT * g_gx_event);

/*---------------------------------------------------------------------------*
 * Function: gui_thread_entry
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
void gui_thread_entry(void)
{
    ssp_err_t 					ssp_err 			= SSP_SUCCESS;
    UINT 						gx_status 			= GX_SUCCESS;
    GX_CONST GX_STUDIO_WIDGET 	**pp_studio_widget 	= &guix_widget_table[0];

    /* Initializes GUIX. */
    gx_status = gx_system_initialize ();
    if(gx_status)
    {
        // TODO: Error handling
        debug_print("\r\nError at gui_thread_entry::gx_system_initialize\r\n");
    }

    /* Initializes GUIX drivers. */
    ssp_err = g_sf_el_gx.p_api->open (g_sf_el_gx.p_ctrl, g_sf_el_gx.p_cfg);
    if(ssp_err)
    {
        // TODO: Error handling
        debug_print("\r\nError at gui_thread_entry::g_sf_el_gx.p_api->open\r\n");
    }

    gx_status = gx_studio_display_configure ( DISPLAY_1, g_sf_el_gx.p_api->setup, LANGUAGE_ENGLISH,
                                 DISPLAY_1_THEME_1, &p_window_root);
    if(gx_status)
    {
        // TODO: Error handling
        debug_print("\r\nError at gui_thread_entry::gx_studio_display_configure\r\n");
    }

    ssp_err = g_sf_el_gx.p_api->canvasInit (g_sf_el_gx.p_ctrl, p_window_root);
    if(ssp_err)
    {
        // TODO: Error handling
        debug_print("\r\nError at gui_thread_entry::g_sf_el_gx.p_api->canvasInit\r\n");
    }

    // Create the widgets we have defined with the GUIX data structures and resources.
    while (GX_NULL != *pp_studio_widget)
    {
        // We must first create the widgets according the data generated in GUIX Studio.

        // Once we are working on the widget we want to see first, save the pointer for later.
        if (0 == strcmp ("init_window", (char*) (*pp_studio_widget)->widget_name))
        {
            gx_status = gx_studio_named_widget_create ((*pp_studio_widget)->widget_name, (GX_WIDGET *) p_window_root, GX_NULL);
            if(gx_status)
            {
                // TODO: Error handling
                debug_print("\r\nError at gui_thread_entry::gx_studio_named_widget_create\r\n");
            }
        }
        else
        {
            gx_status = gx_studio_named_widget_create ((*pp_studio_widget)->widget_name, GX_NULL, GX_NULL);
            if(gx_status)
            {
                // TODO: Error handling
                debug_print("\r\nError at gui_thread_entry::gx_studio_named_widget_create\r\n");
            }
        }
        // Move to next top-level widget
        pp_studio_widget++;
    }

    /* Shows the root window to make it and patients screen visible. */
    gx_status = gx_widget_show(p_window_root);
    if(gx_status)
    {
        // TODO: Error handling
        debug_print("\r\nError at gui_thread_entry::gx_widget_show\r\n");
    }

    /* Lets GUIX run. */
    gx_status = gx_system_start ();
    if(gx_status)
    {
        // TODO: Error handling
        debug_print("\r\nError at gui_thread_entry::gx_system_start\r\n");
    }

#if defined(BSP_BOARD_S7G2_SK)
    /** Open the SPI driver to initialize the LCD (SK-S7G2) **/
    ssp_err = g_spi_lcdc.p_api->open (g_spi_lcdc.p_ctrl, (spi_cfg_t *) g_spi_lcdc.p_cfg);
    if(ssp_err)
    {
        // TODO: Error handling
        debug_print("\r\nError at gui_thread_entry::g_spi_lcdc.p_api->open\r\n");
    }

    /** Setup the ILI9341V (SK-S7G2) **/
    ILI9341V_Init ();
#endif

    /* Controls the GPIO pin for LCD ON (DK-S7G2, PE-HMI1) */
#if defined(BSP_BOARD_S7G2_DK)
    ssp_err = g_ioport.p_api->pinWrite(IOPORT_PORT_07_PIN_10, IOPORT_LEVEL_HIGH);
    if(ssp_err)
    {
        // TODO: Error handling
        debug_print("\r\nError at gui_thread_entry::g_ioport.p_api->pinWrite\r\n");
    }
#elif defined(BSP_BOARD_S7G2_PE_HMI1)
    ssp_err = g_ioport.p_api->pinWrite(IOPORT_PORT_10_PIN_03, IOPORT_LEVEL_HIGH);
    if(ssp_err)
    {
        // TODO: Error handling
        debug_print("\r\nError at gui_thread_entry::g_ioport.p_api->pinWrite\r\n");
    }
#endif

    /* Opens PWM driver and controls the TFT panel back light (DK-S7G2, PE-HMI1) */
#if defined(BSP_BOARD_S7G2_DK) || defined(BSP_BOARD_S7G2_PE_HMI1)
    ssp_err = g_backlight_pwm.p_api->open(g_backlight_pwm.p_ctrl, g_backlight_pwm.p_cfg);
    if(ssp_err)
    {
        // TODO: Error handling
        debug_print("\r\nError at gui_thread_entry::g_backlight_pwm.p_api->open\r\n");
    }
#endif

	/* Let thread die, with updated Synergy V2 touch, this thread is no longer needed to handle touch events */
}

/*---------------------------------------------------------------------------*
 * Function: ssp_touch_event_to_guix_event
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
static bool ssp_touch_event_to_guix_event(sf_touch_panel_v2_payload_t * p_touch_payload, GX_EVENT * gx_event)
{
    bool send_event = true;

    switch (p_touch_payload->event_type)
    {
        case SF_TOUCH_PANEL_V2_EVENT_DOWN:
            gx_event->gx_event_type = GX_EVENT_PEN_DOWN;
        break;

        case SF_TOUCH_PANEL_V2_EVENT_UP:
            gx_event->gx_event_type = GX_EVENT_PEN_UP;
        break;

        case SF_TOUCH_PANEL_V2_EVENT_HOLD:
        case SF_TOUCH_PANEL_V2_EVENT_MOVE:
            gx_event->gx_event_type = GX_EVENT_PEN_DRAG;
        break;

        case SF_TOUCH_PANEL_V2_EVENT_INVALID:
        case SF_TOUCH_PANEL_V2_EVENT_NONE:
        default:
            send_event = false;
        break;
    }

    if (send_event)
    {
        /** Send event to GUI */
        gx_event->gx_event_sender = GX_ID_NONE;
        gx_event->gx_event_target = 0;
        gx_event->gx_event_display_handle = 0;

        gx_event->gx_event_payload.gx_event_pointdata.gx_point_x = p_touch_payload->x;

#if defined(BSP_BOARD_S7G2_SK)
        gx_event->gx_event_payload.gx_event_pointdata.gx_point_y = (320 - p_touch_payload->y);  // SK-S7G2
#else
        gx_event->gx_event_payload.gx_event_pointdata.gx_point_y = p_touch_payload->y;  // DK-S7G2, PE-HMI1
#endif

    }

    return send_event;
}

void touch_callback(sf_touchpanel_v2_callback_args_t *p_args)
{
    UINT 		gx_status 		= GX_SUCCESS;
    GX_EVENT 	g_gx_event		= { 0 };
    bool 		new_gui_event 	= false;

    new_gui_event = ssp_touch_event_to_guix_event (&p_args->payload, &g_gx_event);

    /** Post message. */
    if (new_gui_event)
    {
        gx_status = gx_system_event_send (&g_gx_event);
        if(gx_status)
        {
            // TODO: Error handling
            debug_print("\r\nError at gui_thread_entry::gx_system_event_send\r\n");
        }
    }
}

#if defined(BSP_BOARD_S7G2_SK)
void g_lcd_spi_callback(spi_callback_args_t * p_args)
{
    (void) p_args;
    tx_semaphore_ceiling_put (&g_main_semaphore_lcdc, 1);
}
#endif
