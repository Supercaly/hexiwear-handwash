#ifndef DISPLAY_WRAPPER_H_
#define DISPLAY_WRAPPER_H_

#include "menu_resources.h"

#include "Hexi_KW40Z/Hexi_KW40Z.h"
#include "Hexi_OLED_SSD1351/Hexi_OLED_SSD1351.h"
#include "mbed.h"

typedef struct Page
{
    struct Page *up;
    struct Page *down;
    struct Page *left;
    struct Page *right;
    const uint8_t *image;
} page_t;

extern page_t page1;
extern page_t page2;

class DisplayWrapper
{
public:
    DisplayWrapper(SSD1351 *oled, KW40Z *kw40z);
    ~DisplayWrapper();

    void update_label_stats(int none_count, int wash_count, int san_count);

    // Button callbacks
    void btnUpFn();
    void btnDownFn();
    void btnLeftFn();
    void btnRightFn();

private:
    SSD1351 *_oled;
    KW40Z *_kw40z;

    page_t *current_page;
    
    void init_display();
    void clear_screen();
    void draw_page(page_t *page, oled_transition_t transition);

    int none_count, wash_count, san_count;

    // Haptic feedback management
    DigitalOut _haptic;
    RtosTimer _haptic_timer;
    void start_haptic();
    void stop_haptic();
};

#endif // DISPLAY_WRAPPER_H_