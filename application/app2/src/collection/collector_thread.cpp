#include "collector_thread.h"

#include "FXAS21002.h"
#include "FXOS8700.h"
#include "log.h"
#include "mbed.h"
#include "raw_sensor_data.h"

static RawSensorData result;

static void collect_sensor_data(FXOS8700 *acc, FXAS21002 *gyr)
{
    int size = 0;
    float tmp_acc[3];
    float tmp_gyr[3];

    log_info("Begin raw sensor data acquisition\n");

    // Collect raw sensor data until the data block is full
    while (size < FULL_WINDOW_SIZE)
    {
        acc->acquire_accel_data_g(tmp_acc);
        gyr->acquire_gyro_data_dps(tmp_gyr);

        result.ax[size] = tmp_acc[0];
        result.ay[size] = tmp_acc[1];
        result.az[size] = tmp_acc[2];
        result.gx[size] = tmp_gyr[0];
        result.gy[size] = tmp_gyr[1];
        result.gz[size] = tmp_gyr[2];

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

    for (int i = 0; i < 100; i++)
    {
        timer.reset();
        timer.start();

        collect_sensor_data(&acc, &gyr);
        timer.stop();

        log_info("Got %d samples in %lldms!\n",
                 FULL_WINDOW_SIZE,
                 chrono::duration_cast<chrono::milliseconds>(timer.elapsed_time()).count());
    }
}
