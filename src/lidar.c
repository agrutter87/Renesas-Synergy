/*-------------------------------------------------------------------------*
 * File:  lidar.c
 *-------------------------------------------------------------------------*
 * Description:
 */
/**
 *    @addtogroup lidar
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
#include <lidar.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Function: TODO: Function Function Name, one for each function
 *---------------------------------------------------------------------------*/
/** TODO: Description of Function
 *
 *  @param [in] val1        TODO: param1
 *  @param [in] val2        TODO: param1
 *  @return                 TODO: Return Value
 *  @par Example Code:
 *  @code
 *  //TODO: provide example or remove if not needed
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
ssp_err_t readDistance(const sf_i2c_instance_t * i2c, lidar_reading_t * reading)
{
    ssp_err_t ssp_err = SSP_SUCCESS;
    uint8_t out_buffer[3];
    uint8_t in_buffer[8] = {0};

    out_buffer[0] = 0x01;
    out_buffer[1] = 0x02;
    out_buffer[2] = 7;
    ssp_err = i2c->p_api->write(i2c->p_ctrl, out_buffer, 3, true, 100);
    if(ssp_err)
    {
        return ssp_err;
    }

#if 0
    Wire.requestFrom (deviceAddress, (uint8_t) 7); //Ask for 7 bytes

    if (Wire.available ())
    {
        for (uint8_t x = 0; x < 7; x++)
        {
            uint8_t incoming = Wire.read ();

            if (x == 0)
            {
                //Trigger done
                if (incoming == 0x00)
                {
                    //Serial.print("Data not valid: ");//for debugging
                    valid_data = false;
                    //return(false);
                }
                else if (incoming == 0x01)
                {
                    Serial.print ("Data valid:     ");
                    valid_data = true;
                }
            }
            else if (x == 2)
                distance = incoming; //LSB of the distance value "Dist_L"
            else if (x == 3)
                distance |= incoming << 8; //MSB of the distance value "Dist_H"
            else if (x == 4)
                strength = incoming; //LSB of signal strength value
            else if (x == 5)
                strength |= incoming << 8; //MSB of signal strength value
            else if (x == 6)
                rangeType = incoming; //range scale
        }
    }
    else
    {
        Serial.println ("No wire data avail");
        return (false);
    }
#else
    ssp_err = i2c->p_api->read(i2c->p_ctrl, in_buffer, 7, true, 100);
    if(ssp_err)
    {
        return ssp_err;
    }
    else
    {
        if(in_buffer[0] == 0)
        {
            return SSP_ERR_ABORTED;
        }
        else if (in_buffer[0] == 0x01)
        {
            debug_print("Data valid:     ");
        }

        reading->trigger_done = in_buffer[0];
        reading->zero         = in_buffer[1];
        reading->distance     = ((in_buffer[3] << 8) || in_buffer[2]);
        reading->strength     = ((in_buffer[5] << 8) || in_buffer[4]);
        reading->mode         = in_buffer[6];
    }
#endif
    return ssp_err;
}
