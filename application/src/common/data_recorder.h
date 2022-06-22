#ifndef DATA_RECORDER_H_
#define DATA_RECORDER_H_

#include "data/raw_sensor_data.h"
#include "data/label.h"

#include "mbed.h"

class DataRecorder
{
public:
    DataRecorder();
    ~DataRecorder();

    // Start a recording session to a log file
    void start();

    // Stop an active recording session if there is one
    void stop();

    // Log given data to currently active recording session
    void record_data(Label label,
                     float *feat_data, size_t feat_sz,
                     RawSensorData *raw_data);

    // Returns true if there is an active recording session;
    // false otherwise
    bool is_recording() { return _recording; }

private:
    bool _recording;
    FILE *_out_file;

    bool open_log_file();
    bool close_log_file();
};

#endif // DATA_RECORDER_H_