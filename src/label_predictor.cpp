#include "label_predictor.h"
#include "handwash_model.h"

#include "mbed.h"

Label_Predictor::Label_Predictor() {}

Label_Predictor::~Label_Predictor() {}

Tflite_Error Label_Predictor::init()
{
    return _ml.init(g_handwash_model_data);
}

Tflite_Error Label_Predictor::predict(Sample_Matrix sample, float hand, Label *label)
{
    log_info("data:    %f %f %f %f %f %f %f\n", sample._ax[0], sample._ay[0], sample._az[0], sample._gx[0], sample._gy[0], sample._gz[0], hand);
    log_info("data:    %f %f %f %f %f %f %f\n", sample._ax[1], sample._ay[1], sample._az[1], sample._gx[1], sample._gy[1], sample._gz[1], hand);
    log_info("data:    %f %f %f %f %f %f %f\n", sample._ax[2], sample._ay[2], sample._az[2], sample._gx[2], sample._gy[2], sample._gz[2], hand);
    log_info("data:    %f %f %f %f %f %f %f\n", sample._ax[3], sample._ay[3], sample._az[3], sample._gx[3], sample._gy[3], sample._gz[3], hand);
    log_info("data:    %f %f %f %f %f %f %f\n", sample._ax[4], sample._ay[4], sample._az[4], sample._gx[4], sample._gy[4], sample._gz[4], hand);

    float sum_ax = 0.0, sum_ay = 0.0, sum_az = 0.0, sum_gx = 0.0, sum_gy = 0.0, sum_gz = 0.0;
    float std_ax = 0.0, std_ay = 0.0, std_az = 0.0, std_gx = 0.0, std_gy = 0.0, std_gz = 0.0;
    float max_ax = 0.0, max_ay = 0.0, max_az = 0.0, max_gx = 0.0, max_gy = 0.0, max_gz = 0.0;
    float min_ax = 0.0, min_ay = 0.0, min_az = 0.0, min_gx = 0.0, min_gy = 0.0, min_gz = 0.0;

    // NOTE: This works only because the overlap is 0.5 so the prev data
    // and the new have the same size. For different overlap you need two for loops.
    for (int i = 0; i < SAMPLE_MATRIX_ELEMENT_SIZE; ++i)
    {
        // Compute the sum of every component for the avg
        sum_ax += sample._ax[i];
        sum_ay += sample._ay[i];
        sum_az += sample._az[i];
        sum_gx += sample._gx[i];
        sum_gy += sample._gy[i];
        sum_gz += sample._gz[i];

        // compute the max
        if (sample._ax[i] > max_ax)
            max_ax = sample._ax[i];
        if (sample._ay[i] > max_ay)
            max_ay = sample._ay[i];
        if (sample._az[i] > max_az)
            max_az = sample._az[i];
        if (sample._gx[i] > max_gx)
            max_gx = sample._gx[i];
        if (sample._gy[i] > max_gy)
            max_gy = sample._gy[i];
        if (sample._gz[i] > max_gz)
            max_gz = sample._gz[i];

        // compute the min
        if (sample._ax[i] < max_ax)
            max_ax = sample._ax[i];
        if (sample._ay[i] < max_ay)
            max_ay = sample._ay[i];
        if (sample._az[i] < max_az)
            max_az = sample._az[i];
        if (sample._gx[i] < max_gx)
            max_gx = sample._gx[i];
        if (sample._gy[i] < max_gy)
            max_gy = sample._gy[i];
        if (sample._gz[i] < max_gz)
            max_gz = sample._gz[i];
    }

    float avg_ax = sum_ax / SAMPLE_MATRIX_ELEMENT_SIZE,
          avg_ay = sum_ay / SAMPLE_MATRIX_ELEMENT_SIZE,
          avg_az = sum_az / SAMPLE_MATRIX_ELEMENT_SIZE,
          avg_gx = sum_gx / SAMPLE_MATRIX_ELEMENT_SIZE,
          avg_gy = sum_gy / SAMPLE_MATRIX_ELEMENT_SIZE,
          avg_gz = sum_gz / SAMPLE_MATRIX_ELEMENT_SIZE;

    sum_ax = 0.0;
    sum_ay = 0.0;
    sum_az = 0.0;
    sum_gx = 0.0;
    sum_gy = 0.0;
    sum_gz = 0.0;
    for (int i = 0; i < SAMPLE_MATRIX_ELEMENT_SIZE; ++i)
    {
        // Compute the standard deviation sum for each component
        sum_ax += (sample._ax[i] - avg_ax) * (sample._ax[i] - avg_ax);
        sum_ay += (sample._ay[i] - avg_ay) * (sample._ay[i] - avg_ay);
        sum_az += (sample._az[i] - avg_az) * (sample._az[i] - avg_az);
        sum_gx += (sample._gx[i] - avg_gx) * (sample._gx[i] - avg_gx);
        sum_gy += (sample._gy[i] - avg_gy) * (sample._gy[i] - avg_gy);
        sum_gz += (sample._gz[i] - avg_gz) * (sample._gz[i] - avg_gz);
    }

    std_ax = sqrt(sum_ax / (SAMPLE_MATRIX_ELEMENT_SIZE - 1));
    std_ay = sqrt(sum_ay / (SAMPLE_MATRIX_ELEMENT_SIZE - 1));
    std_az = sqrt(sum_az / (SAMPLE_MATRIX_ELEMENT_SIZE - 1));
    std_gx = sqrt(sum_gx / (SAMPLE_MATRIX_ELEMENT_SIZE - 1));
    std_gy = sqrt(sum_gy / (SAMPLE_MATRIX_ELEMENT_SIZE - 1));
    std_gz = sqrt(sum_gz / (SAMPLE_MATRIX_ELEMENT_SIZE - 1));

    // Serial a(USBTX, USBRX);
    // a.printf("avg: %f %f %f %f %f %f, std: %f %f %f %f %f %f, max: %f %f %f %f %f %f, min: %f %f %f %f %f %f, hand: %d",
    //          avg_ax, avg_ay, avg_az, avg_gx, avg_gy, avg_gz,
    //          std_ax, std_ay, std_az, std_gx, std_gy, std_gz,
    //          max_ax, max_ay, max_az, max_gx, max_gy, max_gz,
    //          min_ax, min_ay, min_az, min_gx, min_gy, min_gz,
    //          hand);

    float input_data[25] = {
        avg_ax,
        avg_ay,
        avg_az,
        avg_gx,
        avg_gy,
        avg_gz,
        std_ax,
        std_ay,
        std_az,
        std_gx,
        std_gy,
        std_gz,
        max_ax,
        max_ay,
        max_az,
        max_gx,
        max_gy,
        max_gz,
        min_ax,
        min_ay,
        min_az,
        min_gx,
        min_gy,
        min_gz,
        hand,
    };

    uint8_t predicted_label;
    Tflite_Error error = _ml.predict_class(input_data, &predicted_label);

    if (label != NULL)
    {
        *label = Label(predicted_label);
    }

    return error;
}