#include "menu.h"

#include "resources/menu_resources.h"

Menu::Menu(MenuItem *items, int count) : _items(items),
                                         _items_count(count),
                                         _current(0) {}
Menu::~Menu() {}

void Menu::draw(oled::SSD1351 *display, oled::Transition t)
{
    display->draw_screen(_items[_current].image, t);
}

void Menu::on_draw(oled::SSD1351 *display)
{
    // Draw top arrow
    if (_current > 0)
    {
        display->set_dynamic_area({.xCrd = TOP_ARROW_X,
                                   .yCrd = TOP_ARROW_Y,
                                   .width = 4,
                                   .height = 5});
        display->draw_image(arrow_bmp);
    }

    // Draw bottom arrow
    if (_current < _items_count - 1)
    {
        display->set_dynamic_area({.xCrd = BOTTOM_ARROW_X,
                                   .yCrd = BOTTOM_ARROW_Y,
                                   .width = 4,
                                   .height = 5});
        display->draw_image(arrow_bmp);
    }
}

void Menu::event_up(Navigator *nav)
{
    if (_current > 0)
    {
        nav->do_haptic();
        nav->get_display()->draw_screen(_items[--_current].image, oled::Transition::TOP_DOWN);
        nav->redraw();
    }
}

void Menu::event_down(Navigator *nav)
{
    if (_current < _items_count - 1)
    {
        nav->do_haptic();
        nav->get_display()->draw_screen(_items[++_current].image, oled::Transition::DOWN_TOP);
        nav->redraw();
    }
}

void Menu::event_left(Navigator *nav)
{
    _current = 0;
    nav->navigate_back();
}

void Menu::event_right(Navigator *nav)
{
    nav->navigate_to(_items[_current].item);
}
