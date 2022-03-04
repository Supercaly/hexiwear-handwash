#ifndef MENU_PAGE_H_
#define MENU_PAGE_H_

#include "display_wrapper.h"
#include "Hexi_OLED_SSD1351/Hexi_OLED_SSD1351.h"
#include <stdint.h>

class DisplayWrapper;

struct Page
{
    struct Page *up;
    struct Page *down;
    struct Page *left;
    struct Page *right;
    void (*action)(DisplayWrapper *);
    void (*onDraw)(DisplayWrapper *);
    const uint8_t *image;
};

extern Page page_settings_menu;
extern Page page_stats_menu;
extern Page page_wrist_menu;
// TODO: Replace left and right pages with one and draw the content depending on local variables
extern Page page_wrist_left;
extern Page page_wrist_right;
extern Page page_stats;

#endif // MENU_PAGE_H_