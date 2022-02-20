#include "display_wrapper.h"

DisplayWrapper::DisplayWrapper(SSD1351 *oled) : _oled(oled)
{
    init_display();
}

DisplayWrapper::~DisplayWrapper() {}

void DisplayWrapper::label_screen(int none_count, int wash_count, int san_count)
{
    _oled->Label((uint8_t *)"STATS", 30, 10);

    char none_str[20];
    char wash_str[20];
    char san_str[20];
    sprintf(none_str, "none: %llu", none_count);
    sprintf(wash_str, "wash: %llu", wash_count);
    sprintf(san_str, "san: %llu", san_count);
    _oled->Label((uint8_t *)none_str, 5, 30);
    _oled->Label((uint8_t *)wash_str, 5, 50);
    _oled->Label((uint8_t *)san_str, 5, 70);
}

void DisplayWrapper::init_display()
{
    _oled->DimScreenOFF();
    _oled->FillScreen(COLOR_BLACK);

    oled_text_properties_t txt_prop = {0};
    _oled->GetTextProperties(&txt_prop);
    txt_prop.fontColor = COLOR_WHITE;
    txt_prop.alignParam = OLED_TEXT_ALIGN_CENTER;
    _oled->SetTextProperties(&txt_prop);
}