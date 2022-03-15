#include "display_thread.h"
#include "display_wrapper.h"
#include "global_thread_vars.h"
#include "label.h"
#include "log.h"

#include "Hexi_KW40Z/Hexi_KW40Z.h"
#include "oled/oled_SSD1351.h"
#include "mbed.h"

oled::SSD1351 g_oled(PTB22, PTB21, PTC13, PTB20, PTE6, PTD15);
KW40Z kw40z_device(PTE24, PTE25);
DisplayWrapper display(&g_oled, &kw40z_device);

static void btn_up_fn()
{
    display.btnUpFn();
}

static void btn_down_fn()
{
    display.btnDownFn();
}

static void btn_left_fn()
{
    display.btnLeftFn();
}

static void btn_right_fn()
{
    display.btnRightFn();
}

void display_thread_loop()
{
    // initialize display
    display.init_display();
    // attach button callbacks that redirect input to display wrapper
    kw40z_device.attach_buttonUp(btn_up_fn);
    kw40z_device.attach_buttonDown(btn_down_fn);
    kw40z_device.attach_buttonLeft(btn_left_fn);
    kw40z_device.attach_buttonRight(btn_right_fn);

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

            display.update_label_stats(none_count, wash_count, san_count);
        }
    }
}