#include "collection/collector_thread.h"
#include "common/global_thread_vars.h"
#include "prediction/predictor_thread.h"
#include "ui/display_thread.h"

#include "FATFileSystem.h"
#include "LittleFileSystem.h"
#include "mbed.h"

// Override default console for enabling printfs
FileHandle *mbed::mbed_override_console(int fd)
{
    static BufferedSerial serial_out(USBTX, USBRX, 9600);
    return &serial_out;
}

// Define default filesystem
FileSystem *FileSystem::get_default_instance()
{
    static LittleFileSystem fs("fs");
    return &fs;
}

// Define global thread variables declared in global_thread_vars.h
Mutex g_sensors_lock;
RawSensorData g_raw_sensor_data;
Queue<Label, 5> g_labels_queue;
Config g_config;

int main()
{
    Thread sensor_thread;
    Thread prediction_thread;
    Thread display_thread;

    g_config.init();
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