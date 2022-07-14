#ifndef CONFIG_H_
#define CONFIG_H_

#include "LittleFileSystem.h"
#include "SPIFBlockDevice.h"
#include "mbed.h"
#include "wrist.h"

class Config
{
public:
    Config();
    ~Config();

    void init();

    Wrist get_wrist() { return _wrist; }
    void toggle_wrist();

private:
    SPIFBlockDevice *_spif_bd;
    LittleFileSystem *_fs;
    FILE *_config_file;

    Wrist _wrist;
};

#endif // CONFIG_H_