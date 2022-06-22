#ifndef EXPORT_PAGE_H_
#define EXPORT_PAGE_H_

#include "menu/navigator.h"

class ExportPage : public Page
{
public:
    ExportPage();

    void draw(oled::SSD1351 *display, oled::Transition t) override;
    void on_draw(oled::SSD1351 *display) override;

    void event_up(Navigator *nav) override;
    void event_down(Navigator *nav) override;
    void event_left(Navigator *nav) override;
    void event_right(Navigator *nav) override;

private:
    oled::DynamicArea _area;
    const char *_txt;
};

#endif // EXPORT_PAGE_H_