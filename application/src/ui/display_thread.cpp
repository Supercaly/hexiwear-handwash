#include "display_thread.h"
#include "common/global_thread_vars.h"
#include "common/log.h"
#include "data/label.h"

#include "menu/menu.h"
#include "menu/navigator.h"
#include "menu/stats_page.h"
#include "menu/wrist_page.h"
#include "menu/recorder_page.h"
#include "resources/menu_resources.h"

#include "kw40z.h"
#include "mbed.h"
#include "oled_ssd1351.h"
#include <stdint.h>

oled::SSD1351 g_oled(PTB22, PTB21, PTC13, PTB20, PTE6, PTD15);
KW40Z kw40z_device(PTE24, PTE25);

WristPage wrist_page;
StatsPage stats_page(&menu_nav);
RecorderPage recorder_page;
MenuItem settings_menu_wrist = {
    .image = wrist_menu_bmp,
    .item = &wrist_page};
MenuItem settings_menu_list[] = {settings_menu_wrist};
Menu settings_menu(settings_menu_list, 1);
MenuItem main_menu_settings = {
    .image = settings_menu_bmp,
    .item = &settings_menu};
MenuItem main_menu_rec = {
    .image = recorder_menu_bmp,
    .item = &recorder_page
};
MenuItem main_menu_stats = {
    .image = stats_menu_bmp,
    .item = &stats_page};
MenuItem main_menu_list[] = {main_menu_settings, main_menu_rec, main_menu_stats};
Menu main_menu(main_menu_list, 3);

Navigator menu_nav(&g_oled, &main_menu);

static void btn_up_fn()
{
    menu_nav.on_key_up();
}

static void btn_down_fn()
{
    menu_nav.on_key_down();
}

static void btn_left_fn()
{
    menu_nav.on_key_left();
}

static void btn_right_fn()
{
    menu_nav.on_key_right();
}

void display_thread_loop()
{
    // attach button callbacks that redirect input to display wrapper
    kw40z_device.attach_buttonUp(btn_up_fn);
    kw40z_device.attach_buttonDown(btn_down_fn);
    kw40z_device.attach_buttonLeft(btn_left_fn);
    kw40z_device.attach_buttonRight(btn_right_fn);

    while (true)
    {
        Label *label;
        if (g_labels_queue.try_get_for(Kernel::wait_for_u32_forever, &label))
        {
            // Got a new label from predictor
            log_info("Got label %s\n\n", label_to_cstr(*label));

            switch (*label)
            {
            case Label::NONE:
                stats_page.update_none();
                break;
            case Label::WASH:
                stats_page.update_wash();
                break;
            case Label::SAN:
                stats_page.update_san();
                break;
            }
        }
    }
}