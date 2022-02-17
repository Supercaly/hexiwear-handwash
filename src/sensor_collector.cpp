#include "sensor_collector.h"
#include "log.h"

#include "mbed.h"

Sensor_Collector::Sensor_Collector() : _accelerometer(PTC11, PTC10),
                                       _gyroscope(PTC11, PTC10),
                                       _raw_data(new Sample_Matrix),
                                       _collected_size(0)
{
    _accelerometer.accel_config();
    _gyroscope.gyro_config();
}

Sensor_Collector::~Sensor_Collector()
{
    delete _raw_data;
}

void Sensor_Collector::collect()
{
    log_info("Begin sensor aquisition...\n");
    _collected_size = 0;
    float tmp_acc[3];
    float tmp_gyr[3];

    while (_collected_size < SAMPLE_MATRIX_ELEMENT_SIZE)
    {
        _accelerometer.acquire_accel_data_g(tmp_acc);
        _gyroscope.acquire_gyro_data_dps(tmp_gyr);

        _raw_data->ax[_collected_size] = tmp_acc[0];
        _raw_data->ay[_collected_size] = tmp_acc[1];
        _raw_data->az[_collected_size] = tmp_acc[2];
        _raw_data->gx[_collected_size] = tmp_gyr[0];
        _raw_data->gy[_collected_size] = tmp_gyr[1];
        _raw_data->gz[_collected_size] = tmp_gyr[2];

        _collected_size++;
        wait_ms(8);
    }
    log_info("End sensor aquisition\n");
}