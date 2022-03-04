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
    .action = [](DisplayWrapper *display)
    {
        display->wrist(!display->wrist());
        if (display->wrist())
        {
            display->image(wrist_left_content_bmp, 20, 20);
        }
        else
        {
            display->image(wrist_right_content_bmp, 20, 20);
        }
    },
    .image = wrist_menu_bmp,
};
Page page_wrist_left = {
    .left = &page_wrist_menu,
    .right = &page_wrist_right,
    .image = wrist_left_bmp,
};
Page page_wrist_right = {
    .left = &page_wrist_menu,
    .right = &page_wrist_left,
    .image = wrist_right_bmp,
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
        d->label((const char *)d->none_count() + 33, 50, 70);
    },
    .image = stats_bmp,
};