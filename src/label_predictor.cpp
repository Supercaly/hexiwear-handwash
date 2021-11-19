#include "label_predictor.h"

Label_Predictor::Label_Predictor() : _model(nullptr),
                                     _error_reporter(nullptr),
                                     _interpreter(nullptr),
                                     _input(nullptr),
                                     _output(nullptr)
{
}

Label_Predictor::Label_Predictor(const tflite::Model *model,
                                 tflite::ErrorReporter *error_reporter,
                                 tflite::MicroInterpreter *interpreter) : _model(model),
                                                                          _error_reporter(error_reporter),
                                                                          _interpreter(interpreter)
{
    _input = _interpreter->input(0);
    _output = _interpreter->output(0);
}

Label_Predictor::~Label_Predictor()
{
}

Label Label_Predictor::predict(float *ax, float *ay, float *az, float *gx, float *gy, float *gz, float hand)
{
    Serial pc(USBTX, USBRX);
    pc.printf("\ndata:    %f %f %f %f %f %f %f\n",ax[0],ay[0],az[0],gx[0],gy[0],gz[0],hand);
    pc.printf("data:    %f %f %f %f %f %f %f\n",ax[1],ay[1],az[1],gx[1],gy[1],gz[1],hand);
    pc.printf("data:    %f %f %f %f %f %f %f\n",ax[2],ay[2],az[2],gx[2],gy[2],gz[2],hand);
    pc.printf("data:    %f %f %f %f %f %f %f\n",ax[3],ay[3],az[3],gx[3],gy[3],gz[3],hand);
    pc.printf("data:    %f %f %f %f %f %f %f\n",ax[4],ay[4],az[4],gx[4],gy[4],gz[4],hand);

    float sum_ax = 0.0, sum_ay = 0.0, sum_az = 0.0, sum_gx = 0.0, sum_gy = 0.0, sum_gz = 0.0;
    float std_ax = 0.0, std_ay = 0.0, std_az = 0.0, std_gx = 0.0, std_gy = 0.0, std_gz = 0.0;
    float max_ax = 0.0, max_ay = 0.0, max_az = 0.0, max_gx = 0.0, max_gy = 0.0, max_gz = 0.0;
    float min_ax = 0.0, min_ay = 0.0, min_az = 0.0, min_gx = 0.0, min_gy = 0.0, min_gz = 0.0;

    // NOTE: This works only because the overlap is 0.5 so the prev data
    // and the new have the same size. For different overlap you need two for loops.
    for (int i = 0; i < SENSORS_DATA_CAPACITY; ++i)
    {
        // Compute the sum of every component for the avg
        sum_ax += ax[i];
        sum_ay += ay[i];
        sum_az += az[i];
        sum_gx += gx[i];
        sum_gy += gy[i];
        sum_gz += gz[i];

        // compute the max
        if (ax[i] > max_ax)
            max_ax = ax[i];
        if (ay[i] > max_ay)
            max_ay = ay[i];
        if (az[i] > max_az)
            max_az = az[i];
        if (gx[i] > max_gx)
            max_gx = gx[i];
        if (gy[i] > max_gy)
            max_gy = gy[i];
        if (gz[i] > max_gz)
            max_gz = gz[i];

        // compute the min
        if (ax[i] < max_ax)
            max_ax = ax[i];
        if (ay[i] < max_ay)
            max_ay = ay[i];
        if (az[i] < max_az)
            max_az = az[i];
        if (gx[i] < max_gx)
            max_gx = gx[i];
        if (gy[i] < max_gy)
            max_gy = gy[i];
        if (gz[i] < max_gz)
            max_gz = gz[i];
    }

    float avg_ax = sum_ax / SENSORS_DATA_CAPACITY,
          avg_ay = sum_ay / SENSORS_DATA_CAPACITY,
          avg_az = sum_az / SENSORS_DATA_CAPACITY,
          avg_gx = sum_gx / SENSORS_DATA_CAPACITY,
          avg_gy = sum_gy / SENSORS_DATA_CAPACITY,
          avg_gz = sum_gz / SENSORS_DATA_CAPACITY;

    sum_ax = 0.0;
    sum_ay = 0.0;
    sum_az = 0.0;
    sum_gx = 0.0;
    sum_gy = 0.0;
    sum_gz = 0.0;
    for (int i = 0; i < SENSORS_DATA_CAPACITY; ++i)
    {
        // Compute the standard deviation sum for each component
        sum_ax += (ax[i] - avg_ax) * (ax[i] - avg_ax);
        sum_ay += (ay[i] - avg_ay) * (ay[i] - avg_ay);
        sum_az += (az[i] - avg_az) * (az[i] - avg_az);
        sum_gx += (gx[i] - avg_gx) * (gx[i] - avg_gx);
        sum_gy += (gy[i] - avg_gy) * (gy[i] - avg_gy);
        sum_gz += (gz[i] - avg_gz) * (gz[i] - avg_gz);
    }

    std_ax = sqrt(sum_ax / (SENSORS_DATA_CAPACITY - 1));
    std_ay = sqrt(sum_ay / (SENSORS_DATA_CAPACITY - 1));
    std_az = sqrt(sum_az / (SENSORS_DATA_CAPACITY - 1));
    std_gx = sqrt(sum_gx / (SENSORS_DATA_CAPACITY - 1));
    std_gy = sqrt(sum_gy / (SENSORS_DATA_CAPACITY - 1));
    std_gz = sqrt(sum_gz / (SENSORS_DATA_CAPACITY - 1));

    // Serial a(USBTX, USBRX);
    // a.printf("avg: %f %f %f %f %f %f, std: %f %f %f %f %f %f, max: %f %f %f %f %f %f, min: %f %f %f %f %f %f, hand: %d",
    //          avg_ax, avg_ay, avg_az, avg_gx, avg_gy, avg_gz,
    //          std_ax, std_ay, std_az, std_gx, std_gy, std_gz,
    //          max_ax, max_ay, max_az, max_gx, max_gy, max_gz,
    //          min_ax, min_ay, min_az, min_gx, min_gy, min_gz,
    //          hand);

    // float data[25] = {
    //     avg_ax,
    //     avg_ay,
    //     avg_az,
    //     avg_gx,
    //     avg_gy,
    //     avg_gz,
    //     std_ax,
    //     std_ay,
    //     std_az,
    //     std_gx,
    //     std_gy,
    //     std_gz,
    //     max_ax,
    //     max_ay,
    //     max_az,
    //     max_gx,
    //     max_gy,
    //     max_gz,
    //     min_ax,
    //     min_ay,
    //     min_az,
    //     min_gx,
    //     min_gy,
    //     min_gz,
    //     hand,
    // };
    _input->data.f[0] = avg_ax;
    _input->data.f[1] = avg_ay;
    _input->data.f[2] = avg_az;
    _input->data.f[3] = avg_gx;
    _input->data.f[4] = avg_gy;
    _input->data.f[5] = avg_gz;
    _input->data.f[6] = std_ax;
    _input->data.f[7] = std_ay;
    _input->data.f[8] = std_az;
    _input->data.f[9] = std_gx;
    _input->data.f[10] = std_gy;
    _input->data.f[11] = std_gz;
    _input->data.f[12] = max_ax;
    _input->data.f[13] = max_ay;
    _input->data.f[14] = max_az;
    _input->data.f[15] = max_gx;
    _input->data.f[16] = max_gy;
    _input->data.f[17] = max_gz;
    _input->data.f[18] = min_ax;
    _input->data.f[19] = min_ay;
    _input->data.f[20] = min_az;
    _input->data.f[21] = min_gx;
    _input->data.f[22] = min_gy;
    _input->data.f[23] = min_gz;
    _input->data.f[24] = hand;

    TfLiteStatus invoke_status = _interpreter->Invoke();
    if (invoke_status != kTfLiteOk)
    {
        Serial ps(USBTX, USBRX);
        ps.printf("Error invoking model\n");
        return LABEL_NONE;
    }

    return action_to_label();
}

Label Label_Predictor::action_to_label()
{
    float *data = tflite::GetTensorData<float>(_output);
    float max_value = data[0];
    int max_index = 0;

    Serial a(USBTX, USBRX);
    a.printf("\nResults: ");
    for (int i = 0; i < 3; ++i)
    {
        float value = data[i];
        a.printf("%d:%f ", i, value);
        if (value > max_value)
        {
            max_value = value;
            max_index = i;
        }
    }
    a.printf("\n");
    return Label(max_index);
}