#include "export_page.h"
#include "resources/menu_resources.h"

ExportPage::ExportPage()
{
    // _area = {
    //     .xCrd = 50,
    //     .yCrd = 81,
    //     .width = 44,
    //     .height = 15};
}

void ExportPage::draw(oled::SSD1351 *display, oled::Transition t)
{
    display->draw_screen(export_bg_bmp, t);
}

void ExportPage::on_draw(oled::SSD1351 *display)
{
    // display->set_dynamic_area(_area);
    // display->draw_image(g_data_recorder.is_recording()
    //                         ? stop_btn_bmp
    //                         : start_btn_bmp);
}

void ExportPage::event_up(Navigator *nav) {}

void ExportPage::event_down(Navigator *nav) {}

void ExportPage::event_left(Navigator *nav)
{
    nav->do_haptic();
    nav->navigate_back();
}

void ExportPage::event_right(Navigator *nav)
{
    nav->do_haptic();
}
