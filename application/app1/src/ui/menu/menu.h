#ifndef MENU_H_
#define MENU_H_

#include "navigator.h"

#include <stdint.h>

#define TOP_ARROW_X 7
#define TOP_ARROW_Y 31
#define BOTTOM_ARROW_X 7
#define BOTTOM_ARROW_Y 58

// Struct representing a single menu item
// This struct has an image to display as splash 
// and a page to open
struct MenuItem
{
    const uint8_t *image;
    Page *item;
};

// Class representing a menu
class Menu : public Page
{
public:
    Menu(MenuItem *items, int count);
    ~Menu();

    void draw(oled::SSD1351 *display, oled::Transition t) override;
    void on_draw(oled::SSD1351 *display) override;

    void event_up(Navigator *nav) override;
    void event_down(Navigator *nav) override;
    void event_left(Navigator *nav) override;
    void event_right(Navigator *nav) override;

private:
    MenuItem *_items;
    int _items_count;
    int _current;
};

#endif // MENU_H_