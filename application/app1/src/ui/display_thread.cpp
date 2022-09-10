#include "display_thread.h"
#include "common/global_thread_vars.h"
#include "menu/menu.h"
#include "menu/navigator.h"
#include "menu/pages/export_page.h"
#include "menu/pages/recorder_page.h"
#include "menu/pages/stats_page.h"
#include "menu/pages/wrist_page.h"
#include "resources/menu_resources.h"

#include "kw40z.h"
#include "label.h"
#include "log.h"
#include "mbed.h"
#include "oled_ssd1351.h"
#include <stdint.h>

oled::SSD1351 g_oled(PTB22, PTB21, PTC13, PTB20, PTE6, PTD15);
KW40Z kw40z_device(PTE24, PTE25);

class ResetPage : public Page
{
public:
    ResetPage(void (*ctx)(void));

    // Draw callbacks
    void draw(oled::SSD1351 *display, oled::Transition t)
    {
        display->draw_screen(sucsess_bmp, t);
    }
    void on_draw(oled::SSD1351 *display)
    {
        _context();
    }
    // Button event callbacks
    void event_up(Navigator *nav) {}
    void event_down(Navigator *nav) {}
    void event_left(Navigator *nav) { nav->navigate_back(); }
    void event_right(Navigator *nav) {}

private:
    void (*_context)(void);
};

ResetPage::ResetPage(void (*ctx)(void)) : _context(ctx) {}

class SimplePage : public Page
{
public:
    SimplePage(const uint8_t *image);

    // Draw callbacks
    void draw(oled::SSD1351 *display, oled::Transition t)
    {
        display->draw_screen(_img, t);
    }
    void on_draw(oled::SSD1351 *display) {}
    // Button event callbacks
    void event_up(Navigator *nav) {}
    void event_down(Navigator *nav) {}
    void event_left(Navigator *nav) { nav->navigate_back(); }
    void event_right(Navigator *nav) {}

private:
    const uint8_t *_img;
};

SimplePage::SimplePage(const uint8_t *image) : _img(image) {}

// -------
//  MENUS
// -------
// WristPage wrist_page;
// RecorderPage recorder_page;
// ExportPage export_page;
// MenuItem settings_menu_list[] = {};
// Menu settings_menu(settings_menu_list,2);
void reset_counters(void);

SimplePage about_page(about_bmp);
ResetPage success_page(&reset_counters);

MenuItem settings_menu_reset = {.image = reset_bmp, .item = &success_page};
MenuItem settings_menu_about = {.image = about_menu_bmp, .item = &about_page};
MenuItem settings_menu_list[] = {settings_menu_reset, settings_menu_about};
Menu settings_menu(settings_menu_list, 2);

// Page about_menu_page(&about_page);
// ResetPage reset_page(&about_menu_page, &success_page);
StatsPage stats_page(&menu_nav, &settings_menu);

void reset_counters(void)
{
    stats_page.reset();
}

// MenuItem settings_menu_wrist = {
//     .image = wrist_menu_bmp,
//     .item = &wrist_page};
// // TODO: Remove unused record menu
// MenuItem settings_menu_rec = {
//     .image = recorder_menu_bmp,
//     .item = &recorder_page};
// // TODO: Remove unused export menu
// MenuItem settings_menu_export = {
//     .image = export_to_sd_menu_bmp,
//     .item = &export_page};

// MenuItem settings_menu_list[] = {settings_menu_wrist, settings_menu_rec, settings_menu_export};
// Menu settings_menu(settings_menu_list, 3);

// MenuItem main_menu_settings = {
//     .image = settings_menu_bmp,
//     .item = &settings_menu};
// MenuItem main_menu_stats = {
//     .image = stats_menu_bmp,
//     .item = &stats_page};

// MenuItem main_menu_list[] = {main_menu_stats, main_menu_settings};
// Menu main_menu(main_menu_list, 2);

Navigator menu_nav(&g_oled, &stats_page);

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