#include "sensors.h"

Sensors::Sensors() : _acc(PTC11, PTC10), _gyr(PTC11, PTC10)
{
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
    }
}

Sensor_Data Sensors::data[SENSORS_DATA_CAPACITY];