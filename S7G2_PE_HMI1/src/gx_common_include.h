/*-------------------------------------------------------------------------*
 * File:  gx_common_includes.h
 *-------------------------------------------------------------------------*/
/**
 *  @file   gx_common_includes.h
 *  @brief  Contains includes to the generated files, whose name can change.
 */
/*--------------------------------------------------------------------------
 * Copyright (C) 2007-2017 Future Designs, Inc.
 *-------------------------------------------------------------------------*/
#ifndef GX_COMMON_INCLUDES_H_
#define GX_COMMON_INCLUDES_H_

/***********************************************************************************************************************
 * Includes
 ***********************************************************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

// Replace these with the actual filenames of the generated application code
#include "gui_thread.h"
#include "main_system.h"
#include "gui.h"
#include "application_define.h"

#include "guix_gen/audio_player_resources.h"
#include "guix_gen/audio_player_specifications.h"

// Replace with name of widget table (Currently always *_widget_table)
extern GX_CONST GX_STUDIO_WIDGET_ENTRY audio_player_widget_table[];
#define GUIX_widget_table audio_player_widget_table


/***********************************************************************************************************************
 * Macro definitions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Typedef definitions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function prototypes
 ***********************************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // GX_COMMON_INCLUDES_H_
/*-------------------------------------------------------------------------*
 * End of File:  gx_common_includes.h
 *-------------------------------------------------------------------------*/
