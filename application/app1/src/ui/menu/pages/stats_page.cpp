#include "stats_page.h"
#include "resources/menu_resources.h"

StatsPage::StatsPage(Navigator *nav, Page *menu) : _nav(nav), _menu(menu)
{
    _none_count = 0;
    _wash_count = 0;
    _san_count = 0;

    _wash_area = {
        .xCrd = 57,
        .yCrd = 19,
        .width = 30,
        .height = 20};
    _san_area = {
        .xCrd = 57,
        .yCrd = 44,
        .width = 30,
        .height = 20};
}

void StatsPage::draw(oled::SSD1351 *display, oled::Transition t)
{
    display->draw_screen(stats_bg_revisited_bmp, t);
}

void StatsPage::on_draw(oled::SSD1351 *display)
{
    display->get_text_properties(&_text_prop);
    _text_prop.alignParam = TEXT_ALIGN_CENTER | TEXT_ALIGN_VCENTER;
    display->set_text_properties(&_text_prop);

    display->set_dynamic_area(_wash_area);
    sprintf(_str, "%d", _wash_count);
    display->text_box(_str);

    display->set_dynamic_area(_san_area);
    sprintf(_str, "%d", _san_count);
    display->text_box(_str);
}

void StatsPage::event_up(Navigator *nav) {}

void StatsPage::event_down(Navigator *nav) {}

void StatsPage::event_left(Navigator *nav) {}

void StatsPage::event_right(Navigator *nav)
{
    nav->navigate_to(_menu);
}

void StatsPage::update_none()
{
    _none_count++;
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

void StatsPage::reset()
{
    _none_count = 0;
    _wash_count = 0;
    _san_count = 0;
    if (_nav->is_top(this))
    {
        _nav->redraw();
    }
}