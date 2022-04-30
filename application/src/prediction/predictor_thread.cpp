#include "predictor_thread.h"
#include "common/global_thread_vars.h"
#include "common/log.h"
#include "data/raw_sensor_data.h"
#include "tflite_wrapper.h"

#include "model/handwash_model.h"

// TODO: Move those constants in config
#define FEATURES_SIZE 25
#define OUT_SIZE 3
#define TENSOR_ARENA_SIZE 3000

// Compute features from rae data.
static float *compute_features(RawSensorData *data, float hand)
{
    // FIXME: Review all this code
    log_info("data:    %f %f %f %f %f %f %f\n", data->ax[0], data->ay[0], data->az[0], data->gx[0], data->gy[0], data->gz[0], hand);
    log_info("data:    %f %f %f %f %f %f %f\n", data->ax[1], data->ay[1], data->az[1], data->gx[1], data->gy[1], data->gz[1], hand);
    log_info("data:    %f %f %f %f %f %f %f\n", data->ax[2], data->ay[2], data->az[2], data->gx[2], data->gy[2], data->gz[2], hand);
    log_info("data:    %f %f %f %f %f %f %f\n", data->ax[3], data->ay[3], data->az[3], data->gx[3], data->gy[3], data->gz[3], hand);
    log_info("data:    %f %f %f %f %f %f %f\n", data->ax[4], data->ay[4], data->az[4], data->gx[4], data->gy[4], data->gz[4], hand);

    float sum_ax = 0.0, sum_ay = 0.0, sum_az = 0.0, sum_gx = 0.0, sum_gy = 0.0, sum_gz = 0.0;
    float std_ax = 0.0, std_ay = 0.0, std_az = 0.0, std_gx = 0.0, std_gy = 0.0, std_gz = 0.0;
    float max_ax = 0.0, max_ay = 0.0, max_az = 0.0, max_gx = 0.0, max_gy = 0.0, max_gz = 0.0;
    float min_ax = 0.0, min_ay = 0.0, min_az = 0.0, min_gx = 0.0, min_gy = 0.0, min_gz = 0.0;

    // FIXME: This works only because the overlap is 0.5 so the prev data
    // and the new have the same size. For different overlap you need two for loops.
    for (int i = 0; i < RAW_SENSOR_DATA_BLOCK_CAP; ++i)
    {
        // Compute the sum of every component for the avg
        sum_ax += data->ax[i];
        sum_ay += data->ay[i];
        sum_az += data->az[i];
        sum_gx += data->gx[i];
        sum_gy += data->gy[i];
        sum_gz += data->gz[i];

        // compute the max
        if (data->ax[i] > max_ax)
            max_ax = data->ax[i];
        if (data->ay[i] > max_ay)
            max_ay = data->ay[i];
        if (data->az[i] > max_az)
            max_az = data->az[i];
        if (data->gx[i] > max_gx)
            max_gx = data->gx[i];
        if (data->gy[i] > max_gy)
            max_gy = data->gy[i];
        if (data->gz[i] > max_gz)
            max_gz = data->gz[i];

        // compute the min
        if (data->ax[i] < max_ax)
            max_ax = data->ax[i];
        if (data->ay[i] < max_ay)
            max_ay = data->ay[i];
        if (data->az[i] < max_az)
            max_az = data->az[i];
        if (data->gx[i] < max_gx)
            max_gx = data->gx[i];
        if (data->gy[i] < max_gy)
            max_gy = data->gy[i];
        if (data->gz[i] < max_gz)
            max_gz = data->gz[i];
    }

    float avg_ax = sum_ax / RAW_SENSOR_DATA_BLOCK_CAP,
          avg_ay = sum_ay / RAW_SENSOR_DATA_BLOCK_CAP,
          avg_az = sum_az / RAW_SENSOR_DATA_BLOCK_CAP,
          avg_gx = sum_gx / RAW_SENSOR_DATA_BLOCK_CAP,
          avg_gy = sum_gy / RAW_SENSOR_DATA_BLOCK_CAP,
          avg_gz = sum_gz / RAW_SENSOR_DATA_BLOCK_CAP;

    sum_ax = 0.0;
    sum_ay = 0.0;
    sum_az = 0.0;
    sum_gx = 0.0;
    sum_gy = 0.0;
    sum_gz = 0.0;
    for (int i = 0; i < RAW_SENSOR_DATA_BLOCK_CAP; ++i)
    {
        // Compute the standard deviation sum for each component
        sum_ax += (data->ax[i] - avg_ax) * (data->ax[i] - avg_ax);
        sum_ay += (data->ay[i] - avg_ay) * (data->ay[i] - avg_ay);
        sum_az += (data->az[i] - avg_az) * (data->az[i] - avg_az);
        sum_gx += (data->gx[i] - avg_gx) * (data->gx[i] - avg_gx);
        sum_gy += (data->gy[i] - avg_gy) * (data->gy[i] - avg_gy);
        sum_gz += (data->gz[i] - avg_gz) * (data->gz[i] - avg_gz);
    }

    std_ax = sqrt(sum_ax / (RAW_SENSOR_DATA_BLOCK_CAP - 1));
    std_ay = sqrt(sum_ay / (RAW_SENSOR_DATA_BLOCK_CAP - 1));
    std_az = sqrt(sum_az / (RAW_SENSOR_DATA_BLOCK_CAP - 1));
    std_gx = sqrt(sum_gx / (RAW_SENSOR_DATA_BLOCK_CAP - 1));
    std_gy = sqrt(sum_gy / (RAW_SENSOR_DATA_BLOCK_CAP - 1));
    std_gz = sqrt(sum_gz / (RAW_SENSOR_DATA_BLOCK_CAP - 1));

    static float input_data[25] = {
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

    g_data_recorder.record_data();

    return input_data;
}

void predictor_thread_loop()
{
    // FIXME: Figure out a way to retrieve the current used hand
    float hand = 0.0;
    RawSensorData *local_samples;
    auto tfwrapper = new TFliteWrapper<FEATURES_SIZE, OUT_SIZE, TENSOR_ARENA_SIZE>();
    TFliteError status;

    status = tfwrapper->init(g_handwash_model_data);
    if (status != TFliteError::OK)
    {
        log_error("TFliteWrapper: %s\n", tflite_error_to_cstr(status));
        return;
    }

    while (true)
    {
        // Enter critical section
        // Copy the data to a local memory so the critical
        // section last as short as possible
        g_sensors_lock.lock();
        local_samples = new RawSensorData(g_raw_sensor_data);
        g_sensors_lock.unlock();
        // Exit critical section

        float *features = compute_features(local_samples, hand);

        Label predicted_label;
        status = tfwrapper->predict_label(features, &predicted_label);

        if (status != TFliteError::OK)
        {
            log_error("TFliteWrapper: %s\n", tflite_error_to_cstr(status));
        }

        delete local_samples;

        g_labels_queue.try_put(&predicted_label);
    }
}