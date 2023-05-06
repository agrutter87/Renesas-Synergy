/*-------------------------------------------------------------------------*
 * File:  gui.c
 *-------------------------------------------------------------------------*
 * Description:
 */
 /**
 *    @addtogroup gui
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
GX_WINDOW_ROOT 				*p_window_root;
/* Create the widgets we have defined with the GUIX data structures and resources. */
GX_CONST GX_STUDIO_WIDGET 	**pp_studio_widget = (GX_CONST GX_STUDIO_WIDGET **) &GUIX_widget_table[0];

TX_BYTE_POOL 				guix_memory_pool;
uint8_t 					guix_memory_pool_bytes[MEMORY_POOL_SIZE_BYTES] BSP_PLACE_IN_SECTION_V2(".sdram");

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
static void guix_init_widgets(void);
static void *guix_memory_allocate(ULONG size);
static void guix_memory_free(VOID *mem);

/*---------------------------------------------------------------------------*
 * Function: guix_init_widgets
 *---------------------------------------------------------------------------*/
static void guix_init_widgets(void)
{
    /* We must first create the widgets according the data generated in GUIX Studio. */
    while (GX_NULL != *pp_studio_widget)
    {
        /* Once we are working on the widget we want to see first, save the pointer for later. */
        if (0 == strcmp ("w_splash", (char*) (*pp_studio_widget)->widget_name))
        {
            gx_studio_named_widget_create ((*pp_studio_widget)->widget_name, (GX_WIDGET *) p_window_root, GX_NULL);
        }
        else
        {
            gx_studio_named_widget_create ((*pp_studio_widget)->widget_name, GX_NULL, GX_NULL);
        }

        /* Move to next top-level widget */
        pp_studio_widget++;
    }
}

/*---------------------------------------------------------------------------*
* Function: guix_memory_allocate
*---------------------------------------------------------------------------*/
void *guix_memory_allocate(ULONG size)
{
   VOID *memptr;
   UINT txstatus = TX_SUCCESS;

   if(guix_memory_pool.tx_byte_pool_size == 0)
   {
       txstatus = tx_byte_pool_create(&guix_memory_pool, "GUIX Memory Byte Pool", guix_memory_pool_bytes, MEMORY_POOL_SIZE_BYTES);
   }

   if(txstatus == TX_SUCCESS)
   {
       txstatus = tx_byte_allocate(&guix_memory_pool, &memptr, size, TX_NO_WAIT);

       if (txstatus == TX_SUCCESS)
       {
           return memptr;
       }
       else
       {
           return NULL;
       }
   }
   else
   {
       return NULL;
   }
}

/*---------------------------------------------------------------------------*
* Function: guix_memory_free
*---------------------------------------------------------------------------*/
void guix_memory_free(VOID *mem)
{
   UINT txstatus = TX_SUCCESS;

   txstatus = tx_byte_release(mem);

   if(txstatus == TX_SUCCESS)
   {
       return;
   }
   else
   {
       return;
   }
}

/*---------------------------------------------------------------------------*
* Function: guix_init
*---------------------------------------------------------------------------*/
void guix_init(void)
{
   ssp_err_t err = SSP_SUCCESS;
   UINT status = TX_SUCCESS;

   /* Initializes GUIX */
   status = gx_system_initialize ();
   APP_ERROR_TRAP(status)

   /* Opens GUIX framework */
   status = g_sf_el_gx.p_api->open (g_sf_el_gx.p_ctrl, g_sf_el_gx.p_cfg);
   APP_ERROR_TRAP(status)

   /* Configures display using setup information within GUIX framework. This starts the LCD signals. */
   status = gx_studio_display_configure (0, g_sf_el_gx.p_api->setup, 0, 0, &p_window_root);
   APP_ERROR_TRAP(status)

   status = gx_system_memory_allocator_set(guix_memory_allocate, guix_memory_free);
   APP_ERROR_TRAP(status)

   /* Initialize drawing canvas */
   status = g_sf_el_gx.p_api->canvasInit (g_sf_el_gx.p_ctrl, p_window_root);
   APP_ERROR_TRAP(status)

   guix_init_widgets(); // Initialize guix elements

   status = gx_widget_show(p_window_root);
   APP_ERROR_TRAP(status)

   status = gx_system_start ();
   APP_ERROR_TRAP(status)

#ifdef BSP_BOARD_S7G2_SK
    /** Open the SPI driver to initialize the LCD and setup ILI9341V driver **/
    status = ILI9341V_Init(&g_spi0);
    APP_ERROR_TRAP(status)
#endif

#ifdef BSP_BOARD_S5D9_PK
    /** Open the SPI driver to initialize the LCD and setup ILI9341V driver **/
    status = ILI9341V_Init(&g_spi0);
    APP_ERROR_TRAP(status)
#endif

#ifdef BSP_BOARD_S7G2_DK
    /** Enable the display */
    status = g_ioport.p_api->pinWrite(IOPORT_PORT_07_PIN_10, IOPORT_LEVEL_HIGH);
    APP_ERROR_TRAP(status)

    /** Enable display backlight */
    status = g_ioport.p_api->pinWrite(IOPORT_PORT_07_PIN_12, IOPORT_LEVEL_HIGH);
    APP_ERROR_TRAP(status)
#endif

#ifdef BSP_BOARD_S7G2_PE_HMI1
    /** Enable the display */
    status = g_ioport.p_api->pinWrite(IOPORT_PORT_10_PIN_03, IOPORT_LEVEL_HIGH);
    APP_ERROR_TRAP(status)

    /** Enable display backlight */
    ssp_err_t ssp_err = g_backlight_pwm.p_api->open(g_backlight_pwm.p_ctrl, g_backlight_pwm.p_cfg);
    APP_ERROR_TRAP(ssp_err)
#endif
}



/*---------------------------------------------------------------------------*
* Function: guix_show_window
*---------------------------------------------------------------------------*/
UINT guix_show_window(GX_WINDOW * p_new, GX_WIDGET * p_widget, bool detach_old)
{
   UINT err = GX_SUCCESS;

   if (!p_new->gx_widget_parent)
   {
       err = gx_widget_attach(p_window_root, p_new);
       gx_system_focus_claim(p_new);

       if (p_widget && detach_old)
       {
           if (p_widget != (GX_WIDGET*) p_new)
           {
               gx_widget_detach(p_widget);
           }
       }
   }
   else
   {
       err = gx_widget_show(p_new);
       gx_system_focus_claim(p_new);

       if (p_widget && detach_old)
       {
           if (p_widget != (GX_WIDGET*) p_new)
           {
               gx_widget_hide(p_widget);
           }
       }
   }


   return err;
}
