/*-------------------------------------------------------------------------*
 * File:  lidar.h
 *-------------------------------------------------------------------------*
 * Description:
 */
 /**
 *  @file   lidar
 *  @brief  TODO: Short description
 *
 *  TODO: Description of file
 */
/*-------------------------------------------------------------------------*/
#ifndef LIDAR_H_
#define LIDAR_H_

/*--------------------------------------------------------------------------
 * Created by: Alex Grutter
 *--------------------------------------------------------------------------
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <lidar_thread.h>
#include <main_system.h>

#define TFMINI_DEBUGMODE  					1

// The frame size is nominally 9 characters, but we don't include the first two 0x59's marking the start of the frame
#define TFMINI_FRAME_SIZE                 7

// Timeouts
#define TFMINI_MAXBYTESBEFOREHEADER       30
#define TFMINI_MAX_MEASUREMENT_ATTEMPTS   10

// States
#define READY                             0
#define ERROR_SERIAL_NOHEADER             1
#define ERROR_SERIAL_BADCHECKSUM          2
#define ERROR_SERIAL_TOOMANYTRIES         3
#define MEASUREMENT_OK                    10

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
typedef enum e_lidar_mode_t
{
    LIDAR_MODE_SHORT_DISTANCE   = 0,
    LIDAR_MODE_MIDDLE_DISTANCE  = 3,
    LIDAR_MODE_LONG_DISTANCE    = 7
} lidar_mode_t;

typedef struct st_lidar_reading_t
{
	uint16_t	state;
    uint16_t    distance;
    uint16_t    strength;
} lidar_reading_t;

typedef enum e_lidar_output_data_format
{
	LIDAR_OUTPUT_DATA_FORMAT_STANDARD 	= 1,
	LIDAR_OUTPUT_DATA_FORMAT_PIXHAWK	= 4,
} lidar_output_data_format_t;

typedef enum e_lidar_output_data_units
{
	LIDAR_OUTPUT_DATA_UNITS_MM 	= 0,
	LIDAR_OUTPUT_DATA_UNITS_CM	= 1,
} lidar_output_distance_units_t;

typedef enum e_lidar_detection_pattern
{
	LIDAR_DETECTION_PATTERN_AUTO	= 0,
	LIDAR_DETECTION_PATTERN_FIX		= 1
} lidar_detection_pattern_t;

typedef enum e_lidar_distance_mode
{
	LIDAR_DISTANCE_MODE_0_to_2M		= 0,
	LIDAR_DISTANCE_MODE_0_to_5M		= 2,
	LIDAR_DISTANCE_MODE_0P5_to_5M	= 3,
	LIDAR_DISTANCE_MODE_1_to_12M	= 7
} lidar_distance_mode_t;

typedef struct st_lidar_cfg_t
{
	sf_comms_instance_t 			*p_comms;
	lidar_output_data_format_t		output_data_format;
	uint16_t						output_data_cycle_10ms_units;
	lidar_output_distance_units_t	output_distance_units;
	lidar_detection_pattern_t		detection_pattern;
	lidar_distance_mode_t			distance_mode;
} lidar_cfg_t;
/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
/** TODO: Description, one for each prototype
 *
 *  @param [in] val1        TODO: param1
 *  @param [in] val2        TODO: param1
 *  @return                 TODO: Return Value
 */
ssp_err_t lidar_init(const sf_comms_instance_t * comms, lidar_cfg_t * lidar_cfg);

/** TODO: Description, one for each prototype
 *
 *  @param [in] val1        TODO: param1
 *  @param [in] val2        TODO: param1
 *  @return                 TODO: Return Value
 */
ssp_err_t lidar_distance_read(const sf_comms_instance_t * comms, lidar_reading_t * reading);

#ifdef __cplusplus
}
#endif

#endif // LIDAR_H_
/*-------------------------------------------------------------------------*
 * End of File:  lidar.h
 *-------------------------------------------------------------------------*/
