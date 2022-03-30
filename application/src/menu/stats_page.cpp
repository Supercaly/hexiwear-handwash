#include "stats_page.h"
#include "generated/menu_resources.h"

StatsPage::StatsPage(Navigator *nav) : _nav(nav)
{
    _none_count = 0;
    _wash_count = 0;
    _san_count = 0;

    _none_area = {
        .xCrd = 57,
        .yCrd = 8,
        .width = 30,
        .height = 20};
    _wash_area = {
        .xCrd = 57,
        .yCrd = 31,
        .width = 30,
        .height = 20};
    _san_area = {
        .xCrd = 57,
        .yCrd = 54,
        .width = 30,
        .height = 20};
}

void StatsPage::draw(oled::SSD1351 *display, oled::Transition t)
{
    display->draw_screen(stats_bg_bmp, t);
}

void StatsPage::on_draw(oled::SSD1351 *display)
{
    display->get_text_properties(&_text_prop);
    _text_prop.alignParam = TEXT_ALIGN_CENTER | TEXT_ALIGN_VCENTER;
    display->set_text_properties(&_text_prop);

    // Workaround for the overlapping bug of oled (#1)
    // On every write repaint the bg black to get rid of the old text
    // TODO: Remove this after that bug has been fixed
    // oled::DynamicArea bg_area = {
    //     .xCrd = 57,
    //     .yCrd = 11,
    //     .width = 30,
    //     .height = 14};
    // display->set_dynamic_area(bg_area);
    // display->draw_box(oled::Color::BLACK);

    display->set_dynamic_area(_none_area);
    sprintf(_str, "%d", _none_count);
    display->text_box(_str);

    display->set_dynamic_area(_wash_area);
    sprintf(_str, "%d", _wash_count);
    display->text_box(_str);

    display->set_dynamic_area(_san_area);
    sprintf(_str, "%d", _san_count);
    display->text_box(_str);
}

void StatsPage::event_up(Navigator *nav) {}

void StatsPage::event_down(Navigator *nav) {}

void StatsPage::event_left(Navigator *nav)
{
    nav->do_haptic();
    nav->navigate_back();
}

void StatsPage::event_right(Navigator *nav) {}

void StatsPage::update_none()
{
    _none_count++;
    if (_nav->is_top(this))
    {
        _nav->redraw();
    }
}
void StatsPage::update_wash()
{
    _wash_count++;
    if (_nav->is_top(this))
    {
        _nav->redraw();
    }
}
void StatsPage::update_san()
{
    _san_count++;
    if (_nav->is_top(this))
    {
        _nav->redraw();
    }
}