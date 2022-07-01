#include "data_exporter.h"
#include "common/global_thread_vars.h"
#include "common/log.h"

DataExporter::DataExporter()
{
    _internal_bd = BlockDevice::get_default_instance();
    _internal_fs = FileSystem::get_default_instance();

    _sd_bd = new SDBlockDevice(PTE3, PTE1, PTE2, PTE4);
    _sd_fs = new FATFileSystem("sd");
}

DataExporter::~DataExporter() {}

bool DataExporter::export_data()
{
    if (g_data_recorder.is_recording())
    {
        log_error("Cannot export data while recording!\n");
        return false;
    }

    if (!mount_fs(_internal_fs, _internal_bd))
    {
        return false;
    }
    else
    {
        void *data_buffer;
        long data_size;

        if (!open_file("/fs/raw_data.txt", "rb", &_internal_f))
        {
            goto exit_with_error;
        }
        else
        {
            // Read content from internal memory
            fseek(_internal_f, 0L, SEEK_END);
            data_size = ftell(_internal_f);
            fseek(_internal_f, 0L, SEEK_SET);
            log_info("total internal file size: %ld bytes\n", data_size);

            data_buffer = malloc(data_size);
            if (data_buffer == NULL)
            {
                log_error("DataExporter: cannot allocate memory for coping data!\n");
                goto exit_with_error;
            }

            fread(data_buffer, 1, data_size, _internal_f);
            printf("log file: '");
            for (int i = 0; i < data_size; i++)
            {
                printf("%02X ", ((char *)data_buffer)[i]);
            }
            printf("'\n");

            // Close internal file and unmount internal filesystem
            fclose(_internal_f);
            _internal_fs->unmount();

            // Mount SD filesystem
            if (!mount_fs(_sd_fs, _sd_bd))
            {
                goto exit_with_error;
            }
            else
            {
                if (!open_file("/sd/raw_data.txt", "wb+", &_sd_f))
                {
                    goto exit_with_error;
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
                    _sd_fs->unmount();
                }
            }
        }
        free(data_buffer);
        return true;

    exit_with_error:
        // Unmount all filesystems and free allocated memory
        _internal_fs->unmount();
        _sd_fs->unmount();
        if (data_buffer != NULL)
        {
            free(data_buffer);
        }
        return false;
    }
}

bool DataExporter::mount_fs(FileSystem *fs, BlockDevice *bd)
{
    if (fs->mount(bd) < 0)
    {
        log_error("DataExporter: cannot mount filesystem!");
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
    fprintf(file, "  {\n");
    fprintf(file, "    \"label\": %d,\n", *((uint8_t *)data + 6));
    fprintf(file, "    \"features\": [");

    float *feats = (float *)((uint8_t *)data + 7);
    for (int i = 0; i < 25; i++)
    {
        if (i != 0)
        {
            fprintf(file, ",");
        }
        fprintf(file, "%f", feats[i]);
    }
    fprintf(file, "]\n");
    // TODO: Format the raw data when they'll be recorded

    fprintf(file, "  }");
}