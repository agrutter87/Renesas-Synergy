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
GX_WINDOW_ROOT * p_window_root;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
#if 0
static UINT show_window(GX_WINDOW * p_new, GX_WIDGET * p_widget, bool detach_old);
static void update_text_id(GX_WIDGET * p_widget, GX_RESOURCE_ID id, UINT string_id);
#endif

/*---------------------------------------------------------------------------*
 * Function: init_window_handler
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
UINT init_window_handler(GX_WINDOW *widget, GX_EVENT *event_ptr)
{
    UINT gx_status = gx_window_event_process (widget, event_ptr);

    switch (event_ptr->gx_event_type)
    {
        case GX_EVENT_FOCUS_GAINED:
            break;
        case GX_EVENT_SHOW:
            break;
        default:
            gx_window_event_process (widget, event_ptr);
            break;
    }

    return gx_status;
}

#if 0
static UINT show_window(GX_WINDOW * p_new, GX_WIDGET * p_widget, bool detach_old)
{
    UINT gx_status = GX_SUCCESS;

    if (!p_new->gx_widget_parent)
    {
        gx_status = gx_widget_attach(p_window_root, p_new);
    }
    else
    {
        gx_status = gx_widget_show(p_new);
    }

    gx_system_focus_claim(p_new);

    GX_WIDGET * p_old = p_widget;
    if (p_old && detach_old)
    {
        if (p_old != (GX_WIDGET*) p_new)
        {
            gx_widget_detach(p_old);
        }
    }

    return gx_status;
}
#endif

#if 0
static void update_text_id(GX_WIDGET * p_widget, GX_RESOURCE_ID id, UINT string_id)
{
    GX_PROMPT * p_prompt = NULL;

    UINT gx_status = gx_widget_find(p_widget, (USHORT )id, GX_SEARCH_DEPTH_INFINITE, (GX_WIDGET** )&p_prompt);
    if (gx_status == GX_SUCCESS)
    {
        gx_prompt_text_id_set (p_prompt, string_id);
    }
}
#endif
