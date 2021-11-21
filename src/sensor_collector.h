#ifndef SENSOR_COLLECTOR_H
#define SENSOR_COLLECTOR_H

#include "sample_matrix.h"

#include "FXAS21002.h"
#include "FXOS8700.h"

class Sensor_Collector
{
public:
    Sensor_Collector();
    ~Sensor_Collector();

    void collect();

    const Sample_Matrix getSamples() { return _raw_data; }

private:
    FXOS8700 _accelerometer;
    FXAS21002 _gyroscope;

    float _tmp_acc_data[3];
    float _tmp_gyr_data[3];

    int _collected_size;
    Sample_Matrix _raw_data;
};

#endif // SENSOR_COLLECTOR_H