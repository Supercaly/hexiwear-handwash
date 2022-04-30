#include "data_recorder.h"
#include "common/log.h"

#include "FileSystem.h"

DataRecorder::DataRecorder() : _recording(false) {}

DataRecorder::~DataRecorder()
{
    close_log_file();
}

void DataRecorder::start()
{
    if (open_log_file())
    {
        log_info("Data Recorder: start recording!\n");
        _recording = true;
    }
}

void DataRecorder::stop()
{
    if (close_log_file())
    {
        log_info("Data Recorder: stop recording!\n");
        _recording = false;
    }
}

void DataRecorder::record_data()
{
    if (_recording && _out_file != NULL)
    {
        fprintf(_out_file, "new data\n");
        printf("write to logs\n");
    }
}

bool DataRecorder::open_log_file()
{
    FileSystem *fs = FileSystem::get_default_instance();
    if (fs->mount(BlockDevice::get_default_instance()) < 0)
    {
        log_error("Data Recorder: cannot mount filesystem for writing logs!\n");
        return false;
    }
    else
    {
        _out_file = fopen("/sd/raw_data.txt", "w+");
        if (_out_file == NULL)
        {
            log_error("Data Recorder: cannot open file log file\n");
        }
        return _out_file != NULL;
    }
}

bool DataRecorder::close_log_file()
{
    if (_out_file != NULL)
    {
        fclose(_out_file);
        FileSystem *fs = FileSystem::get_default_instance();
        fs->unmount();
        return true;
    }
    return false;
}