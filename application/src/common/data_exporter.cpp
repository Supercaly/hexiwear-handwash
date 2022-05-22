#include "data_exporter.h"
#include "common/global_thread_vars.h"
#include "common/log.h"

#include "FATFileSystem.h"
#include "FileSystem.h"
#include "SDBlockDevice.h"

DataExporter::DataExporter() {}

DataExporter::~DataExporter() {}

bool DataExporter::export_data()
{
    if (g_data_recorder.is_recording())
    {
        log_error("Cannot export data while recording!\n");
        return false;
    }

    FileSystem *fs = FileSystem::get_default_instance();
    if (fs->mount(BlockDevice::get_default_instance()) < 0)
    {
        log_error("Cannot mount filesystem for reading logs!\n");
        return false;
    }
    else
    {
        FILE *f = fopen("/fs/raw_data.txt", "rb");
        if (f == NULL)
        {
            log_error("Cannot open log file %s(%d)\n", strerror(errno), errno);
            fs->unmount();
            return false;
        }
        else
        {
            void *copy_buff;

            fseek(f, 0L, SEEK_END);
            long sz = ftell(f);
            fseek(f, 0L, SEEK_SET);
            log_info("log file size: %ld bytes\n", sz);

            copy_buff = malloc(sz);
            if (copy_buff == NULL)
            {
                log_error("Cannot allocate memory for coping\n");
                return false;
            }

            fread(copy_buff, 1, sz, f);
            printf("log file: '");
            for (int i = 0; i < sz; i++)
            {
                printf("%02X ", ((char *)copy_buff)[i]);
            }
            printf("'\n");

            fclose(f);
            fs->unmount();

            SDBlockDevice sd(PTE3, PTE1, PTE2, PTE4);
            FATFileSystem sdfs("sd");
            if (sdfs.mount(&sd) < 0)
            {
                log_error("Cannot mount sd\n");
                goto exit_error;
            }
            else
            {
                FILE *sdf = fopen("/sd/raw_data.txt", "wb+");
                if (sdf == NULL)
                {
                    log_error("Cannot open file on sd\n");
                    goto exit_error;
                }
                else
                {
                    fwrite(copy_buff, sizeof(char), sz, sdf);
                    log_info("File exported\n");
                    fclose(sdf);
                    sdfs.unmount();
                    return true;
                }
            }

        exit_error:
            free(copy_buff);
            return false;
        }
    }
}