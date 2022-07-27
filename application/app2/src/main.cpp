// #define MEASURE_COLLECTION
#define MEASURE_INFERENCE

#ifdef MEASURE_COLLECTION
#include "collector_thread.h"
#endif // MEASURE_COLLECTION
#ifdef MEASURE_INFERENCE
#include "prediction_thread.h"
#endif // MEASURE_INFERENCE

#include "FATFileSystem.h"
#include "mbed.h"

// Override default console for enabling printf
FileHandle *mbed::mbed_override_console(int fd)
{
    static BufferedSerial serial_out(USBTX, USBRX, 9600);
    return &serial_out;
}

// Define default filesystem
FileSystem *FileSystem::get_default_instance()
{
    static FATFileSystem fs("sd");
    return &fs;
}

int main()
{
#ifdef MEASURE_COLLECTION
    Thread collection_thread;
    collection_thread.start(collector_thread_loop);
#endif // MEASURE_COLLECTION

#ifdef MEASURE_INFERENCE
    Thread prediction_thread;
    prediction_thread.start(prediction_thread_loop);
#endif // MEASURE_INFERENCE

    DigitalOut status_led(LED_RED);
    while (true)
    {
        status_led = !status_led;
        ThisThread::sleep_for(500ms);
    }

    return 0;
}