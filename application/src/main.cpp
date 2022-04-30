#include "collection/collector_thread.h"
#include "common/global_thread_vars.h"
#include "prediction/predictor_thread.h"
#include "ui/display_thread.h"

#include "mbed.h"
#include "FATFileSystem.h"

// Override default console for enabling printfs
FileHandle *mbed::mbed_override_console(int fd)
{
    static BufferedSerial serial_out(USBTX, USBRX, 9600);
    return &serial_out;
}

// Define default filesystem to be the sd card
FileSystem *FileSystem::get_default_instance()
{
    static FATFileSystem fs("sd");
    return &fs;
}

// Define global thread variables declared in global_thread_vars.h
Mutex g_sensors_lock;
RawSensorData g_raw_sensor_data;
Queue<Label, 5> g_labels_queue;
DataRecorder g_data_recorder;

int main()
{
    Thread sensor_thread;
    Thread prediction_thread;
    Thread display_thread;
    sensor_thread.start(collector_thread_loop);
    prediction_thread.start(predictor_thread_loop);
    display_thread.start(display_thread_loop);

    DigitalOut status_led(LED_RED);
    while (true)
    {
        status_led = !status_led;
        ThisThread::sleep_for(500ms);
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