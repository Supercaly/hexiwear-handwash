#include "collector_thread.h"
#include "config.h"
#include "global_thread_vars.h"
#include "log.h"

static void collect_sensor_data(FXOS8700 *acc, FXAS21002 *gyr)
{
    int size = 0;
    float tmp_acc[3];
    float tmp_gyr[3];

    log_info("\033[33mBegin raw sensor data acquisition\n\033[0m");

    // Collect raw sensor data until the data block is full
    while (size < RAW_SENSOR_DATA_BLOCK_CAP)
    {
        acc->acquire_accel_data_g(tmp_acc);
        gyr->acquire_gyro_data_dps(tmp_gyr);

        g_raw_sensor_data.ax[size] = tmp_acc[0];
        g_raw_sensor_data.ay[size] = tmp_acc[1];
        g_raw_sensor_data.az[size] = tmp_acc[2];
        g_raw_sensor_data.gx[size] = tmp_gyr[0];
        g_raw_sensor_data.gy[size] = tmp_gyr[1];
        g_raw_sensor_data.gz[size] = tmp_gyr[2];

        size++;

        // This parameter can be changed to adjust the timing;
        // at the moment it's set to have a sample rate of 100Hz.
        wait_ms(8);
    }

    log_info("\033[33mEnd raw sensor data acquisition\n\033[0m");
}

void collector_thread_loop()
{
    // TODO: Replace this timer with a global timing system
    Timer timer;

    FXOS8700 acc(PTC11, PTC10);
    FXAS21002 gyr(PTC11, PTC10);

    acc.accel_config();
    gyr.gyro_config();

    while (true)
    {
        timer.reset();
        timer.start();

        // Enter critical section
        g_sensors_lock.lock();

        collect_sensor_data(&acc, &gyr);
        timer.stop();

        g_sensors_lock.unlock();
        // Exit critical section

        log_info("Got %d samples in %dms!\n", RAW_SENSOR_DATA_BLOCK_CAP, timer.read_ms());
    }
}
