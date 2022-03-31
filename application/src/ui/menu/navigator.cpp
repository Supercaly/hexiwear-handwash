#include "navigator.h"

Navigator::Navigator(oled::SSD1351 *oled, Page *root) : _display(oled),
                                                        _haptic(PTB9),
                                                        _haptic_timer(this, &Navigator::stop_haptic, osTimerOnce)
{
    _display->dim_screen_off();
    _nav_stack.push(root);
    _nav_stack.top()->draw(_display, oled::Transition::NONE);
    _nav_stack.top()->on_draw(_display);
}

Navigator::~Navigator() {}

void Navigator::on_key_up()
{
    _nav_stack.top()->event_up(this);
}

void Navigator::on_key_down()
{
    _nav_stack.top()->event_down(this);
}

void Navigator::on_key_left()
{
    _nav_stack.top()->event_left(this);
}

void Navigator::on_key_right()
{
    _nav_stack.top()->event_right(this);
}

void Navigator::navigate_to(Page *to)
{
    if (to != NULL)
    {
        _nav_stack.push(to);
        to->draw(_display, oled::Transition::RIGHT_LEFT);
        to->on_draw(_display);
    }
}

void Navigator::navigate_back()
{
    if (_nav_stack.size() > 1)
    {
        _nav_stack.pop();
        _nav_stack.top()->draw(_display, oled::Transition::LEFT_RIGHT);
        _nav_stack.top()->on_draw(_display);
    }
}

void Navigator::do_haptic()
{
    _haptic_timer.start(50);
    _haptic = 1;
}

void Navigator::stop_haptic()
{
    _haptic = 0;
    _haptic_timer.stop();
}

void Navigator::redraw()
{
    _nav_stack.top()->on_draw(_display);
}