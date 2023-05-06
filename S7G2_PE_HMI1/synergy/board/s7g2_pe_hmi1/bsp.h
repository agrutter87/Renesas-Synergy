/***********************************************************************************************************************
 * Copyright [2015-2021] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
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
/***********************************************************************************************************************
* File Name    : bsp.h
* Description  : Includes and API function available for this board.
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @ingroup BSP_Boards
 * @defgroup BSP_BOARD_S7G2HMI BSP for the PE-HMI1 Board
 * @brief BSP for the PE-HMI1 Board
 *
 * The PE-HMI1 is a development board for evaluating the Renesas SynergyTM S7G2 microcontroller in a Human-Machine
 * Interface (HMI) application. The board features a capacitive touch 7 inch WVGA TFTLCD graphics display and multiple
 * wired and wireless interfaces to allow rapid development of HMI applications such as thermostats, security panels,
 * or medical monitoring equipment. The PE-HMI1 kit with its TFTLCD display and communication features closely matches
 * the functionality and appearance of a real HMI end-product.
 *
 * @{
***********************************************************************************************************************/

#ifndef BSP_H_
#define BSP_H_

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @ingroup BSP_BOARD_S7G2HMI
 * @defgroup BSP_CONFIG_S7G2HMI Build Time Configurations
 *
 * The BSP has multiple header files that contain build-time configuration options. The header files
 * configure the following settings:
 *
 * - General BSP Options
 * - Clocks
 * - Interrupts
 * - Pin Configuration
 *
 * @{
 **********************************************************************************************************************/

/** @} (end defgroup BSP_CONFIG_S7G2HMI) */
/* BSP Board Specific Includes. */
#include "bsp_init.h"
#include "bsp_sdram.h"
#include "bsp_leds.h"
#include "bsp_ethernet_phy.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define BSP_BOARD_S7G2_PE_HMI1

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global functions (to be accessed by other files)
***********************************************************************************************************************/

/** @} (end defgroup BSP_BOARD_S7G2HMI) */

#endif /* BSP_H_ */
