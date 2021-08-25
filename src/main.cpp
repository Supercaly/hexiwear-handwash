#include <string.h>
#include "mbed.h"
#include "Hexi_OLED_SSD1351.h"
#include "stat_display.h"
#include "sensors.h"
#include "label.h"

DigitalOut status_led(LED_RED);
SSD1351 oled(PTB22, PTB21, PTC13, PTB20, PTE6, PTD15);
Serial ps(USBTX, USBRX);
Queue<Label, 5> labels_queue;

void sensor_thread_loop()
{
    Sensors sensors;
    Timer timer;
    int i = 0;

    while (true)
    {
        sensors.reset();
        timer.reset();
        timer.start();

        ps.printf("Begin sensor data aquisition\n");
        sensors.acquire_data();
        timer.stop();
        ps.printf("End sensor data aquisition\n");

        ps.printf("Got %d samples in %dms!\n\n",
                  sensors.data_size,
                  timer.read_ms());

        Label evt = Label((++i) % 3);
        labels_queue.put(&evt);
    }
}

void display_thread_loop()
{
    Stat_Display stat(&oled);
    char val[10];
    stat.init_display();

    while (true)
    {
        osEvent event = labels_queue.get();
        if (event.status == osEventMessage)
        {
            Label label = *(Label *)event.value.p;
            ps.printf("Got label %s\n", label_to_cstr(label));
            stat.new_event(label);
        }
    }
}

int main()
{
    Thread sensor_thread;
    Thread display_thread;
    sensor_thread.start(sensor_thread_loop);
    display_thread.start(display_thread_loop);

    while (true)
    {
        status_led = !status_led;
        wait_ms(500);
    }

    return 0;
}