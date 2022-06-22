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

void DataRecorder::record_data(Label label, float *feat_data, size_t feat_sz, RawSensorData *raw_data)
{
    if (_recording && _out_file != NULL)
    {
        // put header bytes
        fwrite("--bk--", sizeof(char), 6, _out_file);
        // put result label
        fwrite((uint8_t *)&label, sizeof(uint8_t), 1, _out_file);
        // put features
        fwrite(feat_data, sizeof(float), feat_sz, _out_file);
        // put raw data
        // fwrite(raw_data->ax, sizeof(float), RAW_SENSOR_DATA_BLOCK_CAP, _out_file);
        // fwrite(raw_data->ay, sizeof(float), RAW_SENSOR_DATA_BLOCK_CAP, _out_file);
        // fwrite(raw_data->az, sizeof(float), RAW_SENSOR_DATA_BLOCK_CAP, _out_file);
        // fwrite(raw_data->gx, sizeof(float), RAW_SENSOR_DATA_BLOCK_CAP, _out_file);
        // fwrite(raw_data->gy, sizeof(float), RAW_SENSOR_DATA_BLOCK_CAP, _out_file);
        // fwrite(raw_data->gz, sizeof(float), RAW_SENSOR_DATA_BLOCK_CAP, _out_file);
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
        _out_file = fopen("/fs/raw_data.txt", "wb+");
        if (_out_file == NULL)
        {
            log_error("Data Recorder: cannot open log file %s(%d)\n", strerror(errno), errno);
            fs->unmount();
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