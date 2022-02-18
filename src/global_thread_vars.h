#ifndef GLOBAL_THREAD_VARS_H_
#define GLOBAL_THREAD_VARS_H_

#include "label.h"
#include "sensor_collector.h"
#include "mbed.h"

extern Mutex g_sensors_lock;

extern Sensor_Collector g_sens_collector;

extern Queue<Label, 5> g_labels_queue;

#endif // GLOBAL_THREAD_VARS_H_