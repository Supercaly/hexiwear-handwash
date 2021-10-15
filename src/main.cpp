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

Mlp_hw_model model;
void prediction_thread_loop()
{
    float *ax = (float *)calloc(SENSORS_DATA_CAPACITY, sizeof(float));
    float *ay = (float *)calloc(SENSORS_DATA_CAPACITY, sizeof(float));
    float *az = (float *)calloc(SENSORS_DATA_CAPACITY, sizeof(float));
    float *gx = (float *)calloc(SENSORS_DATA_CAPACITY, sizeof(float));
    float *gy = (float *)calloc(SENSORS_DATA_CAPACITY, sizeof(float));
    float *gz = (float *)calloc(SENSORS_DATA_CAPACITY, sizeof(float));
    if (gz == NULL)
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
        for (int i = 0; i < SENSORS_DATA_CAPACITY; ++i)
        {
            ax[i] = g_sensors.data[i].ax;
            ay[i] = g_sensors.data[i].ay;
            az[i] = g_sensors.data[i].az;
            gx[i] = g_sensors.data[i].gx;
            gy[i] = g_sensors.data[i].gy;
            gz[i] = g_sensors.data[i].gz;
        }
        g_sensors_lock.unlock();
        // Exit critical section

        Label label = p.predict(ax, ay, az, gx, gy, gz, 0);
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
    float datas[4][25] = {
        {4.7816847826086954, 4.255673913043478, 6.804749999999999, -4.866524390243903, -12.64298780487805, 1.8560975609756099, 5.042959792416002, 3.6051656054565338, 4.689261124302095, 73.98725569808039, 146.18617668440422, 104.77332013971174, 24.467391304347824, 18.119565217391305, 20.26086956521739, 513.9024390243903, 724.7560975609757, 1998.048780487805, -19.73913043478261, -5.83695652173913, -19.032608695652172, -1070.1219512195123, -826.5243902439025, -659.329268292683, 0},
        {3.9045, 3.041391304347826, 10.307032608695652, 1.2215853658536586, 6.238658536585366, 2.6151219512195123, 2.3108993241194034, 0.4394577086695477, 1.6974184194267288, 5.896054178318922, 24.823116892708953, 13.001777112226305, 12.445652173913043, 4.445652173913043, 12.16304347826087, 37.01219512195122, 144.93902439024393, 41.28048780487805, 2.2065217391304346, 0.6195652173913043, 3.6739130434782608, -17.073170731707318, -48.963414634146346, -61.28048780487806, 1},
        {7.733739130434782, 3.080195652173913, 5.739184782608696, -2.1140853658536587, -2.292804878048781, -4.111463414634147, 3.6155270216581137, 1.7111971180892396, 4.88651139946025, 40.47706692228569, 105.71085303701174, 28.802850652199393, 18.902173913043477, 7.75, 12.282608695652174, 118.59756097560977, 463.4146341463415, 114.81707317073172, -0.358695652173913, -2.4456521739130435, -6.717391304347826, -181.64634146341464, -424.32926829268297, -134.87804878048783, 1},
        {9.94104347826087, 2.6059891304347826, 3.5058260869565214, -3.655304878048781, -4.376280487804879, -0.4609756097560976, 2.3393364378493318, 2.0882401045686403, 3.5653870741855624, 34.72797464150248, 115.97925823579385, 26.23924135527785, 16.967391304347824, 9.184782608695652, 12.76086956521739, 84.75609756097562, 382.9878048780488, 67.07317073170732, -0.8478260869565217, -5.326086956521739, -5.25, -113.41463414634147, -499.20731707317077, -104.69512195121952, 1},
    };
    // exp 0
    uTensor::Tensor o = new uTensor::RamTensor({1, 3}, flt);
    for (int i = 0; i < 4; ++i)
    {
        uTensor::Tensor in = new uTensor::RomTensor({25, 1}, flt, datas[i]);
        model.set_inputs({{Mlp_hw_model::input_0, in}})
            .set_outputs({{Mlp_hw_model::output_0, o}})
            .eval();

        for (uint32_t j = 0; j < (*o)->num_elems(); ++j)
        {
            log("%f ", static_cast<float>(o(j)));
        }
        log("\n");
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