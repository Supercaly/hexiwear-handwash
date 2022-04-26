#include "recorder_page.h"
#include "resources/menu_resources.h"

RecorderPage::RecorderPage()
{
    _area = {
        .xCrd = 50,
        .yCrd = 81,
        .width = 44,
        .height = 15};
}

void RecorderPage::draw(oled::SSD1351 *display, oled::Transition t)
{
    display->draw_screen(recorder_start_bmp, t);
}

void RecorderPage::on_draw(oled::SSD1351 *display)
{
    display->set_dynamic_area(_area);
    display->draw_image(_recording ? stop_btn_bmp : start_btn_bmp);
}

void RecorderPage::event_up(Navigator *nav) {}

void RecorderPage::event_down(Navigator *nav) {}

void RecorderPage::event_left(Navigator *nav)
{
    nav->do_haptic();
    nav->navigate_back();
}

void RecorderPage::event_right(Navigator *nav)
{
    nav->do_haptic();
    _recording = !_recording;
    nav->redraw();
}
