#ifndef DATA_RECORDER_H_
#define DATA_RECORDER_H_

#include "mbed.h"

class DataRecorder
{
public:
    DataRecorder();
    ~DataRecorder();

    void start();
    void stop();

    void record_data();

    bool is_recording() { return _recording; }

private:
    bool _recording;
    FILE *_out_file;

    bool open_log_file();
    bool close_log_file();
};

#endif // DATA_RECORDER_H_