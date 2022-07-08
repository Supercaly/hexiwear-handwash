#ifndef NAVIGATOR_H_
#define NAVIGATOR_H_

#include "common/haptic_feedback.h"

#include "mbed.h"
#include "oled_ssd1351.h"
#include <stack>

class Navigator;

// Base Class representing a page on the navigation stack
class Page
{
public:
    // Draw callbacks
    virtual void draw(oled::SSD1351 *display, oled::Transition t) = 0;
    virtual void on_draw(oled::SSD1351 *display) = 0;
    // Button event callbacks
    virtual void event_up(Navigator *nav) = 0;
    virtual void event_down(Navigator *nav) = 0;
    virtual void event_left(Navigator *nav) = 0;
    virtual void event_right(Navigator *nav) = 0;
};

// Class representing a page navigator
// This class has the task to manage all the pages displayed
// by the menu and to respond to user button press
class Navigator
{
public:
    // Create a Navigator with an instance of the display and a root page
    Navigator(oled::SSD1351 *oled, Page *root);
    ~Navigator();

    // Button callbacks
    void on_key_up();
    void on_key_down();
    void on_key_left();
    void on_key_right();

    // Navigate to the given page
    void navigate_to(Page *to);

    // Navigate back to the last page
    void navigate_back();

    // Redraw the current page.
    // This methods will call on_draw on the page currently
    // on top of the navigation stack
    void redraw();

    // Return the instance of the display to draw on it
    oled::SSD1351 *get_display() { return _display; }

    // Respond to an event vibrating the device (haptic feedback)
    void do_haptic();

    // Return true if the given page is currently on top of the navigation stack
    bool is_top(Page *p) { return _nav_stack.top() == p; }

private:
    oled::SSD1351 *_display;
    HapticFeedback _haptic;
    std::stack<Page *> _nav_stack;
    
    Thread _btn_dispatcher_thread;
    EventQueue _btn_event_queue;
};

#endif // NAVIGATOR_H_