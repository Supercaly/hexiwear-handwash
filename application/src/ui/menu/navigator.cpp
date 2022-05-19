#include "navigator.h"

Navigator::Navigator(oled::SSD1351 *oled, Page *root) : _display(oled),
                                                        _haptic(PTB9)
{
    _btn_dispatcher_thread.start(callback(&_btn_event_queue, &EventQueue::dispatch_forever));

    _display->dim_screen_off();
    _nav_stack.push(root);
    _nav_stack.top()->draw(_display, oled::Transition::NONE);
    _nav_stack.top()->on_draw(_display);
}

Navigator::~Navigator() {}

void Navigator::on_key_up()
{
    _btn_event_queue.call(_nav_stack.top(), &Page::event_up, this);
}

void Navigator::on_key_down()
{
    _btn_event_queue.call(_nav_stack.top(), &Page::event_down, this);
}

void Navigator::on_key_left()
{
    _btn_event_queue.call(_nav_stack.top(), &Page::event_left, this);
}

void Navigator::on_key_right()
{
    _btn_event_queue.call(_nav_stack.top(), &Page::event_right, this);
}

void Navigator::navigate_to(Page *to)
{
    if (to != NULL)
    {
        do_haptic();
        _nav_stack.push(to);
        to->draw(_display, oled::Transition::RIGHT_LEFT);
        to->on_draw(_display);
    }
}

void Navigator::navigate_back()
{
    if (_nav_stack.size() > 1)
    {
        do_haptic();
        _nav_stack.pop();
        _nav_stack.top()->draw(_display, oled::Transition::LEFT_RIGHT);
        _nav_stack.top()->on_draw(_display);
    }
}

void Navigator::redraw()
{
    _nav_stack.top()->on_draw(_display);
}

void Navigator::do_haptic()
{
    _haptic.vibrate();
}