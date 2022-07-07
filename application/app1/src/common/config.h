#ifndef CONFIG_H_
#define CONFIG_H_

#include "data/wrist.h"

#include "mbed.h"

class Config
{
public:
    Config();
    ~Config();

    void init();

    Wrist get_wrist() { return _wrist; }
    void toggle_wrist();

private:
    FILE *_config_file;

    Wrist _wrist;
};

#endif // CONFIG_H_