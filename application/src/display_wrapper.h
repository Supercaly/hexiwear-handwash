#ifndef DISPLAY_WRAPPER_H_
#define DISPLAY_WRAPPER_H_

#include "menu_page.h"

#include "Hexi_KW40Z/Hexi_KW40Z.h"
#include "Hexi_OLED_SSD1351/Hexi_OLED_SSD1351.h"
#include "mbed.h"

class Page;

class DisplayWrapper
{
public:
    DisplayWrapper(SSD1351 *oled, KW40Z *kw40z);
    ~DisplayWrapper();

    // Init the display
    void init_display();

    // Button callbacks
    void btnUpFn();
    void btnDownFn();
    void btnLeftFn();
    void btnRightFn();

    // Getters for counters
    void update_label_stats(int none_count, int wash_count, int san_count);
    int none_count() { return _none_count; }
    int wash_count() { return _wash_count; }
    int san_count() { return _san_count; }
    
    int wrist() { return _wrist; }
    void wrist(int w) { _wrist = w; }

    // Display API wrapper
    void label(const char *txt, int x, int y);
    void image(const uint8_t *image, int x, int y);

private:
    SSD1351 *_oled;
    KW40Z *_kw40z;

    Page *_current_page;

    int _wrist;

    void clear_screen();
    void draw_page(Page *page, oled_transition_t transition);

    int _none_count, _wash_count, _san_count;

    // Haptic feedback management
    DigitalOut _haptic;
    RtosTimer _haptic_timer;
    void start_haptic();
    void stop_haptic();
};

#endif // DISPLAY_WRAPPER_H_