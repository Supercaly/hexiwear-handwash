#include "label_predictor.h"
#include "handwash_model.h"

#include <math.h>

Label_Predictor::Label_Predictor() {}

Label_Predictor::~Label_Predictor() {}

Tflite_Error Label_Predictor::init()
{
    return _ml.init(g_handwash_model_data);
}

Tflite_Error Label_Predictor::predict(Sample_Matrix *sample, float hand, Label *label)
{
    log_info("data:    %f %f %f %f %f %f %f\n", sample->ax[0], sample->ay[0], sample->az[0], sample->gx[0], sample->gy[0], sample->gz[0], hand);
    log_info("data:    %f %f %f %f %f %f %f\n", sample->ax[1], sample->ay[1], sample->az[1], sample->gx[1], sample->gy[1], sample->gz[1], hand);
    log_info("data:    %f %f %f %f %f %f %f\n", sample->ax[2], sample->ay[2], sample->az[2], sample->gx[2], sample->gy[2], sample->gz[2], hand);
    log_info("data:    %f %f %f %f %f %f %f\n", sample->ax[3], sample->ay[3], sample->az[3], sample->gx[3], sample->gy[3], sample->gz[3], hand);
    log_info("data:    %f %f %f %f %f %f %f\n", sample->ax[4], sample->ay[4], sample->az[4], sample->gx[4], sample->gy[4], sample->gz[4], hand);

    float sum_ax = 0.0, sum_ay = 0.0, sum_az = 0.0, sum_gx = 0.0, sum_gy = 0.0, sum_gz = 0.0;
    float std_ax = 0.0, std_ay = 0.0, std_az = 0.0, std_gx = 0.0, std_gy = 0.0, std_gz = 0.0;
    float max_ax = 0.0, max_ay = 0.0, max_az = 0.0, max_gx = 0.0, max_gy = 0.0, max_gz = 0.0;
    float min_ax = 0.0, min_ay = 0.0, min_az = 0.0, min_gx = 0.0, min_gy = 0.0, min_gz = 0.0;

    // NOTE: This works only because the overlap is 0.5 so the prev data
    // and the new have the same size. For different overlap you need two for loops.
    for (int i = 0; i < SAMPLE_MATRIX_ELEMENT_SIZE; ++i)
    {
        // Compute the sum of every component for the avg
        sum_ax += sample->ax[i];
        sum_ay += sample->ay[i];
        sum_az += sample->az[i];
        sum_gx += sample->gx[i];
        sum_gy += sample->gy[i];
        sum_gz += sample->gz[i];

        // compute the max
        if (sample->ax[i] > max_ax)
            max_ax = sample->ax[i];
        if (sample->ay[i] > max_ay)
            max_ay = sample->ay[i];
        if (sample->az[i] > max_az)
            max_az = sample->az[i];
        if (sample->gx[i] > max_gx)
            max_gx = sample->gx[i];
        if (sample->gy[i] > max_gy)
            max_gy = sample->gy[i];
        if (sample->gz[i] > max_gz)
            max_gz = sample->gz[i];

        // compute the min
        if (sample->ax[i] < max_ax)
            max_ax = sample->ax[i];
        if (sample->ay[i] < max_ay)
            max_ay = sample->ay[i];
        if (sample->az[i] < max_az)
            max_az = sample->az[i];
        if (sample->gx[i] < max_gx)
            max_gx = sample->gx[i];
        if (sample->gy[i] < max_gy)
            max_gy = sample->gy[i];
        if (sample->gz[i] < max_gz)
            max_gz = sample->gz[i];
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
        sum_ax += (sample->ax[i] - avg_ax) * (sample->ax[i] - avg_ax);
        sum_ay += (sample->ay[i] - avg_ay) * (sample->ay[i] - avg_ay);
        sum_az += (sample->az[i] - avg_az) * (sample->az[i] - avg_az);
        sum_gx += (sample->gx[i] - avg_gx) * (sample->gx[i] - avg_gx);
        sum_gy += (sample->gy[i] - avg_gy) * (sample->gy[i] - avg_gy);
        sum_gz += (sample->gz[i] - avg_gz) * (sample->gz[i] - avg_gz);
    }

    std_ax = sqrt(sum_ax / (SAMPLE_MATRIX_ELEMENT_SIZE - 1));
    std_ay = sqrt(sum_ay / (SAMPLE_MATRIX_ELEMENT_SIZE - 1));
    std_az = sqrt(sum_az / (SAMPLE_MATRIX_ELEMENT_SIZE - 1));
    std_gx = sqrt(sum_gx / (SAMPLE_MATRIX_ELEMENT_SIZE - 1));
    std_gy = sqrt(sum_gy / (SAMPLE_MATRIX_ELEMENT_SIZE - 1));
    std_gz = sqrt(sum_gz / (SAMPLE_MATRIX_ELEMENT_SIZE - 1));

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

    uint8_t predicted_label = 0;
    Tflite_Error error = _ml.predict_class(input_data, &predicted_label);

    if (label != NULL)
    {
        *label = Label(predicted_label);
    }

    return error;
}