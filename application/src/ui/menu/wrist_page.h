#ifndef WRIST_PAGE_H_
#define WRIST_PAGE_H_

#include "menu/navigator.h"

class WristPage : public Page
{
public:
    WristPage();

    void draw(oled::SSD1351 *display, oled::Transition t) override;
    void on_draw(oled::SSD1351 *display) override;

    void event_up(Navigator *nav) override;
    void event_down(Navigator *nav) override;
    void event_left(Navigator *nav) override;
    void event_right(Navigator *nav) override;

private:
    oled::DynamicArea _area;
    // TODO: Move this into global settings
    int _wrist;
};

#endif // WRIST_PAGE_H_