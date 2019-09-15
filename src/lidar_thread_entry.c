#include <main_system.h>
#include "lidar_thread.h"
#include "lidar.h"

/* LiDAR Thread entry function */
void lidar_thread_entry(void)
{
    /* SSP error variable used for tracking the status of Synergy API functions */
    ssp_err_t ssp_err = SSP_SUCCESS;

    /* Store readings from LiDAR in this */
    lidar_reading_t reading = {0};

    /* ignore invalid ranging data */
    bool valid_data = false;

    /* Configure IIC0 */
    ssp_err = g_sf_i2c_device0.p_api->open (g_sf_i2c_device0.p_ctrl, g_sf_i2c_device0.p_cfg);
    if (ssp_err)
    {
        // TODO: Error handling
        debug_print ("\r\nError at accelerometer_thread_entry::g_i2c0.p_api->open\r\n");
    }

    /* Set flag to allow threads which need the IIC0 to run after common drivers are initialized */
    tx_event_flags_set (&g_main_system_event_flags, MAIN_SYSTEM_EVENT_IIC0_ENABLED, TX_OR);
    debug_print ("\r\nMAIN_SYSTEM_EVENT_%s_ENABLED\r\n", "IIC0");

    debug_print ("TFMini I2C Test\r\n");

    while(1)
    {
        ssp_err = readDistance(&g_sf_i2c_device0, &reading);
        if (ssp_err == SSP_SUCCESS)
        {
            if (valid_data == true)
            {
                debug_print("\tDist[%d]\tstrength[%d]\tmode[%d]\r\n",
                            reading.distance, reading.strength, reading.mode);
            }
            else
            {
                //don't print invalid data
            }
        }
        else
        {
            debug_print("Read fail\r\n");
        }

        /* Delay 50 ms between readings */
        tx_thread_sleep(5);
    }
}
