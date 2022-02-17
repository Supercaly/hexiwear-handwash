#ifndef STAT_DISPLAY_H_
#define STAT_DISPLAY_H_

#include "label.h"

#include "Hexi_OLED_SSD1351.h"

class Stat_Display
{
public:
    Stat_Display(SSD1351 *oled);
    ~Stat_Display();

    void new_event(Label event);

    void update_display();

private:
    SSD1351 *_oled;
    uint64_t _time_none;
    uint64_t _time_wash;
    uint64_t _time_san;
};

#endif // STAT_DISPLAY_H_