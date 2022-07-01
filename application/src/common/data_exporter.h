#ifndef DATA_EXPORTER_H_
#define DATA_EXPORTER_H_

#include "mbed.h"
#include "BlockDevice.h"
#include "FATFileSystem.h"
#include "FileSystem.h"
#include "SDBlockDevice.h"

class DataExporter
{
public:
    DataExporter();
    ~DataExporter();

    bool export_data();

private:
    FileSystem
        *_internal_fs,
        *_sd_fs;
    BlockDevice
        *_internal_bd,
        *_sd_bd;
    FILE
        *_internal_f,
        *_sd_f;

    bool mount_fs(FileSystem *fs, BlockDevice *bd);
    bool open_file(const char *path, const char *mode, FILE **f);
    void format_data(FILE *file, void *data);
};

#endif // DATA_EXPORTER_H_