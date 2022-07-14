#include "config.h"

#include "log.h"

Config::Config()
{
    _spif_bd = new SPIFBlockDevice(PTD6, PTD7, PTD5, PTD4);
    _fs = new LittleFileSystem("fs");
}

Config::~Config()
{
    fclose(_config_file);
    _fs->unmount();

    delete _fs;
    delete _spif_bd;
}

void Config::init()
{
    if (_fs->mount(_spif_bd) < 0)
    {
        if (_fs->reformat(_spif_bd) < 0)
        {
            log_error("Config: cannot mount filesystem for reading config!\n");
            return;
        }
    }

    _config_file = fopen("/fs/config", "rb+");
    if (_config_file == NULL)
    {
        // Config file not existing... create it
        _config_file = fopen("/fs/config", "wb+");
        if (_config_file == NULL)
        {
            log_error("Config: Cannot open log file %s(%d)\n", strerror(errno), errno);
            _fs->unmount();
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

    log_info("Config:\n");
    log_info("  wrist: '%d'\n", _wrist);
}

void Config::toggle_wrist()
{
    _wrist = _wrist == Wrist::LEFT ? Wrist::RIGHT : Wrist::LEFT;

    _config_file = fopen("/fs/config", "wb+");
    if (_config_file == NULL)
    {
        log_error("Config: Cannot open log file %s(%d)\n", strerror(errno), errno);
    }
    else
    {
        fseek(_config_file, 0L, SEEK_SET);
        fwrite(&_wrist, sizeof(Wrist), 1, _config_file);
        fclose(_config_file);
    }
}