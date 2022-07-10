#ifndef GLOBAL_THREAD_VARS_H_
#define GLOBAL_THREAD_VARS_H_

#include "common/config.h"
#include "common/data_exporter.h"
#include "common/data_recorder.h"

#include "label.h"
#include "mbed.h"
#include "raw_sensor_data.h"

extern Mutex g_sensors_lock;

extern RawSensorData g_raw_sensor_data;

extern Queue<Label, 5> g_labels_queue;

extern DataRecorder g_data_recorder;

extern DataExporter g_data_exporter;

extern Config g_config;

#endif // GLOBAL_THREAD_VARS_H_