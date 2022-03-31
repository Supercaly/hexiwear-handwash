#ifndef RAW_SENSOR_DATA_H_
#define RAW_SENSOR_DATA_H_

#include "common/config.h"

#include <string.h>

class RawSensorData
{
public:
    RawSensorData() {}
    ~RawSensorData() {}
    RawSensorData(const RawSensorData &other)
    {
        memcpy(ax, other.ax, RAW_SENSOR_DATA_BLOCK_CAP);
        memcpy(ay, other.ay, RAW_SENSOR_DATA_BLOCK_CAP);
        memcpy(az, other.az, RAW_SENSOR_DATA_BLOCK_CAP);
        memcpy(gx, other.gx, RAW_SENSOR_DATA_BLOCK_CAP);
        memcpy(gy, other.gy, RAW_SENSOR_DATA_BLOCK_CAP);
        memcpy(gz, other.gz, RAW_SENSOR_DATA_BLOCK_CAP);
    }

    float ax[RAW_SENSOR_DATA_BLOCK_CAP];
    float ay[RAW_SENSOR_DATA_BLOCK_CAP];
    float az[RAW_SENSOR_DATA_BLOCK_CAP];
    float gx[RAW_SENSOR_DATA_BLOCK_CAP];
    float gy[RAW_SENSOR_DATA_BLOCK_CAP];
    float gz[RAW_SENSOR_DATA_BLOCK_CAP];
};

#endif // RAW_SENSOR_DATA_H_