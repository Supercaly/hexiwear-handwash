#include "data_importer.h"

#include "FATFileSystem.h"
#include "label.h"
#include "mbed.h"
#include "predictor.h"
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
    Predictor p;
    RawSensorData *d = (RawSensorData *)calloc(1, sizeof(RawSensorData));
    Label l;
    DataImporter i;

    printf("init %d\n", i.init());
    printf("init %d\n", p.init());
    while (i.next_chunk(d, &l))
    {
        printf("label %d\n", l);
        for (int i = 0; i < 5; i++)
        {
            printf("%f %f %f %f %f %f\n",
                   d->ax[i], d->ay[i], d->az[i], d->gx[i], d->gy[i], d->gz[i]);
        }
        Label lab;
        p.predict(d, 0, &lab);
        printf("pred lab %s\n", wrist_to_string(lab))
    }

    DigitalOut status_led(LED_RED);
    while (true)
    {
        status_led = !status_led;
        ThisThread::sleep_for(500ms);
    }

    return 0;
}