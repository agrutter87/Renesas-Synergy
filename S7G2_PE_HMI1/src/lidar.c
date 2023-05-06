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
ssp_err_t lidar_init(const sf_comms_instance_t * comms, lidar_cfg_t * lidar_cfg)
{
    ssp_err_t ssp_err = SSP_SUCCESS;

    uint8_t set_config_mode[8] = {0x42, 0x57, 0x02, 0x00, 0x00, 0x00, 0x01, 0x02};

    ssp_err = comms->p_api->write(comms->p_ctrl, set_config_mode, sizeof(set_config_mode), 100);
    uint8_t out_buffer[8] = {0x42, 0x57, 0x02, 0x00, 0x00, 0x00, lidar_cfg->output_data_format, 0x06};
    ssp_err = comms->p_api->write(comms->p_ctrl, out_buffer, sizeof(out_buffer), 100);

	/* Set output cycle */
    ssp_err = comms->p_api->write(comms->p_ctrl, set_config_mode, sizeof(set_config_mode), 100);
	out_buffer[4] = 100;
	out_buffer[5] = 0;
	out_buffer[6] = 0;
	out_buffer[7] = 0x07;
	ssp_err = comms->p_api->write(comms->p_ctrl, out_buffer, sizeof(out_buffer), 100);

    /* Set to single scan mode */
    ssp_err = comms->p_api->write(comms->p_ctrl, set_config_mode, sizeof(set_config_mode), 100);
	out_buffer[4] = 0;
	out_buffer[5] = 0;
	out_buffer[6] = 0;
	out_buffer[7] = 0x40;
	ssp_err = comms->p_api->write(comms->p_ctrl, out_buffer, sizeof(out_buffer), 100);

#if 0
    /* Set output data format */
   if(ssp_err == SSP_SUCCESS)
    {
		/* Set output cycle */
    	out_buffer[4] = ((lidar_cfg->output_data_cycle_10ms_units * 10U) & 0xFFU);
    	out_buffer[5] = (((lidar_cfg->output_data_cycle_10ms_units * 10U) >> 8) & 0xFFU);
    	out_buffer[6] = 0x00;
    	out_buffer[7] = 0x07;
		ssp_err = comms->p_api->write(comms->p_ctrl, out_buffer, sizeof(out_buffer), 100);
	    if(ssp_err == SSP_SUCCESS)
	    {
	    	/* Set output distance units */
	    	out_buffer[4] = 0x00;
	    	out_buffer[5] = 0x00;
	    	out_buffer[6] = lidar_cfg->output_distance_units;
	    	out_buffer[7] = 0x1A;
			ssp_err = comms->p_api->write(comms->p_ctrl, out_buffer, sizeof(out_buffer), 100);
		    if(ssp_err == SSP_SUCCESS)
		    {
		    	/* Set detection pattern */
		    	out_buffer[4] = 0x00;
		    	out_buffer[5] = 0x00;
		    	out_buffer[6] = lidar_cfg->detection_pattern;
		    	out_buffer[7] = 0x14;
				ssp_err = comms->p_api->write(comms->p_ctrl, out_buffer, sizeof(out_buffer), 100);
			    if(ssp_err == SSP_SUCCESS)
			    {
			    	/* Set distance mode */
			    	out_buffer[4] = 0x00;
			    	out_buffer[5] = 0x00;
			    	out_buffer[6] = lidar_cfg->distance_mode;
			    	out_buffer[7] = 0x11;
					ssp_err = comms->p_api->write(comms->p_ctrl, out_buffer, sizeof(out_buffer), 100);
				    if(ssp_err == SSP_SUCCESS)
				    {

				    }
			    }
		    }
	    }
    }
#endif
    return ssp_err;
}

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
ssp_err_t lidar_distance_read(const sf_comms_instance_t * comms, lidar_reading_t * reading)
{
    ssp_err_t 		ssp_err 		= SSP_SUCCESS;
    /* TODO: Review/Implement serial with driver only and queue instead of SF COMMS */
#if 0
    uint32_t 		numCharsRead 	= 0;
    uint8_t 		lastChar 		= 0x00;

    // Step 1: Read the serial stream until we see the beginning of the TF Mini header, or we timeout reading too many characters.
    while(1)
    {
    	uint8_t curChar;

        ssp_err = comms->p_api->read(comms->p_ctrl, &curChar, 1, TX_NO_WAIT);
        if(ssp_err == SSP_ERR_TIMEOUT)
        {
        	break;
        }
        else if(ssp_err == SSP_SUCCESS)
        {
            if ((lastChar == 0x59) && (curChar == 0x59))
            {
            	// Break to begin frame
            	break;
            }
            else
            {
            	// We have not seen two 0x59's in a row -- store the current character and continue reading.
            	lastChar = curChar;
            	numCharsRead += 1;
            }
        }

        // Error detection:  If we read more than X characters without finding a frame header, then it's likely there is an issue with
        // the Serial connection, and we should timeout and throw an error.
        if (numCharsRead > TFMINI_MAXBYTESBEFOREHEADER)
        {
        	reading->state = ERROR_SERIAL_NOHEADER;

        	if(TFMINI_DEBUGMODE == 1)
			{
        		debug_print("ERROR: no header");
			}

        	ssp_err = SSP_ERR_TIMEOUT;
        	break;
        }
    }

    if(ssp_err == SSP_SUCCESS)
    {
		// Step 2: Read one frame from the TFMini
		uint8_t frame[TFMINI_FRAME_SIZE];

		uint8_t checksum = 0x59 + 0x59;
		for (int i=0; i<TFMINI_FRAME_SIZE; i++)
		{
			// Read one character
			// wait for a character to become available
			ssp_err = comms->p_api->read(comms->p_ctrl, &frame[i], 1, TX_WAIT_FOREVER);
			if(ssp_err == SSP_SUCCESS)
			{
				// Store running checksum
				if (i < TFMINI_FRAME_SIZE-2)
				{
					checksum += frame[i];
				}
			}
			else
			{
				break;
			}
		}

		if(ssp_err == SSP_SUCCESS)
		{
			// Step 2A: Compare checksum
			// Last byte in the frame is an 8-bit checksum
			uint8_t checksumByte = frame[TFMINI_FRAME_SIZE-1];
			if (checksum != checksumByte)
			{
				reading->state = ERROR_SERIAL_BADCHECKSUM;
				if (TFMINI_DEBUGMODE == 1)
				{
					debug_print("ERROR: bad checksum");
				}
			}
			else
			{
				// Step 3: Interpret frame
				uint16_t dist = (frame[1] << 8) + frame[0];
				uint16_t st = (frame[3] << 8) + frame[2];
				uint8_t reserved = frame[4];
				uint8_t originalSignalQuality = frame[5];

				// Step 4: Store values
				reading->distance = dist;
				reading->strength = st;
				reading->state = MEASUREMENT_OK;
			}
		}
    }
#endif

    return ssp_err;
}
