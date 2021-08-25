#ifndef _STAT_DISPLAY_H_
#define _STAT_DISPLAY_H_

#include "mbed.h"
#include "label.h"
#include "Hexi_OLED_SSD1351.h"

class Stat_Display
{
public:
    Stat_Display(SSD1351 *oled);
    ~Stat_Display();

    void new_event(Label event);

    void init_display();
    void update_display();

private:
    SSD1351 *_oled;
    int _time_none;
    int _time_wash;
    int _time_san;
};

#endif // _STAT_DISPLAY_H_