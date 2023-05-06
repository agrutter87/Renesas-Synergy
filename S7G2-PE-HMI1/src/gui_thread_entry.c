/***********************************************************************************************************************
* Copyright [2016] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
* 
 * This file is part of Renesas SynergyTM Software Package (SSP)
*
* The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
* and/or its licensors ("Renesas") and subject to statutory and contractual protections.
*
* This file is subject to a Renesas SSP license agreement. Unless otherwise agreed in an SSP license agreement with
* Renesas: 1) you may not use, copy, modify, distribute, display, or perform the contents; 2) you may not use any name
* or mark of Renesas for advertising or publicity purposes or in connection with your use of the contents; 3) RENESAS
* MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED
* "AS IS" WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
* PARTICULAR PURPOSE, AND NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR
* CONSEQUENTIAL DAMAGES, INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF
* CONTRACT OR TORT, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents
* included in this file may be subject to different terms.
**********************************************************************************************************************/

#ifdef BSP_BOARD_S7G2_SK
#include "lcd_setup/lcd_setup.h"
#endif
#ifdef BSP_BOARD_S5D9_PK
#include "lcd_setup/lcd_setup.h"
#endif
#include "gx_common_include.h"

void gui_thread_entry (void);

static GX_EVENT     g_gx_event      = { 0 };

/* GUI Thread entry function */
void gui_thread_entry (void)
{
    UINT                    status;
    app_message_payload_t * p_message = NULL;

    guix_init(); // start the gui including GUIX initialization and hardware enable/power-on

    while (1)
    {
        status = g_sf_message.p_api->pend(g_sf_message.p_ctrl,
                                          &gui_thread_message_queue,
                                          (sf_message_header_t **) &p_message,
                                          TX_WAIT_FOREVER);
        if (!status && (p_message->header.event_b.class_code == SF_MESSAGE_EVENT_CLASS_APP_CB))
        {
            GX_EVENT gxe =
            {
                .gx_event_type           = GX_NULL,
                .gx_event_sender         = GX_ID_NONE,
                .gx_event_target         = GX_ID_NONE,
                .gx_event_display_handle = GX_ID_NONE
            };

            switch (p_message->header.event_b.code)
            {
                case SF_MESSAGE_EVENT_APP_CB_STATUS:
                {
                    gxe.gx_event_type = APP_EVENT_STATUS_CHANGED;

                    break;
                }

                case SF_MESSAGE_EVENT_APP_CB_VOLUME:
                {
                    gxe.gx_event_type = APP_EVENT_VOLUME_CHANGED;

                    break;
                }

                case SF_MESSAGE_EVENT_APP_CB_USB_IN:
                {
                    gxe.gx_event_type = APP_EVENT_USB_INSERTED;

                    break;
                }

                case SF_MESSAGE_EVENT_APP_CB_USB_OUT:
                {
                    gxe.gx_event_type = APP_EVENT_USB_REMOVED;

                    break;
                }

                case SF_MESSAGE_EVENT_APP_ERR_OPEN:
                case SF_MESSAGE_EVENT_APP_ERR_PAUSE:
                case SF_MESSAGE_EVENT_APP_ERR_RESUME:
                case SF_MESSAGE_EVENT_APP_ERR_CLOSE:
                case SF_MESSAGE_EVENT_APP_ERR_VOLUME:
                case SF_MESSAGE_EVENT_APP_ERR_PLAYBACK:
                {
                    gxe.gx_event_type                       = APP_EVENT_ERROR;
                    gxe.gx_event_payload.gx_event_ulongdata = p_message->header.event_b.code;

                    break;
                }

                case SF_MESSAGE_EVENT_APP_ERR_HEADER:
                {
                    gxe.gx_event_type                       = APP_EVENT_ERROR;
                    gxe.gx_event_payload.gx_event_ulongdata = p_message->data.error_code;

                    break;
                }
            }

            if (gxe.gx_event_type != GX_NULL)
            {
                status = gx_system_event_send(&gxe);
                APP_ERROR_TRAP(status)
            }
        }

        status = g_sf_message.p_api->bufferRelease(g_sf_message.p_ctrl,
                                                   (sf_message_header_t *) p_message,
                                                   SF_MESSAGE_RELEASE_OPTION_ACK);
        APP_ERROR_TRAP(status);
    }
}

void touch_callback(sf_touchpanel_v2_callback_args_t *p_args)
{
    UINT        gx_status       = GX_SUCCESS;
    bool        send_event      = true;
    GX_EVENT    *p_gx_event     = &g_gx_event;
    sf_touch_panel_v2_payload_t const * const p_payload = &p_args->payload;

    switch (p_payload->event_type)
    {
        /* New touch event reported. */
        case SF_TOUCH_PANEL_V2_EVENT_DOWN:
            p_gx_event->gx_event_type = GX_EVENT_PEN_DOWN;
            break;

        /* Touch released. */
        case SF_TOUCH_PANEL_V2_EVENT_UP:
            p_gx_event->gx_event_type = GX_EVENT_PEN_UP;
            break;

        /* Touch has not moved since last touch event. */
        /* Touch has moved since last touch event. */
        case SF_TOUCH_PANEL_V2_EVENT_HOLD:
        case SF_TOUCH_PANEL_V2_EVENT_MOVE:
            p_gx_event->gx_event_type = GX_EVENT_PEN_DRAG;
            break;


        /* No valid touch event happened. */
        /* Invalid touch data */
        case SF_TOUCH_PANEL_V2_EVENT_NONE:
        case SF_TOUCH_PANEL_V2_EVENT_INVALID:
        default:
            send_event = false;
            break;
    }

    if (send_event)
    {
        /** Send event to GUI */
        p_gx_event->gx_event_sender = GX_ID_NONE;
        p_gx_event->gx_event_target = 0;
        p_gx_event->gx_event_display_handle = 0;

        p_gx_event->gx_event_payload.gx_event_pointdata.gx_point_x = p_payload->x;

#if defined(BSP_BOARD_S7G2_SK)||defined(BSP_BOARD_S5D9_PK)
        p_gx_event->gx_event_payload.gx_event_pointdata.gx_point_y = (GX_VALUE)(320 - p_payload->y);  // SK-S7G2
#else
        p_gx_event->gx_event_payload.gx_event_pointdata.gx_point_y = p_payload->y;  // DK-S7G2, PE-HMI1
#endif
        gx_status = gx_system_event_send(p_gx_event);
        /* Confirm that the event flags were set */
        if (GX_SUCCESS != gx_status)
        {

        }
    }
}

#ifndef BSP_BOARD_S7G2_PE_HMI1 /* Pushbuttons are only present on the SK/DK-S7G2 boards */

void g_btn_down_callback (external_irq_callback_args_t * p_args)
{
    SSP_PARAMETER_NOT_USED(p_args);

    app_message_payload_t  * p_message = NULL;
    sf_message_acquire_cfg_t cfg_acquire =
    {
        .buffer_keep = false
    };
    sf_message_post_cfg_t    cfg_post    =
    {
        .priority = SF_MESSAGE_PRIORITY_NORMAL
    };

    g_sf_message.p_api->bufferAcquire(g_sf_message.p_ctrl, (sf_message_header_t **) &p_message, &cfg_acquire,
                                      TX_NO_WAIT);

    p_message->header.event_b.class_code          = SF_MESSAGE_EVENT_CLASS_APP_CMD;
    p_message->header.event_b.class_instance = 0;
    p_message->header.event_b.code           = SF_MESSAGE_EVENT_APP_VOL_DOWN;
    p_message->data.vol_change               = 15;

    g_sf_message.p_api->post(g_sf_message.p_ctrl, &p_message->header, &cfg_post, NULL, TX_NO_WAIT);
}

void g_btn_up_callback (external_irq_callback_args_t * p_args)
{
    SSP_PARAMETER_NOT_USED(p_args);

    app_message_payload_t  * p_message = NULL;
    sf_message_acquire_cfg_t cfg_acquire =
    {
        .buffer_keep = false
    };
    sf_message_post_cfg_t    cfg_post    =
    {
        .priority = SF_MESSAGE_PRIORITY_NORMAL
    };

    g_sf_message.p_api->bufferAcquire(g_sf_message.p_ctrl, (sf_message_header_t **) &p_message, &cfg_acquire,
                                      TX_NO_WAIT);

    p_message->header.event_b.class_code          = SF_MESSAGE_EVENT_CLASS_APP_CMD;
    p_message->header.event_b.class_instance = 0;
    p_message->header.event_b.code           = SF_MESSAGE_EVENT_APP_VOL_UP;
    p_message->data.vol_change               = 15;

    g_sf_message.p_api->post(g_sf_message.p_ctrl, &p_message->header, &cfg_post, NULL, TX_NO_WAIT);
}

#endif
