#ifndef GLOBAL_THREAD_VARS_H_
#define GLOBAL_THREAD_VARS_H_

#include "data/label.h"
#include "data/raw_sensor_data.h"
#include "common/data_recorder.h"
#include "common/data_exporter.h"

#include "mbed.h"

extern Mutex g_sensors_lock;

extern RawSensorData g_raw_sensor_data;

extern Queue<Label, 5> g_labels_queue;

extern DataRecorder g_data_recorder;

extern DataExporter g_data_exporter;

#endif // GLOBAL_THREAD_VARS_H_