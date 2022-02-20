#include "display_thread.h"
#include "display_wrapper.h"
#include "global_thread_vars.h"
#include "label.h"
#include "log.h"

#include "mbed.h"

SSD1351 g_oled(PTB22, PTB21, PTC13, PTB20, PTE6, PTD15);

void display_thread_loop()
{
    DisplayWrapper *display = new DisplayWrapper(&g_oled);
    int none_count = 0, wash_count = 0, san_count = 0;

    while (true)
    {
        osEvent event = g_labels_queue.get();
        if (event.status == osEventMessage)
        {
            // Got a new label from predictor
            Label label = *(Label *)event.value.p;
            log_info("Got label %s\n\n", label_to_cstr(label));

            switch (label)
            {
            case Label::NONE:
                none_count++;
                break;
            case Label::WASH:
                wash_count++;
                break;
            case Label::SAN:
                san_count++;
                break;
            }

            display->label_screen(none_count, wash_count, san_count);
        }
    }
}