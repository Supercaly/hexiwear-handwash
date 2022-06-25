#include "config.h"
#include "common/log.h"

Config::Config() {}

Config::~Config()
{
    fclose(_config_file);
    FileSystem::get_default_instance()->unmount();
}

void Config::init()
{
    FileSystem *fs = FileSystem::get_default_instance();
    if (fs->mount(BlockDevice::get_default_instance()) < 0)
    {
        log_error("Config: cannot mount filesystem for reading config!\n");
    }
    else
    {
        _config_file = fopen("/fs/config", "rb+");
        if (_config_file == NULL)
        {
            // Config file not existing... create it
            _config_file = fopen("/fs/config", "wb+");
            if (_config_file == NULL)
            {
                log_error("Config: Cannot open log file %s(%d)\n", strerror(errno), errno);
                fs->unmount();
            }
            else
            {
                log_info("Config: config file created!\n");
                fseek(_config_file, 0L, SEEK_SET);
                fwrite(&_wrist, sizeof(Wrist), 1, _config_file);
                fclose(_config_file);
            }
        }
        else
        {
            // Read previously stored configs
            fseek(_config_file, 0L, SEEK_SET);
            fread(&_wrist, sizeof(Wrist), 1, _config_file);
            fclose(_config_file);
        }

        printf("Config:\n");
        printf("  wrist: '%d'\n", _wrist);
    }
}

void Config::toggle_wrist()
{
    _wrist = _wrist == Wrist::LEFT ? Wrist::RIGHT : Wrist::LEFT;

    // Open config file and update the wrist
    FileSystem *fs = FileSystem::get_default_instance();
    if (fs->mount(BlockDevice::get_default_instance()) < 0)
    {
        log_error("Config: cannot mount filesystem for reading config!\n");
    }
    else
    {
        _config_file = fopen("/fs/config", "wb+");
        if (_config_file == NULL)
        {
            log_error("Config: Cannot open log file %s(%d)\n", strerror(errno), errno);
            fs->unmount();
        }
        else
        {
            fseek(_config_file, 0L, SEEK_SET);
            fwrite(&_wrist, sizeof(Wrist), 1, _config_file);
            fclose(_config_file);
        }
    }
}