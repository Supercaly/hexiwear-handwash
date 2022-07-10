#include "data_importer.h"

#include "FATFileSystem.h"
#include "label.h"
#include "mbed.h"
#include "raw_sensor_data.h"

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
    RawSensorData *d = (RawSensorData *)calloc(1, sizeof(RawSensorData));
    Label l;
    DataImporter i;
    printf("init %d\n", i.init());
    i.next_chunk(d, &l);

    DigitalOut status_led(LED_RED);
    while (true)
    {
        status_led = !status_led;
        ThisThread::sleep_for(500ms);
    }

    return 0;
}