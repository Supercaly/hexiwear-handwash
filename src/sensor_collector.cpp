#include "sensor_collector.h"
#include "log.h"

#include "mbed.h"

Sensor_Collector::Sensor_Collector() : _accelerometer(PTC11, PTC10),
                                       _gyroscope(PTC11, PTC10)
{
    _accelerometer.accel_config();
    _gyroscope.gyro_config();

    _collected_size = 0;
}

Sensor_Collector::~Sensor_Collector()
{
}

void Sensor_Collector::collect()
{
    log_info("Begin sensor aquisition...\n");
    _collected_size = 0;
    while (_collected_size < SAMPLE_MATRIX_ELEMENT_SIZE)
    {
        _accelerometer.acquire_accel_data_g(_tmp_acc_data);
        _gyroscope.acquire_gyro_data_dps(_tmp_gyr_data);

        _raw_data._ax[_collected_size] = _tmp_acc_data[0];
        _raw_data._ay[_collected_size] = _tmp_acc_data[1];
        _raw_data._az[_collected_size] = _tmp_acc_data[2];
        _raw_data._gx[_collected_size] = _tmp_gyr_data[0];
        _raw_data._gy[_collected_size] = _tmp_gyr_data[1];
        _raw_data._gz[_collected_size] = _tmp_gyr_data[2];

        _collected_size++;
        wait_ms(8);
    }
    log_info("End sensor aquisition\n");
}