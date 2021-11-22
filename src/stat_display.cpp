#include "stat_display.h"

Stat_Display::Stat_Display(SSD1351 *oled) : _oled(oled),
                                            _time_none(0), _time_wash(0), _time_san(0)
{
    oled_text_properties_t txt_prop = {0};
    _oled->GetTextProperties(&txt_prop);

    _oled->DimScreenOFF();
    _oled->FillScreen(COLOR_BLACK);

    txt_prop.fontColor = COLOR_WHITE;
    txt_prop.alignParam = OLED_TEXT_ALIGN_CENTER;
    _oled->SetTextProperties(&txt_prop);
    _oled->Label((uint8_t *)"STATS", 30, 10);

    txt_prop.alignParam = OLED_TEXT_ALIGN_LEFT;
    _oled->SetTextProperties(&txt_prop);
    _oled->Label((uint8_t *)"none:", 5, 30);
    _oled->Label((uint8_t *)"wash:", 5, 50);
    _oled->Label((uint8_t *)"san:", 5, 70);

    update_display();
}

Stat_Display::~Stat_Display() {}

void Stat_Display::new_event(Label event)
{
    switch (event)
    {
    case Label::NONE:
        _time_none += 10;
        break;
    case Label::WASH:
        _time_wash += 10;
        break;
    case Label::SAN:
        _time_san += 10;
        break;
    }
    update_display();
}

void Stat_Display::update_display()
{
    char none_str[10];
    char wash_str[10];
    char san_str[10];
    sprintf(none_str, "%llus", _time_none);
    sprintf(wash_str, "%llus", _time_wash);
    sprintf(san_str, "%llus", _time_san);

    _oled->Label((uint8_t *)none_str, 45, 30);
    _oled->Label((uint8_t *)wash_str, 45, 50);
    _oled->Label((uint8_t *)san_str, 45, 70);
}