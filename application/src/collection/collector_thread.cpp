#include "collector_thread.h"
#include "common/config.h"
#include "common/global_thread_vars.h"
#include "common/log.h"

#include "FXAS21002.h"
#include "FXOS8700.h"

static void collect_sensor_data(FXOS8700 *acc, FXAS21002 *gyr)
{
    int size = 0;
    float tmp_acc[3];
    float tmp_gyr[3];

    log_info("Begin raw sensor data acquisition\n");

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
        ThisThread::sleep_for(8ms);
    }

    log_info("End raw sensor data acquisition\n");
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

        log_info("Got %d samples in %lldms!\n",
                 RAW_SENSOR_DATA_BLOCK_CAP,
                 chrono::duration_cast<chrono::milliseconds>(timer.elapsed_time()).count());
    }
}
