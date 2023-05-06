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
/**********************************************************************************************************************
* File Name    : bsp_sdram.c
* Description  : SDRAM initialization.
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @addtogroup BSP_S7G2HMI_INIT
 *
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "bsp_api.h"

#if defined(BSP_BOARD_S7G2_PE_HMI1)

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/*
 * Set ACTIVE-to-PRECHARGE command (tRAS) timing
 * e.g. tRAS = 42ns -> 6cycles are needed at SDCLK 120MHz
 *      tRAS = 37ns -> 5cycles are needed at SDCLK 120MHz
 */
#define BSP_PRV_SDRAM_TRAS                  (6U)

/*
 * Set ACTIVE-to-READ or WRITE delay tRCD (tRCD) timing
 * e.g. tRCD = 18ns -> 3cycles are needed at SDCLK 120MHz
 *      tRCD = 15ns -> 2cycles are needed at SDCLK 120MHz
 */
#define BSP_PRV_SDRAM_TRCD                  (3U)

/*
 * Set PRECHARGE command period (tRP) timing
 * e.g. tRP  = 18ns -> 3cycles are needed at SDCLK 120MHz
 *      tRP  = 15ns -> 2cycles are needed at SDCLK 120MHz
 */
#define BSP_PRV_SDRAM_TRP                   (3U)

/*
 * Set WRITE recovery time (tWR) timing
 * e.g. tWR  = 1CLK + 6ns -> 2cycles are needed at SDCLK 120MHz
 *      tWR  = 1CLK + 7ns -> 2cycles are needed at SDCLK 120MHz
 */
#define BSP_PRV_SDRAM_TWR                   (2U)

/*
 * Set CAS (READ) latency (CL) timing
 * e.g. CL = 18ns -> 3cycles are needed at SDCLK 120MHz
 * e.g. CL = 15ns -> 2cycles are needed at SDCLK 120MHz
 */
#define BSP_PRV_SDRAM_CL                    (3U)

/*
 * Set AUTO REFRESH period (tRFC) timing
 * e.g. tRFC = 60nS -> 8cycles are needed at SDCLK 120MHz
 *      tRFC = 66nS -> 8cycles are needed at SDCLK 120MHz
 */
#define BSP_PRV_SDRAM_TRFC                  (8U)

/*
 * Set Average Refresh period
 * e.g. tREF = 64ms/8192rows -> 7.8125us/each row.  937cycles are needed at SDCLK 120MHz
 */
#define BSP_PRV_SDRAM_REF_CMD_INTERVAL      (937U)

/*
 * Set Auto-Refresh issue times in initialization sequence needed for SDRAM device
 * Typical SDR SDRAM device needs twice of Auto-Refresh command issue
 */
#define BSP_PRV_SDRAM_SDIR_REF_TIMES        (2U)

/*
 * Set RAW address offset
 * Available settings are
 * 8  : 8-bit
 * 9  : 9-bit
 * 10 : 10-bit
 * 11 : 11-bit
 */
#define BSP_PRV_SDRAM_SDADR_ROW_ADDR_OFFSET (9U)

/*
 * Select endian mode for SDRAM address space
 * 0 : Endian of SDRAM address space is the same as the endian of operating mode
 * 1 : Endian of SDRAM address space is not the endian of operating mode
 */
#define BSP_PRV_SDRAM_ENDIAN_MODE           (0U)

/*
 * Select access mode
 * Typically Continuous access should be enabled to get better SDRAM bandwidth
 * 0: Continuous access is disabled
 * 1: Continuous access is enabled
 */
#define BSP_PRV_SDRAM_CONTINUOUS_ACCESSMODE (1U)

/*
 * Select bus width
 * 0: 16-bit
 * 2: 8-bit
 */
#define BSP_PRV_SDRAM_BUS_WIDTH             (0U)

#if ((BSP_PRV_SDRAM_SDADR_ROW_ADDR_OFFSET != 8) && (BSP_PRV_SDRAM_SDADR_ROW_ADDR_OFFSET != 9) \
  && (BSP_PRV_SDRAM_SDADR_ROW_ADDR_OFFSET != 10) && (BSP_PRV_SDRAM_SDADR_ROW_ADDR_OFFSET > 11))
#error "BSP_PRV_SDRAM_SDADR_ROW_ADDR_OFFSET must be either of 8,9,10 or 11"
#endif

#if ((BSP_PRV_SDRAM_BUS_WIDTH != 0) && (BSP_PRV_SDRAM_BUS_WIDTH != 2))
#error "BSP_PRV_SDRAM_BUS_WIDTH must be either of 0(16-bit) or 2(8-bit)"
#endif

#if ((BSP_PRV_SDRAM_ENDIAN_MODE != 0) && (BSP_PRV_SDRAM_ENDIAN_MODE != 1))
#error "BSP_PRV_SDRAM_ENDIAN_MODE must be either of 0(same endian as operating mode) or 2(another endian against operating mode)"
#endif

#if ((BSP_PRV_SDRAM_CONTINUOUS_ACCESSMODE != 0) && (BSP_PRV_SDRAM_CONTINUOUS_ACCESSMODE != 1))
#error "BSP_PRV_SDRAM_CONTINUOUS_ACCESSMODE must be either of 0(continuous access is disabled) or 1(continuous access is enabled)"
#endif

#define BSP_PRV_SDRAM_MR_WB_SINGLE_LOC_ACC  (1U)     /* MR.M9                : Single Location Access */
#define BSP_PRV_SDRAM_MR_OP_MODE            (0U)     /* MR.M8:M7             : Standard Operation */
#define BSP_PRV_SDRAM_MR_BT_SEQUENCTIAL     (0U)     /* MR.M3 Burst Type     : Sequential */
#define BSP_PRV_SDRAM_MR_BURST_LENGTH       (0U)     /* MR.M2:M0 Burst Length: 0(1 burst) */

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
 
/***********************************************************************************************************************
Private function prototypes
***********************************************************************************************************************/

/***********************************************************************************************************************
Private global variables
***********************************************************************************************************************/

/***********************************************************************************************************************
Functions
***********************************************************************************************************************/
/*******************************************************************************************************************//**
* @brief   Initializes SDRAM.
*
* This function initializes SDRAMC and SDR SDRAM device.
*
* @note This function is executed once after reset.
***********************************************************************************************************************/
void bsp_sdram_init (void)
{
    /** Delay at least 100uS after SDCLK active */
    R_BSP_SoftwareDelay(100U, BSP_DELAY_UNITS_MICROSECONDS);

    /** Setting for SDRAM initialization sequence */
#if (BSP_PRV_SDRAM_TRP < 3)
    R_BUS->SDIR_b.PRC  = 0U;
#else
    R_BUS->SDIR_b.PRC  = BSP_PRV_SDRAM_TRP - 3U;
#endif

    while(R_BUS->SDSR)
    {
        /* According to h/w maual, need to confirm that all the status bits in SDSR are 0 before SDIR modification. */
    }

    R_BUS->SDIR_b.ARFC = BSP_PRV_SDRAM_SDIR_REF_TIMES;

    while(R_BUS->SDSR)
    {
        /* According to h/w maual, need to confirm that all the status bits in SDSR are 0 before SDIR modification. */
    }

#if (BSP_PRV_SDRAM_TRFC < 3)
    R_BUS->SDIR_b.ARFI = 0U;
#else
    R_BUS->SDIR_b.ARFI = BSP_PRV_SDRAM_TRFC - 3U;
#endif

    while(R_BUS->SDSR)
    {
        /* According to h/w maual, need to confirm that all the status bits in SDSR are 0 before SDICR modification. */
    }

    /** Start SDRAM initialization sequence.
     * Following operation is automatically done when set SDICR.INIRQ bit.
     * Perform a PRECHARGE ALL command and wait at least tRP time.
     * Issue an AUTO REFRESH command and wait at least tRFC time.
     * Issue an AUTO REFRESH command and wait at least tRFC time.
     */
    R_BUS->SDICR_b.INIRQ = 1U;
    while(R_BUS->SDSR_b.INIST)
    {
        /* Wait the end of initialization sequence. */
    }

    /** Setting for SDRAM controller */
    R_BUS->SDCCR_b.BSIZE  = BSP_PRV_SDRAM_BUS_WIDTH;              /* set SDRAM bus width */
    R_BUS->SDAMOD_b.BE    = BSP_PRV_SDRAM_CONTINUOUS_ACCESSMODE;  /* enable continuous access */
    R_BUS->SDCMOD_b.EMODE = BSP_PRV_SDRAM_ENDIAN_MODE;            /* set endian mode for SDRAM address space */

    while(R_BUS->SDSR)
    {
        /* According to h/w maual, need to confirm that all the status bits in SDSR are 0 before SDMOD modification. */
    }

    /** Using LMR command, program the mode register */
    R_BUS->SDMOD = ((((BSP_PRV_SDRAM_MR_WB_SINGLE_LOC_ACC << 9)
                   |(BSP_PRV_SDRAM_MR_OP_MODE           << 7))
                   |(BSP_PRV_SDRAM_CL                   << 4))
                   |(BSP_PRV_SDRAM_MR_BT_SEQUENCTIAL    << 3))
                   |(BSP_PRV_SDRAM_MR_BURST_LENGTH      << 0);

    /** wait at least tMRD time */
    while(R_BUS->SDSR_b.MRSST)
    {
        /* Wait until Mode Register setting done. */
    }

    /** Set timing parameters for SDRAM */
    R_BUS->SDTR_b.RAS = BSP_PRV_SDRAM_TRAS - 1U;       /* set ACTIVE-to-PRECHARGE command cycles*/
    R_BUS->SDTR_b.RCD = BSP_PRV_SDRAM_TRCD - 1U;       /* set ACTIVE to READ/WRITE delay cycles */
    R_BUS->SDTR_b.RP  = BSP_PRV_SDRAM_TRP  - 1U;       /* set PRECHARGE command period cycles */
    R_BUS->SDTR_b.WR  = BSP_PRV_SDRAM_TWR  - 1U;       /* set write recovery cycles */
    R_BUS->SDTR_b.CL  = BSP_PRV_SDRAM_CL;             /* set SDRAM column latency cycles */

    /** Set row address offset for target SDRAM */
    R_BUS->SDADR_b.MXC   = BSP_PRV_SDRAM_SDADR_ROW_ADDR_OFFSET - 8U;

    R_BUS->SDRFCR_b.REFW = BSP_PRV_SDRAM_TRFC - 1U;                /* set Auto-Refresh issuing cycle */
    R_BUS->SDRFCR_b.RFC  = BSP_PRV_SDRAM_REF_CMD_INTERVAL - 1U;    /* set Auto-Refresh period */

    /** Start Auto-refresh */
    R_BUS->SDRFEN_b.RFEN = 1U;

    /** Enable SDRAM access */
    R_BUS->SDCCR_b.EXENB = 1U;
}

#endif

/** @} (end addtogroup BSP_S7G2HMI_INIT) */


