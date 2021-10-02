#include "sensors.h"

Sensors::Sensors() : _acc(PTC11, PTC10),
                     _gyr(PTC11, PTC10),
                     _log(USBTX, USBRX)
{
    _acc.accel_config();
    _gyr.gyro_config();
}

Sensors::~Sensors()
{
}

void Sensors::reset()
{
    data_size = 0;
}

void Sensors::acquire_data()
{
    while (data_size < SENSORS_DATA_CAPACITY)
    {
        float accel_data[3];
        float gyro_data[3];
        _acc.acquire_accel_data_g(accel_data);
        _gyr.acquire_gyro_data_dps(gyro_data);
        data[data_size] = SENSOR_DATA(accel_data, gyro_data);
        data_size++;
        wait_ms(8);

#ifdef LOG_REALTIME_SENSOR_DATA
        _log_sensor(data[data_size - 1]);
#endif // LOG_REALTIME_SENSOR_DATA
    }
}

Sensor_Data Sensors::data[SENSORS_DATA_CAPACITY];

void Sensors::_log_sensor(Sensor_Data d)
{
    _log.printf(SD_Arg "\n", SD_Fmt(d));
}