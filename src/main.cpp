#include <string.h>
#include "mbed.h"
#include "Hexi_OLED_SSD1351.h"
#include "stat_display.h"
#include "sensors.h"
#include "label.h"
#include "label_predictor.h"
#include "log.h"

SSD1351 g_oled(PTB22, PTB21, PTC13, PTB20, PTE6, PTD15);
Queue<Label, 5> g_labels_queue;
Sensors g_sensors;
Mutex g_sensors_lock;

void sensor_collect_thread_loop()
{
    Timer timer;

    while (true)
    {
        g_sensors.reset();
        timer.reset();
        timer.start();

        // Enter critical section
        g_sensors_lock.lock();
        log("Begin sensor data aquisition\n");
        g_sensors.acquire_data();
        timer.stop();
        log("End sensor data aquisition\n");
        g_sensors_lock.unlock();
        // Exit critical section

        log("Got %d samples in %dms!\n",
            g_sensors.data_size,
            timer.read_ms());
    }
}

My_model model;
void prediction_thread_loop()
{
    float *copy = (float *)calloc(SENSORS_DATA_CAPACITY * 6, sizeof(float));
    if (copy == NULL)
    {
        error("can't allocate memory for sensor data copy");
    }

    Label_Predictor p(&model);

    while (true)
    {
        // Enter critical section
        // Copy the data to a local buffer so the critical
        // section last for only a for-loop
        g_sensors_lock.lock();
        for (int i = 0; i < SENSORS_DATA_CAPACITY * 6; i += 6)
        {
            copy[i + 0] = g_sensors.data[i].ax;
            copy[i + 1] = g_sensors.data[i].ay;
            copy[i + 2] = g_sensors.data[i].az;
            copy[i + 3] = g_sensors.data[i].gx;
            copy[i + 4] = g_sensors.data[i].gy;
            copy[i + 5] = g_sensors.data[i].gz;
        }
        g_sensors_lock.unlock();
        // Exit critical section

        Label label = p.predict(copy);
        g_labels_queue.put(&label);
    }
}

void display_thread_loop()
{
    Stat_Display stat(&g_oled);

    while (true)
    {
        osEvent event = g_labels_queue.get();
        if (event.status == osEventMessage)
        {
            Label label = *(Label *)event.value.p;
            log("\nGot label %s\n\n", label_to_cstr(label));
            stat.new_event(label);
        }
    }
}

int main2()
{
    float *data = (float *)calloc(4 * 6, sizeof(float));

    for (int i = 0; i < 24; i += 6)
    {
        data[i + 0] = i / 6 + 1;
        data[i + 1] = i / 6 + 1;
        data[i + 2] = i / 6 + 1;
        data[i + 3] = i / 6 + 1;
        data[i + 4] = i / 6 + 1;
        data[i + 5] = i / 6 + 1;
    }

    uTensor::Tensor t = new uTensor::RomTensor({6, 4}, flt, data);

    for (uint32_t i = 0; i < (*t)->num_elems(); ++i)
    {
        float v = static_cast<float>(t(i));
        log("%f\n", v);
    }

    return 0;
}

int main()
{
    Thread sensor_thread;
    Thread prediction_thread;
    Thread display_thread;
    sensor_thread.start(sensor_collect_thread_loop);
    prediction_thread.start(prediction_thread_loop);
    display_thread.start(display_thread_loop);

    DigitalOut status_led(LED_RED);
    while (true)
    {
        status_led = !status_led;
        wait_ms(500);
    }

    return 0;
}