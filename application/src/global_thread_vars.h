#ifndef GLOBAL_THREAD_VARS_H_
#define GLOBAL_THREAD_VARS_H_

#include "label.h"
#include "mbed.h"
#include "raw_sensor_data.h"

extern Mutex g_sensors_lock;

extern RawSensorData g_raw_sensor_data;

extern Queue<Label, 5> g_labels_queue;

#endif // GLOBAL_THREAD_VARS_H_