#include "data_exporter.h"
#include "common/global_thread_vars.h"

#include "log.h"

DataExporter::DataExporter() : _export_success(false),
                               _data_buffer(nullptr),
                               _total_size(0L)
{
    _internal_bd = BlockDevice::get_default_instance();
    _internal_fs = FileSystem::get_default_instance();

    _sd_bd = new SDBlockDevice(PTE3, PTE1, PTE2, PTE4);
    _sd_fs = new FATFileSystem("sd");
}

DataExporter::~DataExporter() {}

bool DataExporter::export_data()
{
    _export_thread = new Thread();
    _export_thread->start(callback(this, &DataExporter::export_thread));
    _export_thread->join();
    delete _export_thread;

    return _export_success;
}

bool DataExporter::mount_fs(FileSystem *fs, BlockDevice *bd)
{
    if (fs->mount(bd) < 0)
    {
        log_error("DataExporter: cannot mount filesystem!\n");
        return false;
    }
    return true;
}

bool DataExporter::open_file(const char *path, const char *mode, FILE **f)
{
    *f = fopen(path, mode);
    if (*f == NULL)
    {
        log_error("DataExporter: cannot open file '%s' (%s)\n", path, strerror(errno));
        return false;
    }
    return true;
}

void DataExporter::format_data(FILE *file, void *data)
{
    int offset = 6 * sizeof(char);

    fprintf(file, "  {\n");
    fprintf(file, "    \"label\": %d,\n", *((uint8_t *)data + offset));

    // Format the features
    fprintf(file, "    \"features\": ");
    offset += 1 * sizeof(uint8_t);
    float *feats = (float *)((uint8_t *)data + offset);
    // TODO: Replace this number with a macro for features size
    format_farray(file, feats, 25);
    fprintf(file, ",\n");

    // Format raw data by column
    fprintf(file, "    \"raw\": {\n");
    offset += 25 * sizeof(float);
    // ax
    fprintf(file, "      \"ax\": ");
    float *ax = (float *)((uint8_t *)data + offset);
    format_farray(file, ax, RAW_SENSOR_DATA_BLOCK_CAP);
    fprintf(file, ",\n");
    // ay
    offset += RAW_SENSOR_DATA_BLOCK_CAP * sizeof(float);
    fprintf(file, "      \"ay\": ");
    float *ay = (float *)((uint8_t *)data + offset);
    format_farray(file, ay, RAW_SENSOR_DATA_BLOCK_CAP);
    fprintf(file, ",\n");
    // az
    offset += RAW_SENSOR_DATA_BLOCK_CAP * sizeof(float);
    fprintf(file, "      \"az\": ");
    float *az = (float *)((uint8_t *)data + offset);
    format_farray(file, az, RAW_SENSOR_DATA_BLOCK_CAP);
    fprintf(file, ",\n");
    // gx
    offset += RAW_SENSOR_DATA_BLOCK_CAP * sizeof(float);
    fprintf(file, "      \"gx\": ");
    float *gx = (float *)((uint8_t *)data + offset);
    format_farray(file, gx, RAW_SENSOR_DATA_BLOCK_CAP);
    fprintf(file, ",\n");
    // gy
    offset += RAW_SENSOR_DATA_BLOCK_CAP * sizeof(float);
    fprintf(file, "      \"gy\": ");
    float *gy = (float *)((uint8_t *)data + offset);
    format_farray(file, gy, RAW_SENSOR_DATA_BLOCK_CAP);
    fprintf(file, ",\n");
    // gz
    offset += RAW_SENSOR_DATA_BLOCK_CAP * sizeof(float);
    fprintf(file, "      \"gz\": ");
    float *gz = (float *)((uint8_t *)data + offset);
    format_farray(file, gz, RAW_SENSOR_DATA_BLOCK_CAP);
    fprintf(file, "\n");
    fprintf(file, "    }\n");

    fprintf(file, "  }");
}

void DataExporter::format_farray(FILE *file, float *a, int size)
{
    fprintf(file, "[");
    for (int i = 0; i < size; i++)
    {
        if (i != 0)
        {
            fprintf(file, ",");
        }
        fprintf(file, "%f", a[i]);
    }
    fprintf(file, "]");
}

void DataExporter::export_thread()
{
    _export_success = false;

    if (g_data_recorder.is_recording())
    {
        log_error("Cannot export data while recording!\n");
        return;
    }

    if (!mount_fs(_internal_fs, _internal_bd))
    {
        return;
    }
    else
    {
        void *data_buffer = nullptr;
        long data_size;

        if (!open_file("/fs/raw_data.txt", "rb", &_internal_f))
        {
            goto clean_and_exit;
        }
        else
        {
            // Read content from internal memory
            fseek(_internal_f, 0L, SEEK_END);
            data_size = ftell(_internal_f);
            fseek(_internal_f, 0L, SEEK_SET);
            log_info("DataExporter: total file size: %ld bytes\n", data_size);

            data_buffer = malloc(data_size);
            if (data_buffer == NULL)
            {
                log_error("DataExporter: cannot allocate memory for coping data!\n");
                goto clean_and_exit;
            }

            fread(data_buffer, 1, data_size, _internal_f);
            
            // Close internal file and unmount internal filesystem
            fclose(_internal_f);
            _internal_f = nullptr;
            _internal_fs->unmount();

            // Mount SD filesystem
            if (!mount_fs(_sd_fs, _sd_bd))
            {
                goto clean_and_exit;
            }
            else
            {
                if (!open_file("/sd/raw_data.txt", "wb+", &_sd_f))
                {
                    goto clean_and_exit;
                }
                else
                {
                    // Format and write the data to the SD card
                    fprintf(_sd_f, "{\n");
                    for (int i = 0; i < data_size; i += 107)
                    {
                        format_data(_sd_f, (uint8_t *)data_buffer + i);
                        if (i != data_size)
                        {
                            fprintf(_sd_f, ",\n");
                        }
                        else
                        {
                            fprintf(_sd_f, "\n");
                        }
                    }
                    fprintf(_sd_f, "}");

                    log_info("File exported\n");

                    fclose(_sd_f);
                    _sd_f = nullptr;
                    _sd_fs->unmount();
                }
            }
        }
        _export_success = true;

    clean_and_exit:
        // Unmount all filesystems and free allocated memory
        if (_internal_f != nullptr)
        {
            fclose(_internal_f);
            _internal_f = nullptr;
        }
        if (_sd_f != nullptr)
        {
            fclose(_sd_f);
            _sd_f = nullptr;
        }
        _internal_fs->unmount();
        _sd_fs->unmount();
        if (data_buffer != NULL)
        {
            free(data_buffer);
        }
        return;
    }
}