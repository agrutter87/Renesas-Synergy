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
#include <marsgro_system.h>

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
    uint8_t     trigger_done;
    uint8_t     zero;
    uint16_t    distance;
    uint16_t    strength;
    uint8_t     mode;
} lidar_reading_t;

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
ssp_err_t readDistance(const sf_i2c_instance_t * i2c, lidar_reading_t * reading);

#ifdef __cplusplus
}
#endif

#endif // LIDAR_H_
/*-------------------------------------------------------------------------*
 * End of File:  lidar.h
 *-------------------------------------------------------------------------*/
