#include <main_system.h>
#include "lidar_thread.h"
#include "lidar.h"

static lidar_cfg_t g_lidar_cfg =
{
	.p_comms 						= NULL,
	.output_data_format 			= LIDAR_OUTPUT_DATA_FORMAT_STANDARD,
	.output_data_cycle_10ms_units 	= 100,
	.output_distance_units			= LIDAR_OUTPUT_DATA_UNITS_CM,
	.detection_pattern				= LIDAR_DETECTION_PATTERN_AUTO,
	.distance_mode					= LIDAR_DISTANCE_MODE_0_to_2M,
};

/* Store readings from LiDAR in this */
lidar_reading_t g_last_reading = {0};

/* LiDAR Thread entry function */
void lidar_thread_entry(void)
{
    /* SSP error variable used for tracking the status of Synergy API functions */
    ssp_err_t ssp_err = SSP_SUCCESS;

    /* ignore invalid ranging data */
    bool valid_data = false;

    /* Set flag to allow threads which need the IIC0 to run after common drivers are initialized */
    tx_event_flags_set (&g_main_system_event_flags, MAIN_SYSTEM_EVENT_SCI_UART7_ENABLED, TX_OR);
    debug_print ("\r\nMAIN_SYSTEM_EVENT_%s_ENABLED\r\n", "UART7");

    debug_print ("TFMini UART Test\r\n");

    uint8_t buffer[100] = {0};

    lidar_init(&g_sf_uart7, &g_lidar_cfg);

    while(1)
    {
        ssp_err = lidar_distance_read(&g_sf_uart7, &g_last_reading);
        if (ssp_err == SSP_SUCCESS)
        {
        	switch(g_last_reading.state)
        	{
				case READY:
					debug_print("Ready\r\n");
					break;
				case ERROR_SERIAL_BADCHECKSUM:
					debug_print("Bad checksum\r\n");
					break;
				case ERROR_SERIAL_NOHEADER:
					debug_print("No header\r\n");
					break;
				case ERROR_SERIAL_TOOMANYTRIES:
					debug_print("Too many tries\r\n");
					break;
				case MEASUREMENT_OK:
					debug_print("\tDist[%d]\tstrength[%d]\r\n", g_last_reading.distance, g_last_reading.strength);
					break;
				default:
					break;
        	}

        }
        else
        {
            debug_print("Read fail\r\n");
        }

        /* Delay 50 ms between readings */
        tx_thread_sleep(100);
    }
}

