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
#include "SEGGER_RTT.h"
#include "application_define.h"

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

#define EVENTFLAG_USB_DEVICE_INSERTED   0x01

/* Define the number of buffers used in this demo. */
#define MAX_NUM_BUFFERS  2

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

/* video class instance */
UX_HOST_CLASS_VIDEO* volatile video_host_class;

/* video buffer */
UCHAR video_buffer[10*1024];

/* Name string of VS types */
struct
{
    int type;
    char* name;
} vs_type_name[] =
{
    { UX_HOST_CLASS_VIDEO_VS_UNDEFINED,             "UX_HOST_CLASS_VIDEO_VS_UNDEFINED"             },
    { UX_HOST_CLASS_VIDEO_VS_INPUT_HEADER,          "UX_HOST_CLASS_VIDEO_VS_INPUT_HEADER"          },
    { UX_HOST_CLASS_VIDEO_VS_OUTPUT_HEADER,         "UX_HOST_CLASS_VIDEO_VS_OUTPUT_HEADER"         },
    { UX_HOST_CLASS_VIDEO_VS_STILL_IMAGE_FRAME,     "UX_HOST_CLASS_VIDEO_VS_STILL_IMAGE_FRAME"     },
    { UX_HOST_CLASS_VIDEO_VS_FORMAT_UNCOMPRESSED,   "UX_HOST_CLASS_VIDEO_VS_FORMAT_UNCOMPRESSED"   },
    { UX_HOST_CLASS_VIDEO_VS_FRAME_UNCOMPRESSED,    "UX_HOST_CLASS_VIDEO_VS_FRAME_UNCOMPRESSED"    },
    { UX_HOST_CLASS_VIDEO_VS_FORMAT_MJPEG,          "UX_HOST_CLASS_VIDEO_VS_FORMAT_MJPEG"          },
    { UX_HOST_CLASS_VIDEO_VS_FRAME_MJPEG,           "UX_HOST_CLASS_VIDEO_VS_FRAME_MJPEG"           },
    { UX_HOST_CLASS_VIDEO_VS_FORMAT_MPEG2TS,        "UX_HOST_CLASS_VIDEO_VS_FORMAT_MPEG2TS"        },
    { UX_HOST_CLASS_VIDEO_VS_FORMAT_DV,             "UX_HOST_CLASS_VIDEO_VS_FORMAT_DV"             },
    { UX_HOST_CLASS_VIDEO_VS_COLORFORMAT,           "UX_HOST_CLASS_VIDEO_VS_COLORFORMAT"           },
    { UX_HOST_CLASS_VIDEO_VS_FORMAT_FRAME_BASED,    "UX_HOST_CLASS_VIDEO_VS_FORMAT_FRAME_BASED"    },
    { UX_HOST_CLASS_VIDEO_VS_FRAME_FRAME_BASED,     "UX_HOST_CLASS_VIDEO_VS_FRAME_FRAME_BASED"     },
    { UX_HOST_CLASS_VIDEO_VS_FORMAT_STREAM_BASED,   "UX_HOST_CLASS_VIDEO_VS_FORMAT_STREAM_BASED"   }
};

/** Global variables */
       FX_MEDIA                    * gp_media = FX_NULL;
static UX_HOST_CLASS_STORAGE       * gp_storage       = UX_NULL;
static UX_HOST_CLASS_STORAGE_MEDIA * gp_storage_media = UX_NULL;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
UINT write_key(uint8_t key);

VOID uvc_transfer_request_done_callback(UX_TRANSFER * transfer_request);

VOID uvc_parameter_interval_list(UX_HOST_CLASS_VIDEO *video);
UINT uvc_parameter_frame_list(UX_HOST_CLASS_VIDEO *video);
VOID uvc_parameter_list(UX_HOST_CLASS_VIDEO *video);

VOID uvc_process_function(UX_HOST_CLASS_VIDEO* video);

/*---------------------------------------------------------------------------*
 * Function: write_key
 *---------------------------------------------------------------------------*/
/*
  Handles USB Host event. p_host_class Pointer to type of USB Host Class structure of the device connected.
  (HID/Storage) and Pointer to instance representing the device.
 */
UINT ux_host_event_callback(ULONG event, UX_HOST_CLASS * host_class, VOID * instance)
{
    UX_HOST_CLASS_STORAGE_MEDIA		*p_ux_host_class_storage_media;
    UINT                     		status;

    UX_HOST_CLASS          			*p_storage_class = NULL;
    app_message_payload_t  			*p_message       = NULL;
    sf_message_acquire_cfg_t 		cfg_acquire =
    {
        .buffer_keep = false
    };
    sf_message_post_cfg_t    		cfg_post =
    {
        .priority = SF_MESSAGE_PRIORITY_NORMAL
    };

    switch(event)
    {
    	case UX_DEVICE_INSERTION:
    		SEGGER_RTT_printf(0, "UX_DEVICE_INSERTION %s, instance = 0x%x\n", host_class->ux_host_class_name, instance);
        	if (UX_SUCCESS == _ux_utility_memory_compare (_ux_system_host_class_hid_name, host_class->ux_host_class_name,
        	                                               _ux_utility_string_length_get(_ux_system_host_class_hid_name)))
        	{
	        	if (UX_SUCCESS == _ux_utility_memory_compare (_ux_system_host_class_hid_client_keyboard_name, ((UX_HOST_CLASS_HID *)instance)->ux_host_class_hid_client->ux_host_class_hid_client_name,
	        	                                               _ux_utility_string_length_get(_ux_system_host_class_hid_client_keyboard_name)))
	        	{
	        		SEGGER_RTT_printf(0, "HID keyboard is connected \n");
					g_ioport.p_api->pinWrite(leds.p_leds[0], LED_ON);
					g_hid = instance;
	        	}
	        	else if (UX_SUCCESS == _ux_utility_memory_compare (_ux_system_host_class_hid_client_mouse_name, ((UX_HOST_CLASS_HID *)instance)->ux_host_class_hid_client->ux_host_class_hid_client_name,
                        _ux_utility_string_length_get(_ux_system_host_class_hid_client_mouse_name)))
				{
	        		SEGGER_RTT_printf(0, "HID mouse is connected \n");
					g_ioport.p_api->pinWrite(leds.p_leds[0], LED_ON);
					g_hid = instance;
				}
	        	else if (UX_SUCCESS == _ux_utility_memory_compare (_ux_system_host_class_hid_client_remote_control_name, ((UX_HOST_CLASS_HID *)instance)->ux_host_class_hid_client->ux_host_class_hid_client_name,
                        _ux_utility_string_length_get(_ux_system_host_class_hid_client_remote_control_name)))
				{
	        		SEGGER_RTT_printf(0, "HID remote control is connected \n");
					g_ioport.p_api->pinWrite(leds.p_leds[0], LED_ON);
					g_hid = instance;
				}
	        	else
	        	{
	        		/* _ux_host_class_hid_client_search could not find a device from _ux_host_class_hid_activate */
	        		SEGGER_RTT_printf(0, "HID device \"%s\" is connected \n", ((UX_HOST_CLASS_HID *)instance)->ux_host_class_hid_client->ux_host_class_hid_client_name);
	        	}
        	}
            else if (UX_SUCCESS == _ux_utility_memory_compare (_ux_system_host_class_storage_name, host_class->ux_host_class_name,
                                                   _ux_utility_string_length_get(_ux_system_host_class_storage_name)))
            {
                // Get the pointer to the media
                ux_system_host_storage_fx_media_get (instance, &p_ux_host_class_storage_media, &g_fx_media0_ptr);

				SEGGER_RTT_printf(0, "Storage device is connected \n");

                g_ioport.p_api->pinWrite(leds.p_leds[2], LED_ON);
                g_storage = instance;

                /* Get the storage class.  */
                status = ux_host_stack_class_get(_ux_system_host_class_storage_name, &p_storage_class);
                if (UX_SUCCESS != status)
                {
                    return (status);
                }

                /* Check if we got a storage class device.  */
                if (p_storage_class == host_class)
                {
                    /* We only get the first media attached to the class container. */
                    if (FX_NULL == gp_media)
                    {
                        gp_storage       = instance;
                        gp_storage_media = host_class->ux_host_class_media;
                        gp_media         = &gp_storage_media->ux_host_class_storage_media;

                        /** Trigger app event */
                        status = g_sf_message.p_api->bufferAcquire(g_sf_message.p_ctrl,
                                                                   (sf_message_header_t **) &p_message,
                                                                   &cfg_acquire,
                                                                   TX_NO_WAIT);
                        APP_ERROR_TRAP(status);

                        p_message->header.event_b.class_code          = SF_MESSAGE_EVENT_CLASS_APP_CB;
                        p_message->header.event_b.class_instance = 0;
                        p_message->header.event_b.code           = SF_MESSAGE_EVENT_APP_CB_USB_IN;

                        status                                   = g_sf_message.p_api->post(g_sf_message.p_ctrl,
                                                                                            &p_message->header,
                                                                                            &cfg_post,
                                                                                            NULL,
                                                                                            TX_NO_WAIT);
                        APP_ERROR_TRAP(status);
                    }
                }
            }
            else if (UX_SUCCESS == _ux_utility_memory_compare (_ux_system_host_class_video_name, host_class->ux_host_class_name,
                                                   _ux_utility_string_length_get(_ux_system_host_class_video_name)))
            {
				video_host_class = instance;

				/* Set the event flag to let application know the device insertion. */
				tx_event_flags_set (&g_device_insert_eventflag, EVENTFLAG_USB_DEVICE_INSERTED, TX_OR);
            }
    		break;
    	case UX_DEVICE_REMOVAL:
    		SEGGER_RTT_printf(0, "UX_DEVICE_REMOVAL %s, instance = 0x%x\n", host_class->ux_host_class_name, instance);
            /* Check if we got an HID class device.  */
            if (instance == g_hid)
            {
				SEGGER_RTT_printf(0, "HID device is removed \n");

                g_ioport.p_api->pinWrite(leds.p_leds[0], LED_OFF);
                g_hid = NULL;
            }
            else if (instance == g_storage)
            {
				SEGGER_RTT_printf(0, "Storage device is removed \n");

                g_ioport.p_api->pinWrite(leds.p_leds[2], LED_OFF);
                g_storage = NULL;

                /*  Check if the storage device is removed.  */
                if (instance == gp_storage)
                {
                    /* Set pointers to null, so that the demo thread will not try to access the media any more.  */
                    gp_media         = FX_NULL;
                    gp_storage_media = UX_NULL;
                    gp_storage       = UX_NULL;

                    /** Trigger app event */
                    status = g_sf_message.p_api->bufferAcquire(g_sf_message.p_ctrl,
                                                               (sf_message_header_t **) &p_message,
                                                               &cfg_acquire,
                                                               TX_NO_WAIT);
                    APP_ERROR_TRAP(status);

                    p_message->header.event_b.class_code          = SF_MESSAGE_EVENT_CLASS_APP_CB;
                    p_message->header.event_b.class_instance = 0;
                    p_message->header.event_b.code           = SF_MESSAGE_EVENT_APP_CB_USB_OUT;

                    status                                   = g_sf_message.p_api->post(g_sf_message.p_ctrl,
                                                                                        &p_message->header,
                                                                                        &cfg_post,
                                                                                        NULL,
                                                                                        TX_NO_WAIT);
                    APP_ERROR_TRAP(status);
                }
            }
            else if (instance == video_host_class)
            {
				SEGGER_RTT_printf(0, "Video device is removed \n");

                /* Clear the event flag in case the camera was removed before the application could clear it. */
                tx_event_flags_set (&g_device_insert_eventflag, (ULONG)~EVENTFLAG_USB_DEVICE_INSERTED, TX_AND);

                video_host_class = NULL;
            }
    		break;
    	case UX_HID_CLIENT_INSERTION:
    		SEGGER_RTT_printf(0, "UX_HID_CLIENT_INSERTION %s, instance = 0x%x\n", host_class->ux_host_class_name, instance);
    		break;
    	case UX_HID_CLIENT_REMOVAL:
    		SEGGER_RTT_printf(0, "UX_HID_CLIENT_REMOVAL %s, instance = 0x%x\n", host_class->ux_host_class_name, instance);
    		break;
    	case UX_STORAGE_MEDIA_INSERTION:
    		SEGGER_RTT_printf(0, "UX_STORAGE_MEDIA_INSERTION %s, instance = 0x%x\n", host_class->ux_host_class_name, instance);
    		break;
    	case UX_STORAGE_MEDIA_REMOVAL:
    		SEGGER_RTT_printf(0, "UX_STORAGE_MEDIA_REMOVAL %s, instance = 0x%x\n", host_class->ux_host_class_name, instance);
    		break;
    	case UX_DEVICE_CONNECTION:
    		SEGGER_RTT_printf(0, "UX_DEVICE_CONNECTION, instance = 0x%x\n", instance);
    		break;
    	case UX_DEVICE_DISCONNECTION:
    		SEGGER_RTT_printf(0, "UX_DEVICE_DISCONNECTION, instance = 0x%x\n", instance);
            break;
    	default:
    		SEGGER_RTT_printf(0, "ux_host_event_callback, unexpected event %d for %s\n", host_class->ux_host_class_name, event);
    		break;
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
			SEGGER_RTT_printf(0, "error while accessing the media \n");

            tx_thread_sleep(TX_WAIT_FOREVER);
        }

        g_ioport.p_api->pinWrite(leds.p_leds[1], LED_OFF);
    }

    return status;
}

/* Video data received callback function. */
VOID uvc_transfer_request_done_callback(UX_TRANSFER * transfer_request)
{
    /* This is the callback function invoked by UVC class after a packet of
       data is received. */

    /* The actual number of bytes being received into the data buffer is
       recorded in tranfer_request -> ux_transfer_request_actual_length. */

    /* Since this callback function executes in the USB host controller
       thread, a semaphore is released so the application can pick up the
       video data in application thread. */

    SSP_PARAMETER_NOT_USED(transfer_request);

    tx_semaphore_put(&g_data_received_semaphore);
}


/* Show the interval types */
VOID uvc_parameter_interval_list(UX_HOST_CLASS_VIDEO *video)
{
    UX_HOST_CLASS_VIDEO_FRAME_DESCRIPTOR frame_descriptor;

    ULONG min_frame_interval;
    ULONG max_frame_interval;
    ULONG frame_interval_step;
    int i;

    /* Make the descriptor machine independent.  */
    _ux_utility_descriptor_parse(video -> ux_host_class_video_current_frame_address,
                                 _ux_system_class_video_frame_descriptor_structure,
                                 UX_HOST_CLASS_VIDEO_FRAME_DESCRIPTOR_ENTRIES, (UCHAR *) &frame_descriptor);

    /* Check the frame interval type.  */
    if (frame_descriptor.bFrameIntervalType == 0)
    {
        /* Frame interval type is continuous.  */
        min_frame_interval = _ux_utility_long_get(video -> ux_host_class_video_current_frame_address + 26);
        max_frame_interval = _ux_utility_long_get(video -> ux_host_class_video_current_frame_address + 30);
        frame_interval_step = _ux_utility_long_get(video -> ux_host_class_video_current_frame_address + 34);

        SSP_PARAMETER_NOT_USED(min_frame_interval);
        SSP_PARAMETER_NOT_USED(max_frame_interval);
        SSP_PARAMETER_NOT_USED(frame_interval_step);

		SEGGER_RTT_printf(0, "        interval min:%d, max:%d, step:%d\n",
				(int)min_frame_interval,
				(int)max_frame_interval,
				(int)frame_interval_step);
    }
    else
    {
		SEGGER_RTT_printf(0, "        interval ");

		/* Frame interval type is discrete.  */
		for(i = 0; i < (int)frame_descriptor.bFrameIntervalType; i++)
		{
			SEGGER_RTT_printf(0, "%d", (int)_ux_utility_long_get(video -> ux_host_class_video_current_frame_address + 26 + (unsigned int)i * sizeof(ULONG)));

			if((unsigned int)(i+1)<frame_descriptor.bFrameIntervalType)
			{
				SEGGER_RTT_printf(0, ", ");
			}
			else
			{
				SEGGER_RTT_printf(0, "\n");
			}
		}
    }
}


/* Show the frame resolutions */
UINT uvc_parameter_frame_list(UX_HOST_CLASS_VIDEO *video)
{
    ULONG frame_index;
    UX_HOST_CLASS_VIDEO_PARAMETER_FRAME_DATA frame_parameter;

    UINT status = UX_SUCCESS;

    /* frame resolutions */
	SEGGER_RTT_printf(0, "    --- frame resolutions\n");

    for (frame_index = 1; frame_index <= video -> ux_host_class_video_number_frames; frame_index++)
    {
        /* Get frame data for current frame index.  */
        frame_parameter.ux_host_class_video_parameter_frame_requested = frame_index;
        status = _ux_host_class_video_frame_data_get(video, &frame_parameter);
        if (status != UX_SUCCESS)
        {
            return(status);
        }

		/* Show the frame resolution  */
		SEGGER_RTT_printf(0, "    frame %d,%d\n",
				(int)frame_parameter.ux_host_class_video_parameter_frame_width,
				(int)frame_parameter.ux_host_class_video_parameter_frame_height);

        /* Save the current frame index.  */
        video -> ux_host_class_video_current_frame = frame_index;

        uvc_parameter_interval_list(video);
    }

	SEGGER_RTT_printf(0, "    --\n");

    return(status);
}


/* Show the device parameters */
VOID uvc_parameter_list(UX_HOST_CLASS_VIDEO *video)
{
    ULONG format_index;
    UX_HOST_CLASS_VIDEO_PARAMETER_FORMAT_DATA format_parameter;

    UINT status = UX_SUCCESS;
    int i;

    /* format types */
    SEGGER_RTT_printf(0, "\n--- format types\n");

    for (format_index = 1; format_index <= video -> ux_host_class_video_number_formats; format_index++)
    {
        /* Get format data for current format index.  */
        format_parameter.ux_host_class_video_parameter_format_requested = format_index;
        status = _ux_host_class_video_format_data_get(video, &format_parameter);
        if (status == UX_SUCCESS)
        {
			/* Show the format type  */
			char* type_name;

			type_name = "Unknow type name";
			for(i=0; (unsigned int)i<(sizeof(vs_type_name)/sizeof(vs_type_name[0])); i++)
			{
				if(format_parameter.ux_host_class_video_parameter_format_subtype==(ULONG)vs_type_name[i].type)
				{
					type_name = vs_type_name[i].name;
					break;
				}
			}

			SEGGER_RTT_printf(0, "format %s\n", type_name);

			/* Save number of frames in the video instance.  */
            video -> ux_host_class_video_number_frames = format_parameter.ux_host_class_video_parameter_number_frame_descriptors;

            uvc_parameter_frame_list(video);
        }
    }

    SEGGER_RTT_printf(0, "--\n");
}

VOID uvc_process_function(UX_HOST_CLASS_VIDEO* video)
{
    /* This demo uses two buffers. One buffer is used by video device while the
       application consumes data in the other buffer. */
    UCHAR *buffer_ptr[MAX_NUM_BUFFERS];

    /* Index variable keeping track of the current buffer being used by the video device. */
    ULONG buffer_index;

    /* Maximum buffer requirement reported by the video device. */
    ULONG max_buffer_size;

    UINT status;
    ULONG actual_flags;
    UINT frame_count;

    UX_HOST_CLASS_VIDEO_PARAMETER_CHANNEL channel;

    /* List parameters */
    uvc_parameter_list(video);

    /* Set video parameters. This setting value is a dummy.
       Depending on the application, set the necessary parameters. */
    status = ux_host_class_video_frame_parameters_set(video,
                UX_HOST_CLASS_VIDEO_VS_FORMAT_MJPEG,
                176, 144,
                333333);

    SEGGER_RTT_printf(0, "parameters set status = %d\n", status);

    /* Set the user callback function of video class. */
    ux_host_class_video_transfer_callback_set(video, uvc_transfer_request_done_callback);

    /* Find out the maximum memory buffer size for the video configuration
       set above. */
    max_buffer_size = ux_host_class_video_max_payload_get(video);

    SEGGER_RTT_printf(0, "max_buffer_size = %d\n", (int)max_buffer_size);

    /* Clear semaphore to zero */
    while (1)
    {
        if(tx_semaphore_get(&g_data_received_semaphore, 0)==TX_NO_INSTANCE)
        {
            break;
        }
    }

    SEGGER_RTT_printf(0, "Start video transfer.\n");

    /* Start video transfer.  */
    status = ux_host_class_video_start(video);
    if(status!=UX_SUCCESS)
    {
        /* Setting these to zero is a hack since we're mixing old and new APIs (new API does this and is required for reads). */
        video -> ux_host_class_video_transfer_request_start_index = 0;
        video -> ux_host_class_video_transfer_request_end_index = 0;

        channel.ux_host_class_video_parameter_format_requested = video -> ux_host_class_video_current_format;
        channel.ux_host_class_video_parameter_frame_requested = video -> ux_host_class_video_current_frame;
        channel.ux_host_class_video_parameter_frame_interval_requested = video -> ux_host_class_video_current_frame_interval;
        channel.ux_host_class_video_parameter_channel_bandwidth_selection = 1024;

        status = ux_host_class_video_ioctl(video, UX_HOST_CLASS_VIDEO_IOCTL_CHANNEL_START, &channel);
    }

    SEGGER_RTT_printf(0, "start status = %d\n", status);

    /* Allocate space for video buffer. */
    for(buffer_index = 0; buffer_index < MAX_NUM_BUFFERS; buffer_index++)
    {
        buffer_ptr[buffer_index] = &video_buffer[max_buffer_size * buffer_index];

        /* Add buffer to the video device for video streaming data. */
        ux_host_class_video_transfer_buffer_add(video, buffer_ptr[buffer_index]);
    }


    buffer_index = 0;
    frame_count = 0;

    while (1)
    {
        /* Suspend here until a transfer callback is called. */
        status = tx_semaphore_get(&g_data_received_semaphore, 100);
        if(status!=TX_SUCCESS)
        {
            /* Check camera status */
            status = tx_event_flags_get(&g_device_insert_eventflag, EVENTFLAG_USB_DEVICE_INSERTED, TX_OR, (ULONG *)&actual_flags, 0);
            if(status==TX_SUCCESS)
            {
                /* Stop video transfer.  */
                ux_host_class_video_stop(video);
            }
            break;
        }

        /* Received data. The callback function needs to obtain the actual
           number of bytes received, so the application routine can read the
           correct amount of data from the buffer. */

        /* Application can now consume video data while the video device stores
           the data into the other buffer. */

        /* Add the buffer back for video transfer. */
        ux_host_class_video_transfer_buffer_add(video, buffer_ptr[buffer_index]);

        /* Increment the buffer_index, and wrap to zero if it exceeds the
           maximum number of buffers. */
        buffer_index = (buffer_index + 1);
        if(buffer_index >= MAX_NUM_BUFFERS)
        {
            buffer_index = 0;
        }

        frame_count++;
    }

    SEGGER_RTT_printf(0, "Stop video transfer. frame_count = %d\n\n", frame_count);
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
    ULONG actual_flags;

    /* Get LED information for this board */
    err = R_BSP_LedsGet(&leds);

    if (SSP_SUCCESS != err)
    {
        /* This is a fairly simple error handling - it holds the
        application execution. In a more realistic scenarios
        a more robust and complex error handling solution should
        be provided. */

    	SEGGER_RTT_printf(0, "Could not get board LED info, error:%d\n",err);

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

    status = tx_thread_resume(&gui_thread);
    APP_ERROR_TRAP(status)

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

        /* Suspend here until a USBX Host Class Instance gets ready. */
        status = tx_event_flags_get(&g_device_insert_eventflag, EVENTFLAG_USB_DEVICE_INSERTED, TX_OR, (ULONG *)&actual_flags, TX_NO_WAIT);
        if(TX_SUCCESS == status)
        {
            /* This delay is required for now to get valid ISO IN UX_ENDPOINT instance. */
            tx_thread_sleep(100);

            if(video_host_class!=NULL)
            {
                uvc_process_function(video_host_class);
            }
        }

        tx_thread_sleep(1);
    }
}

