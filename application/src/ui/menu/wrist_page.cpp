#include "wrist_page.h"
#include "resources/menu_resources.h"

WristPage::WristPage() : _wrist(0)
{
    _area = {
        .xCrd = 20,
        .yCrd = 20,
        .width = 56,
        .height = 56};
}

void WristPage::draw(oled::SSD1351 *display, oled::Transition t)
{
    display->draw_screen(wrist_selection_bg_bmp, t);
}

void WristPage::on_draw(oled::SSD1351 *display)
{
    display->set_dynamic_area(_area);
    display->draw_image(_wrist
                            ? wrist_ct_left_bmp
                            : wrist_ct_right_bmp);
}

void WristPage::event_up(Navigator *nav) {}

void WristPage::event_down(Navigator *nav) {}

void WristPage::event_left(Navigator *nav)
{
    nav->do_haptic();
    nav->navigate_back();
}

void WristPage::event_right(Navigator *nav)
{
    nav->do_haptic();
    _wrist = !_wrist;
    nav->redraw();
}
