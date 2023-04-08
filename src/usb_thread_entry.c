/*-------------------------------------------------------------------------*
 * File:  usb_thread_entry.c
 *-------------------------------------------------------------------------*
 * Description:
 */
 /**
 *    @addtogroup usb
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
#include "usb_thread.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
/* Path to a file where key will be stored */
#define KEYBOARD_FILE_NAME "keys.txt"

/* Helper macro for seeking to the end of file */
#define FILE_SEEK_END ((ULONG) 0xFFFFFFFFUL)

/* USB interface protocol id for keyboard */
#define KEYBOARD_DEVICE (1)

#if defined(BSP_BOARD_S7G2_DK) || defined(BSP_BOARD_S7G2_PE_HMI1)
#define LED_ON  IOPORT_LEVEL_HIGH
#define LED_OFF IOPORT_LEVEL_LOW
#else
#define LED_ON  IOPORT_LEVEL_LOW
#define LED_OFF IOPORT_LEVEL_HIGH
#endif


/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
/* USBX HID Host Class */
static UX_HOST_CLASS_HID * g_hid;

/* USBX mass Host Class */
static UX_HOST_CLASS_STORAGE * g_storage;

/* FileX file */
static FX_FILE g_file;

/* LED type structure */
bsp_leds_t leds;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
UINT write_key(uint8_t key);

/*---------------------------------------------------------------------------*
 * Function: write_key
 *---------------------------------------------------------------------------*/
/*
  Handles USB Host event. p_host_class Pointer to type of USB Host Class structure of the device connected.
  (HID/Storage) and Pointer to instance representing the device.
 */
UINT ux_host_event_callback(ULONG event, UX_HOST_CLASS * host_class, VOID * instance)
{
    UX_HOST_CLASS_STORAGE_MEDIA* p_ux_host_class_storage_media;

    /* Check if there is a device insertion.  */
    if ((ULONG) UX_DEVICE_INSERTION == event)
    {
        /* check for Interface Descriptors Interface protocol value to identify the device on the bus */
        if(((UX_HOST_CLASS_HID *)instance)->ux_host_class_hid_interface->ux_interface_descriptor.bInterfaceProtocol == KEYBOARD_DEVICE)
        {
            #ifdef SEMI_HOSTING
            if (CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk)
            {
                /* Debugger is connected */
                /* Call this before any calls to printf() */
                printf("HID device: Keyboard is connected \n");
            }
            #endif

            g_ioport.p_api->pinWrite(leds.p_leds[0], LED_ON);
            g_hid = instance;
        }
        else if (UX_SUCCESS
                == _ux_utility_memory_compare (_ux_system_host_class_storage_name, host_class,
                                               _ux_utility_string_length_get(_ux_system_host_class_storage_name)))
        {
            // Get the pointer to the media
            ux_system_host_storage_fx_media_get (instance, &p_ux_host_class_storage_media, &g_fx_media0_ptr);

            #ifdef SEMI_HOSTING
            if (CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk)
            {
                /* Debugger is connected */
                /* Call this before any calls to printf() */
                printf("Storage device is connected \n");
            }
            #endif

            g_ioport.p_api->pinWrite(leds.p_leds[2], LED_ON);
            g_storage = instance;
        }
    }

    /* Check if some device is removed.  */
    else if((ULONG) UX_DEVICE_REMOVAL == event)
    {
        /* Check if we got an HID class device.  */
        if (instance == g_hid)
        {
            #ifdef SEMI_HOSTING
            if (CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk)
            {
                /* Debugger is connected */
                /* Call this before any calls to printf() */
                printf("HID device: Keyboard is disconnected \n");
            }
            #endif

            g_ioport.p_api->pinWrite(leds.p_leds[0], LED_OFF);
            g_hid = NULL;
        }
        else if (instance == g_storage)
        {
            #ifdef SEMI_HOSTING
            if (CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk)
            {
                /* Debugger is connected */
                /* Call this before any calls to printf() */
                printf("Storage device is disconnected \n");
            }
            #endif

            g_ioport.p_api->pinWrite(leds.p_leds[2], LED_OFF);
            g_storage = NULL;
        }
    }

    /* Allow other devices to be handled and always return UX_SUCCESS.
     * See ux_system_host_change_function in synegy_gen/common_data.c */
    return UX_SUCCESS;
}

/*---------------------------------------------------------------------------*
 * Function: write_key
 *---------------------------------------------------------------------------*/
/* The function opens the media, and crates key.txt file and writes the
 * received keypad values in the file and closes the media, sets up the
 * LED1 while accessing the media, and clears it after the operation.
 */
UINT write_key(uint8_t key)
{
    UINT status;

    /* Create the file */
    status = fx_file_create(&g_fx_media0, KEYBOARD_FILE_NAME);

    if (((UINT) FX_ALREADY_CREATED == status) || ((UINT) FX_SUCCESS == status))
    {
        g_ioport.p_api->pinWrite(leds.p_leds[1], LED_ON);

        /* Open the file */
        status = fx_file_open(&g_fx_media0, &g_file, KEYBOARD_FILE_NAME, FX_OPEN_FOR_WRITE);

        if ((UINT) FX_SUCCESS == status)
        {
            /* Move current position to the end of the file */
            status = fx_file_seek(&g_file, FILE_SEEK_END);

            if ((UINT) FX_SUCCESS == status)
            {
                /* Write the key to the file */
                status = fx_file_write(&g_file, &key, 1);

                if ((UINT) FX_SUCCESS == status)
                {
                    /* Close the file */
                    status = fx_file_close(&g_file);

                    if ((UINT) FX_SUCCESS == status)
                    {
                        /* Flush the data to the physical media */
                        status = fx_media_flush(&g_fx_media0);
                    }
                }
            }
        }
        else
        {
            #ifdef SEMI_HOSTING
            if (CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk)
            {
                /* Debugger is connected */
                /* Call this before any calls to printf() */
                printf("error while accessing the media \n");
            }
            #endif

            tx_thread_sleep(TX_WAIT_FOREVER);
        }

        g_ioport.p_api->pinWrite(leds.p_leds[1], LED_OFF);
    }

    return status;
}

/*---------------------------------------------------------------------------*
 * Function: usb_thread_entry
 *---------------------------------------------------------------------------*/
/* The application project main thread entry periodically gets the key from the USB keyboard and writes it
to the specified file on the USB disk; the instances of the mass storage and the keyboard devices are
initialized beforehand. The host event callback handles events such as device insertion or removal.
The initialization of mass storage is automatically performed in the generated code; the callback defined
in the main thread entry is responsible for HID class only. */
void usb_thread_entry(void)
{
    /* variable to capture LED errors */
    ssp_err_t err;
    UINT status;
    ULONG key;
    ULONG keyboard_state;

    #ifdef SEMI_HOSTING
    #ifdef __GNUC__
    if (CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk)
    {
        initialise_monitor_handles();
    }
    #endif
    #endif

    /* Get LED information for this board */
    err = R_BSP_LedsGet(&leds);

    if (SSP_SUCCESS != err)
    {
        /* This is a fairly simple error handling - it holds the
        application execution. In a more realistic scenarios
        a more robust and complex error handling solution should
        be provided. */
        #ifdef SEMI_HOSTING
        if (CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk)
        {
            /* Debugger is connected */
            /* Call this before any calls to printf() */
            printf("Could not get board LED info, error:%d\n",err);
        }
        #endif

        while (1)
        {
            ;
        }
    }

    /* LED off */
    for(uint32_t i = 0; i < leds.led_count; i++)
    {
        g_ioport.p_api->pinWrite(leds.p_leds[i], LED_OFF);
    }

    while (1)
    {
        /* Check if the HID device is already attached */
        if(NULL != g_hid)
        {
            /* Read pressed key from keyboard */
            status = ux_host_class_hid_keyboard_key_get((UX_HOST_CLASS_HID_KEYBOARD *)g_hid->ux_host_class_hid_client->ux_host_class_hid_client_local_instance,
                                                        &key, &keyboard_state);
            if (((UINT) UX_SUCCESS == status) && (FX_NULL != g_fx_media0_ptr))
            {
                /* Write the key to the file */
                uint8_t key_value = (uint8_t) key;
                write_key(key_value);
            }
        }

        tx_thread_sleep(1);
    }
}

