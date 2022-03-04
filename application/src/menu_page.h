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

// Predefined pages used by the application
extern Page page_settings_menu;
extern Page page_stats_menu;
extern Page page_wrist_menu;
extern Page page_wrist_selection;
extern Page page_stats;

#endif // MENU_PAGE_H_