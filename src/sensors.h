#ifndef _SENSORS_H_
#define _SENSORS_H_

#include "mbed.h"
#include "FXAS21002.h"
#include "FXOS8700.h"

//#define LOG_REALTIME_SENSOR_DATA

struct Sensor_Data
{
    float ax, ay, az;
    float gx, gy, gz;
};

#define SENSOR_DATA(a, g)    \
    {                        \
        a[0], a[2], a[1],    \
            g[0], g[1], g[2] \
    }
#define SD_Arg "{ax: %f, ay: %f, az: %f, gx: %f, gy: %f, gz: %f}"
#define SD_Fmt(sd) sd.ax, sd.ay, sd.az, sd.gx, sd.gy, sd.gz

class Sensors
{
public:
    Sensors();
    ~Sensors();

    void reset();
    void acquire_data();

#define SENSORS_DATA_CAPACITY 1000
    static Sensor_Data data[SENSORS_DATA_CAPACITY];
    size_t data_size;

private:
    FXOS8700 _acc;
    FXAS21002 _gyr;
    Serial _log;
    void _log_sensor(Sensor_Data d);
};

#endif // _SENSORS_H_