#include "export_page.h"
#include "common/global_thread_vars.h"
#include "common/log.h"
#include "resources/menu_resources.h"

ExportPage::ExportPage()
{
    _txt = "Dock the\nHEXIWEAR";
    _area = {
        .xCrd = 10,
        .yCrd = 23,
        .width = 76,
        .height = 50};
}

void ExportPage::draw(oled::SSD1351 *display, oled::Transition t)
{
    display->draw_screen(export_bg_bmp, t);
}

void ExportPage::on_draw(oled::SSD1351 *display)
{
    display->set_dynamic_area(_area);
    display->text_box(_txt);
}

void ExportPage::event_up(Navigator *nav) {}

void ExportPage::event_down(Navigator *nav) {}

void ExportPage::event_left(Navigator *nav)
{
    nav->navigate_back();
}

void ExportPage::event_right(Navigator *nav)
{
    nav->do_haptic();
    _txt = "exporting...";
    nav->redraw();

    bool res = g_data_exporter.export_data();
    _txt = res ? "done" : "error";
    nav->redraw();
}
