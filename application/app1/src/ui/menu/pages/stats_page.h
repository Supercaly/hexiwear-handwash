#ifndef STATS_PAGE_H_
#define STATS_PAGE_H_

#include "navigator.h"

// Class representing the statistics page
// This class has methods to display the current wash/san counts
class StatsPage : public Page
{
public:
    StatsPage(Navigator *nav);

    void draw(oled::SSD1351 *display, oled::Transition t) override;
    void on_draw(oled::SSD1351 *display) override;

    void event_up(Navigator *nav) override;
    void event_down(Navigator *nav) override;
    void event_left(Navigator *nav) override;
    void event_right(Navigator *nav) override;

    void update_none();
    void update_wash();
    void update_san();

private:
    Navigator *_nav;
    char _str[5];
    oled::TextProperties _text_prop;
    oled::DynamicArea _wash_area,
        _san_area;
    int _none_count,
        _wash_count,
        _san_count;
};

#endif // STATS_PAGE_H_