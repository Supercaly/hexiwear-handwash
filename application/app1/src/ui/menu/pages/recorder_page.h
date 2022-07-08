#ifndef RECORDER_PAGE_H_
#define RECORDER_PAGE_H_

#include "menu/navigator.h"

class RecorderPage : public Page
{
public:
    RecorderPage();

    void draw(oled::SSD1351 *display, oled::Transition t) override;
    void on_draw(oled::SSD1351 *display) override;

    void event_up(Navigator *nav) override;
    void event_down(Navigator *nav) override;
    void event_left(Navigator *nav) override;
    void event_right(Navigator *nav) override;

private:
    oled::DynamicArea _area;
};

#endif // RECORDER_PAGE_H_