#include "sensor_collector.h"
#include "stat_display.h"
#include "label.h"
#include "label_predictor.h"
#include "log.h"

#include "mbed.h"
#include "Hexi_OLED_SSD1351.h"

SSD1351 g_oled(PTB22, PTB21, PTC13, PTB20, PTE6, PTD15);

Mutex g_sensors_lock;
Sensor_Collector g_sens_collector;
Queue<Label, 5> g_labels_queue;

void sensor_collect_thread_loop()
{
    Timer timer;

    while (true)
    {
        timer.reset();
        timer.start();

        // Enter critical section
        g_sensors_lock.lock();
        g_sens_collector.collect();
        timer.stop();
        g_sensors_lock.unlock();
        // Exit critical section

        log_info("Got %d samples in %dms!\n", SAMPLE_MATRIX_ELEMENT_SIZE, timer.read_ms());
    }
}

void prediction_thread_loop()
{
    Sample_Matrix *local_samples;
    std::unique_ptr<Label_Predictor> predictor(new Label_Predictor);
    Tflite_Error status;

    status = predictor->init();
    if (status != Tflite_Error::OK)
    {
        log_error("%s\n", tflite_error_to_cstr(status));
        return;
    }

    while (true)
    {
        // Enter critical section
        // Copy the data to a local memory so the critical
        // section last as short as possible
        g_sensors_lock.lock();
        local_samples = new Sample_Matrix(*g_sens_collector.getSamples());
        g_sensors_lock.unlock();
        // Exit critical section

        Label label;
        status = predictor->predict(local_samples, 0, &label);
        log_error("%s\n", tflite_error_to_cstr(status));

        delete local_samples;

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
            log_info("\nGot label %s\n\n", label_to_cstr(label));
            stat.new_event(label);
        }
    }
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

/*
int main1()
{
    DigitalOut status_led(LED_GREEN);
    FXOS8700 acc(PTC11, PTC10);
    FXAS21002 gyr(PTC11, PTC10);

    // acc.accel_config();
    I2C gyro(PTC11, PTC10);
    char d[2];
    d[0] = 0x13;
    d[1] = 0x08;
    gyro.write(0x40, d, 2);
    d[0] = 0x0D;
    d[1] = 0x00;
    gyro.write(0x40, d, 2);
    d[0] = 0x13;
    d[1] = 0x0A;
    gyro.write(0x40, d, 2);

    gyr.gyro_config();
    float ad[3];
    float gd[3];

    while (true)
    {
        char db[7];
        char c[1];
        c[0] = 0x00;
        gyro.write(0x40, c, 1, true);
        gyro.read(0x40, db, 7);
        // gd[0] = (float)((int16_t)((db[1] * 250) + (db[2]))) * 0.0625;
        // gd[1] = (float)((int16_t)((db[3] * 250) + (db[4]))) * 0.0625;
        // gd[2] = (float)((int16_t)((db[5] * 250) + (db[6]))) * 0.0625;

        //log("- %f %f %f\n", gd[0], gd[1], gd[2]);

#define DTR 3.141592653589793 / 180

        gd[0] = (int16_t)((db[1] << 8) | db[2]) * 0.0625 * DTR;
        gd[1] = (int16_t)((db[3] << 8) | db[4]) * 0.0625 * DTR;
        gd[2] = (int16_t)((db[5] << 8) | db[6]) * 0.0625 * DTR;

        log_info("+ %f %f %f\n", gd[0], gd[1], gd[2]);
        //acc.acquire_accel_data_g(ad);
        //gyr.acquire_gyro_data_dps(gd);

        //log("%d %d %d %d %d %d %d\n", db[0],db[1],db[2],db[3],db[4],db[5],db[6]);
        //log("%f %f %f %f %f %f\n", ad[0], ad[1], ad[2], gd[0], gd[1], gd[2]);

        status_led = !status_led;
        wait_ms(1000);
    }
}


int main2()
{
    oled_text_properties_t txt_prop = {0};
    g_oled.GetTextProperties(&txt_prop);
    g_oled.DimScreenOFF();
    g_oled.FillScreen(COLOR_BLACK);

    //main1();
    Thread t;
    t.start(sensor_collect_thread_loop);
    Thread t2;
    t2.start(prediction_thread_loop);

    // Sample_Matrix m1;
    // m1._ax[0] = 1;
    // m1._ax[1] = 2;
    // m1._ax[2] = 3;
    // m1._ax[3] = 4;
    // m1._ax[4] = 5;
    // Sample_Matrix *m2 = new Sample_Matrix(m1);

    // for (int i = 0; i < 10; ++i)
    // {
    //     m2->_ax[i]+=10;
    //     log_info("%f %f\n", m1._ax[i], m2->_ax[i]);
    // }
    // log_info("ciao %d %d\n", sizeof(m1), sizeof(uintptr_t));

    DigitalOut status_led(LED_RED);
    while (true)
    {
        status_led = !status_led;
        wait_ms(500);
    }

    return 0;
}
*/