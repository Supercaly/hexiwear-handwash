#include "display_wrapper.h"

// TODO: Create proper screen pages pointing to real graphical resources
page_t page1 = {
    .down = &page2,
    .image = menuItem_main_bmp,
};

page_t page2 = {
    .up = &page1,
    .image = menuItem_settings_bmp,
};

DisplayWrapper::DisplayWrapper(SSD1351 *oled, KW40Z *kw40z) : _oled(oled),
                                                              _kw40z(kw40z),
                                                              _haptic(PTB9),
                                                              _haptic_timer(this, &DisplayWrapper::stop_haptic, osTimerOnce)
{
    current_page = &page1;
    init_display();
}

DisplayWrapper::~DisplayWrapper() {}

void DisplayWrapper::init_display()
{
    _oled->DimScreenOFF();
    clear_screen();
    draw_page(current_page, oled_transition_t::OLED_TRANSITION_NONE);
}

void DisplayWrapper::clear_screen()
{
    _oled->FillScreen(COLOR_BLACK);
}

void DisplayWrapper::draw_page(page_t *page, oled_transition_t transition)
{
    _oled->DrawScreen(page->image, 0, 0, 96, 96, transition);
}

/*
 * ----------------------
 * button press callbacks
 * ----------------------
 */

void DisplayWrapper::btnUpFn()
{
    if (current_page->up != NULL)
    {
        start_haptic();
        current_page = current_page->up;
        draw_page(current_page, oled_transition_t::OLED_TRANSITION_DOWN_TOP);
    }
}

void DisplayWrapper::btnDownFn()
{
    if (current_page->down != NULL)
    {
        start_haptic();
        current_page = current_page->down;
        draw_page(current_page, oled_transition_t::OLED_TRANSITION_TOP_DOWN);
    }
}

void DisplayWrapper::btnLeftFn()
{
    if (current_page->left != NULL)
    {
        start_haptic();
        current_page = current_page->left;
        draw_page(current_page, oled_transition_t::OLED_TRANSITION_LEFT_RIGHT);
    }
}

void DisplayWrapper::btnRightFn()
{
    if (current_page->right != NULL)
    {
        start_haptic();
        current_page = current_page->right;
        draw_page(current_page, oled_transition_t::OLED_TRANSITION_RIGHT_LEFT);
    }
}

void DisplayWrapper::update_label_stats(int none_count, int wash_count, int san_count)
{
    this->none_count = none_count;
    this->wash_count = wash_count;
    this->san_count = san_count;

    // if (current_screen == 1)
    // {
    //     oled_text_properties_t txt_prop = {0};
    //     _oled->GetTextProperties(&txt_prop);
    //     txt_prop.fontColor = COLOR_WHITE;
    //     _oled->SetTextProperties(&txt_prop);
    //     _oled->Label((uint8_t *)"STATS", 23, 5);
    //     char none_str[20];
    //     char wash_str[20];
    //     char san_str[20];
    //     sprintf(none_str, "none: %d", none_count);
    //     sprintf(wash_str, "wash: %d", wash_count);
    //     sprintf(san_str, "san: %d", san_count);
    //     _oled->Label((uint8_t *)none_str, 5, 25);
    //     _oled->Label((uint8_t *)wash_str, 5, 40);
    //     _oled->Label((uint8_t *)san_str, 5, 60);
    // }
}

/*
 * --------------------------
 * haptic feedback management
 * --------------------------
 */

void DisplayWrapper::start_haptic()
{
    _haptic_timer.start(50);
    _haptic = 1;
}

void DisplayWrapper::stop_haptic()
{
    _haptic = 0;
    _haptic_timer.stop();
}