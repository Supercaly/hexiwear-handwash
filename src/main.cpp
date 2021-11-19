#include "mbed.h"
#include "Hexi_OLED_SSD1351.h"
#include "stat_display.h"
#include "sensors.h"
#include "label.h"
#include "label_predictor.h"
#include "log.h"
#include "handwash_model.h"

#include <string.h>

#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"

SSD1351 g_oled(PTB22, PTB21, PTC13, PTB20, PTE6, PTD15);
Queue<Label, 5> g_labels_queue;
Sensors g_sensors;
Mutex g_sensors_lock;
constexpr int k_tensor_arena_size = 3000;
uint8_t k_tensor_arena[k_tensor_arena_size];

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

void prediction_thread_loop()
{
    tflite::InitializeTarget();
    static tflite::MicroErrorReporter static_error_reporter;

    const tflite::Model *model = tflite::GetModel(g_handwash_model_data);
    if (model->version() != TFLITE_SCHEMA_VERSION)
    {
        error("Provided model has schema version %d, "
              "but supported version is %d.",
              model->version(), TFLITE_SCHEMA_VERSION);
        return;
    }
    // TODO: Replace all ops resolver
    static tflite::AllOpsResolver static_ops_resolver;
    static tflite::MicroInterpreter static_interpreter(
        model, static_ops_resolver, k_tensor_arena, k_tensor_arena_size, &static_error_reporter);
    TfLiteStatus allocate_status = static_interpreter.AllocateTensors();
    if (allocate_status != kTfLiteOk)
    {
        error("Tensor allocation failed");
        return;
    }

    // Allocate memory for the input arrays
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

    Label_Predictor p(model, &static_error_reporter, &static_interpreter);

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
    oled_text_properties_t txt_prop = {0};
    g_oled.GetTextProperties(&txt_prop);
    g_oled.DimScreenOFF();
    g_oled.FillScreen(COLOR_BLACK);

    float data_array[12][25] = {
        {4.604608695652174, 2.716445652173913, 9.86191304347826, 2.117987804878049, 1.1571341463414635, 1.476768292682927, 3.071854119577681, 1.007738661118869, 2.046022480935033, 14.786542454511999, 54.70467364055721, 24.084149110612056, 21.663043478260867, 9.282608695652174, 11.771739130434783, 68.78048780487805, 451.70731707317077, 86.89024390243904, -7.076086956521739, 1.065217391304348, 0.9239130434782609, -71.58536585365854, -212.07317073170734, -113.47560975609757, 1.0},
        {10.038576086956521, 2.243065217391304, 2.8115108695652173, -3.492378048780488, 0.07591463414634174, -0.668719512195122, 2.2182529748676303, 1.6413887215680136, 3.914385150523728, 41.99496616060681, 114.20262735463373, 26.08170459291278, 17.26086956521739, 8.184782608695652, 11.17391304347826, 160.6707317073171, 334.51219512195127, 69.75609756097562, 2.3043478260869565, -2.9239130434782608, -4.913043478260869, -163.96341463414635, -487.74390243902445, -89.81707317073172, 1.0},
        {9.413217391304348, 5.67525, 0.1534782608695652, -1.4156097560975611, -7.621585365853659, -2.8557317073170734, 1.452780383628157, 1.6715107638180637, 2.9892869627103584, 30.861920015871004, 83.34010334655885, 24.920372394519287, 14.0, 11.391304347826086, 9.032608695652174, 104.93902439024392, 339.1463414634147, 69.81707317073172, 3.467391304347826, 1.4565217391304348, -4.83695652173913, -125.97560975609757, -369.26829268292687, -105.67073170731709, 1.0},
        {8.300673913043479, 3.94070652173913, 5.865739130434783, -0.06085365853658537, 10.705365853658538, -0.5234146341463415, 3.0035868605436304, 2.097626310629582, 3.8566941286403855, 24.83186445053912, 79.49248054132242, 22.837474184892834, 24.043478260869566, 10.815217391304348, 11.478260869565217, 119.08536585365854, 524.0243902439025, 95.85365853658537, -1.0108695652173914, -0.6304347826086957, -4.619565217391305, -90.54878048780489, -189.39024390243904, -83.78048780487805, 1.0},
        {3.789782608695652, 0.8452826086956522, 10.646858695652174, -1.5215243902439026, 0.2859146341463415, -0.5118292682926829, 0.7147918061869319, 1.4728381916635496, 0.47489959661197007, 32.55175621594281, 15.303112903233245, 12.503071666083757, 11.532608695652174, 10.304347826086957, 14.597826086956522, 524.7560975609756, 83.90243902439025, 151.03658536585368, -1.608695652173913, -7.978260869565217, 1.7391304347826086, -246.28048780487808, -190.12195121951223, -69.69512195121952, 1.0},
        {2.4924673913043476, 1.5259673913043479, 10.683097826086957, -0.15621951219512198, -6.950000000000001, -1.1244512195121952, 2.479828750498529, 0.5157781451649657, 0.9065028569901165, 4.1793245234950875, 23.410164820000492, 9.096084267806559, 10.695652173913043, 2.369565217391304, 11.804347826086957, 22.926829268292686, 16.463414634146343, 30.54878048780488, 0.05434782608695652, -2.032608695652174, 6.815217391304348, -18.719512195121954, -204.69512195121953, -42.987804878048784, 1.0},
        {7.702989130434783, -0.8136739130434782, 6.054423913043478, 1.6759756097560976, -1.180670731707317, 2.323475609756098, 2.8363381110023447, 2.497013706285479, 4.813311854994268, 48.852680784175256, 79.64828851122938, 30.76389758139261, 16.902173913043477, 8.728260869565217, 20.793478260869566, 165.85365853658539, 415.06097560975616, 154.57317073170734, -7.804347826086956, -9.48913043478261, -8.597826086956522, -164.14634146341464, -293.7804878048781, -128.109756097561, 1.0},
        {3.83525, -1.3644565217391305, 8.115586956521739, 0.4261585365853658, 4.255243902439025, -0.4998780487804879, 4.302100267231492, 2.3520610347615487, 3.4584011167740196, 38.3286909763806, 109.94531780948869, 24.60887771645217, 19.641304347826086, 3.891304347826087, 17.065217391304348, 120.3658536585366, 309.75609756097566, 74.75609756097562, -6.315217391304348, -7.83695652173913, -3.2065217391304346, -157.7439024390244, -425.24390243902445, -93.10975609756099, 1.0},
        {1.2774673913043477, 1.0451521739130434, 10.504358695652174, 2.0751219512195123, -1.1538414634146343, -4.941341463414634, 2.7895693670970925, 0.608847015714849, 1.0640044302613154, 7.747544071043902, 40.86267325005233, 23.862211650814082, 14.119565217391305, 3.5652173913043477, 11.347826086956522, 51.03658536585366, 185.1219512195122, 59.146341463414636, -1.4456521739130435, -2.293478260869565, 4.260869565217392, -19.085365853658537, -213.90243902439028, -113.71951219512196, 1.0},
        {6.994923913043478, -1.288945652173913, 4.345945652173913, -2.9913414634146345, -9.543597560975611, 1.5534756097560978, 3.8262140718777817, 2.0431467019017027, 5.401283132507201, 36.7979320780455, 125.58064692239657, 37.10996246578995, 18.51086956521739, 3.8478260869565215, 16.51086956521739, 131.64634146341464, 409.20731707317077, 162.07317073170734, -7.010869565217391, -6.141304347826087, -6.869565217391304, -151.15853658536588, -611.7073170731708, -151.28048780487805, 1.0},
        {5.00220652173913, -1.6197608695652173, 7.224695652173913, 0.12097560975609756, 8.324390243902439, 0.9748170731707319, 4.269874939191947, 2.2164353705061304, 4.115021337096996, 40.2455110805882, 108.83126408243739, 25.118494028272195, 16.32608695652174, 5.173913043478261, 18.380434782608695, 145.7926829268293, 437.07317073170736, 107.25609756097562, -7.33695652173913, -7.554347826086956, -6.010869565217392, -174.75609756097563, -546.6463414634147, -111.89024390243904, 1.0},
        {1.8489239130434783, 0.7077934782608696, 10.456586956521738, 2.3781707317073173, -0.17262195121951213, -4.7630487804878054, 2.8505332587450654, 0.6877759874025844, 1.1118368226300444, 8.873569957927312, 39.199376732225474, 33.048675588165985, 11.16304347826087, 2.641304347826087, 12.282608695652174, 46.646341463414636, 158.41463414634148, 50.0609756097561, -1.8152173913043477, -1.8043478260869565, 6.010869565217392, -31.09756097560976, -262.8048780487805, -204.69512195121953, 1.0},
    };
    int correct_result_array[12] = {0, 1, 1, 1, 0, 2, 2, 2, 0, 2, 2, 0};

    const tflite::Model *model = nullptr;
    tflite::ErrorReporter *error_reporter = nullptr;
    tflite::MicroInterpreter *interpreter = nullptr;
    TfLiteTensor *input = nullptr;
    TfLiteTensor *output = nullptr;

    // TODO: Put tensor arena size in a config header
    constexpr int kTensorArenaSize = 3000;
    uint8_t tensor_arena[kTensorArenaSize];

    tflite::InitializeTarget();
    static tflite::MicroErrorReporter micro_error_reporter;
    error_reporter = &micro_error_reporter;

    model = tflite::GetModel(g_handwash_model_data);
    if (model->version() != TFLITE_SCHEMA_VERSION)
    {
        TF_LITE_REPORT_ERROR(error_reporter,
                             "Model provided is schema version %d not equal "
                             "to supported version %d.",
                             model->version(), TFLITE_SCHEMA_VERSION);
        return 0;
    }

    static tflite::AllOpsResolver resolver;

    static tflite::MicroInterpreter static_interpreter(
        model, resolver, tensor_arena, kTensorArenaSize, error_reporter);
    interpreter = &static_interpreter;

    TfLiteStatus allocate_status = interpreter->AllocateTensors();
    if (allocate_status != kTfLiteOk)
    {
        TF_LITE_REPORT_ERROR(error_reporter, "AllocateTensors() failed");
        return 0;
    }

    input = interpreter->input(0);
    output = interpreter->output(0);

    log("name: %s\n", input->name);
    log("name: %s\n", output->name);
    log("bytes: %d\n", input->bytes);
    log("bytes: %d\n", output->bytes);
    tflite::RuntimeShape in_dim = tflite::GetTensorShape(input);
    log("in shape: {%dx%d}\n", in_dim.Dims(0), in_dim.Dims(1));
    tflite::RuntimeShape out_dim = tflite::GetTensorShape(output);
    log("out shape: {%dx%d}\n", out_dim.Dims(0), out_dim.Dims(1));

    for (int i = 0; i < 12; i++)
    {
        input->data.f[0] = data_array[i][0];
        input->data.f[1] = data_array[i][1];
        input->data.f[2] = data_array[i][2];
        input->data.f[3] = data_array[i][3];
        input->data.f[4] = data_array[i][4];
        input->data.f[5] = data_array[i][5];
        input->data.f[6] = data_array[i][6];
        input->data.f[7] = data_array[i][7];
        input->data.f[8] = data_array[i][8];
        input->data.f[9] = data_array[i][9];
        input->data.f[10] = data_array[i][10];
        input->data.f[11] = data_array[i][11];
        input->data.f[12] = data_array[i][12];
        input->data.f[13] = data_array[i][13];
        input->data.f[14] = data_array[i][14];
        input->data.f[15] = data_array[i][15];
        input->data.f[16] = data_array[i][16];
        input->data.f[17] = data_array[i][17];
        input->data.f[18] = data_array[i][18];
        input->data.f[19] = data_array[i][19];
        input->data.f[20] = data_array[i][20];
        input->data.f[21] = data_array[i][21];
        input->data.f[22] = data_array[i][22];
        input->data.f[23] = data_array[i][23];
        input->data.f[24] = data_array[i][24];
        TfLiteStatus s = interpreter->Invoke();
        if (s != kTfLiteOk)
        {
            TF_LITE_REPORT_ERROR(error_reporter, "Invoke %d failed\n", i);
            return 0;
        }

        float *res = tflite::GetTensorData<float>(output);
        log("%d) got [%f,%f,%f] expected [%d]\n",
            i, res[0], res[1], res[2], correct_result_array[i]);
    }
    return 0;
}

int main3()
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

        log("+ %f %f %f\n", gd[0], gd[1], gd[2]);
        //acc.acquire_accel_data_g(ad);
        //gyr.acquire_gyro_data_dps(gd);

        //log("%d %d %d %d %d %d %d\n", db[0],db[1],db[2],db[3],db[4],db[5],db[6]);
        //log("%f %f %f %f %f %f\n", ad[0], ad[1], ad[2], gd[0], gd[1], gd[2]);

        status_led = !status_led;
        wait_ms(1000);
    }
}

int main()
{
    oled_text_properties_t txt_prop = {0};
    g_oled.GetTextProperties(&txt_prop);

    g_oled.DimScreenOFF();
    g_oled.FillScreen(COLOR_BLACK);
    main1();
    return 0;
}