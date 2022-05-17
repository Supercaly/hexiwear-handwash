#include "menu.h"

Menu::Menu(MenuItem *items, int count) : _items(items),
                                         _items_count(count),
                                         _current(0) {}
Menu::~Menu() {}

void Menu::draw(oled::SSD1351 *display, oled::Transition t)
{
    display->draw_screen(_items[_current].image, t);
}

void Menu::on_draw(oled::SSD1351 *display) {
    // TODO: Draw menu arrows and remove them from static asset
}

void Menu::event_up(Navigator *nav)
{
    if (_current > 0)
    {
        nav->do_haptic();
        nav->get_display()->draw_screen(_items[--_current].image, oled::Transition::TOP_DOWN);
    }
}

void Menu::event_down(Navigator *nav)
{
    if (_current < _items_count - 1)
    {
        nav->do_haptic();
        nav->get_display()->draw_screen(_items[++_current].image, oled::Transition::DOWN_TOP);
    }
}

void Menu::event_left(Navigator *nav)
{
    nav->do_haptic();
    nav->navigate_back();
}

void Menu::event_right(Navigator *nav)
{
    nav->do_haptic();
    nav->navigate_to(_items[_current].item);
}
