#include "menu_page.h"
#include "generated/menu_resources.h"

#include "log.h"

Page page_settings_menu = {
    .down = &page_stats_menu,
    .right = &page_wrist_menu,
    .image = settings_menu_bmp,
};
Page page_stats_menu = {
    .up = &page_settings_menu,
    .right = &page_stats,
    .image = stats_menu_bmp,
};
Page page_wrist_menu = {
    .left = &page_settings_menu,
    .right = &page_wrist_selection,
    .image = wrist_menu_bmp,
};
Page page_wrist_selection = {
    .left = &page_wrist_menu,
    .action = [](DisplayWrapper *display)
    {
        display->wrist(!display->wrist());
        if (display->wrist())
        {
            display->image(wrist_ct_left_bmp, 20, 20);
        }
        else
        {
            display->image(wrist_ct_right_bmp, 20, 20);
        }
    },
    .onDraw = [](DisplayWrapper *display)
    {
        if (display->wrist())
        {
            display->image(wrist_ct_left_bmp, 20, 20);
        } else 
        {
            display->image(wrist_ct_right_bmp, 20, 20);
        }
    },
    .image = wrist_selection_bg_bmp,
};
Page page_stats = {
    .left = &page_stats_menu,
    .onDraw = [](DisplayWrapper *d)
    {
        // char *none_str;
        // char *wash_str;
        // char *san_str;
        // sprintf(none_str, "%d", d->none_count());
        // sprintf(wash_str, "%d", d->wash_count());
        // sprintf(san_str, "%d", d->san_count());
        // d->label(none_str, 50, 30);
        // d->label(wash_str, 50, 50);
        d->label("ciao", 58, 8);
        d->label("ciao", 58, 40);
        d->label((const char *)"ciao", 58, 64);
    },
    .image = stats_bg_bmp,
};