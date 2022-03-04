#include "display_wrapper.h"

DisplayWrapper::DisplayWrapper(SSD1351 *oled, KW40Z *kw40z) : _oled(oled),
                                                              _kw40z(kw40z),
                                                              _haptic(PTB9),
                                                              _haptic_timer(this, &DisplayWrapper::stop_haptic, osTimerOnce)
{
    _wrist = 0;
    _current_page = &page_settings_menu;
}

DisplayWrapper::~DisplayWrapper() {}

void DisplayWrapper::init_display()
{
    _oled->DimScreenOFF();
    clear_screen();
    draw_page(_current_page, oled_transition_t::OLED_TRANSITION_NONE);
}

void DisplayWrapper::clear_screen()
{
    _oled->FillScreen(COLOR_BLACK);
}

void DisplayWrapper::draw_page(Page *page, oled_transition_t transition)
{
    _oled->DrawScreen(page->image, 0, 0, 96, 96, transition);
    if (page->onDraw != NULL)
    {
        page->onDraw(this);
    }
}

void DisplayWrapper::label(const char *txt, int x, int y)
{
    _oled->Label((uint8_t *)txt, x, y);
}

void DisplayWrapper::image(const uint8_t *image, int x, int y)
{
    _oled->DrawImage(image, 20, 20, 56, 56);
}

/*
 * ----------------------
 * button press callbacks
 * ----------------------
 */

void DisplayWrapper::btnUpFn()
{
    if (_current_page->up != NULL)
    {
        start_haptic();
        _current_page = _current_page->up;
        draw_page(_current_page, oled_transition_t::OLED_TRANSITION_TOP_DOWN);
    }
}

void DisplayWrapper::btnDownFn()
{
    if (_current_page->down != NULL)
    {
        _current_page = _current_page->down;
        start_haptic();
        draw_page(_current_page, oled_transition_t::OLED_TRANSITION_DOWN_TOP);
    }
}

void DisplayWrapper::btnLeftFn()
{
    if (_current_page->left != NULL)
    {
        start_haptic();
        _current_page = _current_page->left;
        draw_page(_current_page, oled_transition_t::OLED_TRANSITION_LEFT_RIGHT);
    }
}

void DisplayWrapper::btnRightFn()
{
    if (_current_page->right != NULL)
    {
        start_haptic();
        _current_page = _current_page->right;
        draw_page(_current_page, oled_transition_t::OLED_TRANSITION_RIGHT_LEFT);
    }
    else if (_current_page->action != NULL)
    {
        start_haptic();
        _current_page->action(this);
    }
}

void DisplayWrapper::update_label_stats(int none_count, int wash_count, int san_count)
{
    _none_count = none_count;
    _wash_count = wash_count;
    _san_count = san_count;

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