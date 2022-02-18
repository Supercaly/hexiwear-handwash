#ifndef DISPLAY_THREAD_H_
#define DISPLAY_THREAD_H_

#include "global_thread_vars.h"
#include "label.h"
#include "log.h"
#include "stat_display.h"

#include "mbed.h"

SSD1351 g_oled(PTB22, PTB21, PTC13, PTB20, PTE6, PTD15);

void display_thread_loop()
{
    Stat_Display stat(&g_oled);

    while (true)
    {
        osEvent event = g_labels_queue.get();
        if (event.status == osEventMessage)
        {
            Label label = *(Label *)event.value.p;
            log_info("\nGot label %s\n\n", label_to_cstr(label));
            stat.new_event(label);
        }
    }
}

#endif // DISPLAY_THREAD_H_