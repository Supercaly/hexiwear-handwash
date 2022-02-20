#ifndef DISPLAY_WRAPPER_H_
#define DISPLAY_WRAPPER_H_

#include "Hexi_OLED_SSD1351/Hexi_OLED_SSD1351.h"

class DisplayWrapper
{
public:
    DisplayWrapper(SSD1351 *oled);
    ~DisplayWrapper();

    void label_screen(int none_count, int wash_count, int san_count);

private:
    SSD1351 *_oled;

    void init_display();
};

#endif // DISPLAY_WRAPPER_H_