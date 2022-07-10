#include "export_page.h"
#include "common/global_thread_vars.h"
#include "resources/menu_resources.h"

#include "log.h"

ExportPage::ExportPage()
{
    _txt = "Dock the hexi\nand press\nexport";
    _area = {
        .xCrd = 5,
        .yCrd = 23,
        .width = 86,
        .height = 50};
}

void ExportPage::draw(oled::SSD1351 *display, oled::Transition t)
{
    display->draw_screen(export_bg_bmp, t);
}

void ExportPage::on_draw(oled::SSD1351 *display)
{
    display->set_dynamic_area(_area);
    oled::TextProperties prop = {0};
    display->get_text_properties(&prop);
    prop.alignParam = TEXT_ALIGN_CENTER | TEXT_ALIGN_VCENTER;
    display->set_text_properties(&prop);
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
    _txt = res ? "File exported!" : "Error exporting\nto sd!";
    nav->redraw();
}
