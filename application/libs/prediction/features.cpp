#include "features.h"

#include <math.h>
#include "log.h"

void compute_features(RawSensorData *data, float *output)
{
    // FIXME: Review all this code
    log_info("data: %f %f %f %f %f %f \n", data->ax[0], data->ay[0], data->az[0], data->gx[0], data->gy[0], data->gz[0]);
    log_info("data: %f %f %f %f %f %f \n", data->ax[1], data->ay[1], data->az[1], data->gx[1], data->gy[1], data->gz[1]);
    log_info("data: %f %f %f %f %f %f \n", data->ax[2], data->ay[2], data->az[2], data->gx[2], data->gy[2], data->gz[2]);
    log_info("data: %f %f %f %f %f %f \n", data->ax[3], data->ay[3], data->az[3], data->gx[3], data->gy[3], data->gz[3]);
    log_info("data: %f %f %f %f %f %f \n", data->ax[4], data->ay[4], data->az[4], data->gx[4], data->gy[4], data->gz[4]);

    float sum_ax = 0.0, sum_ay = 0.0, sum_az = 0.0, sum_gx = 0.0, sum_gy = 0.0, sum_gz = 0.0;
    float std_ax = 0.0, std_ay = 0.0, std_az = 0.0, std_gx = 0.0, std_gy = 0.0, std_gz = 0.0;
    float max_ax = 0.0, max_ay = 0.0, max_az = 0.0, max_gx = 0.0, max_gy = 0.0, max_gz = 0.0;
    float min_ax = 0.0, min_ay = 0.0, min_az = 0.0, min_gx = 0.0, min_gy = 0.0, min_gz = 0.0;

    // FIXME: This works only because the overlap is 0.5 so the prev data
    // and the new have the same size. For different overlap you need two for loops.
    for (int i = 0; i < FULL_WINDOW_SIZE; ++i)
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

    float avg_ax = sum_ax / FULL_WINDOW_SIZE,
          avg_ay = sum_ay / FULL_WINDOW_SIZE,
          avg_az = sum_az / FULL_WINDOW_SIZE,
          avg_gx = sum_gx / FULL_WINDOW_SIZE,
          avg_gy = sum_gy / FULL_WINDOW_SIZE,
          avg_gz = sum_gz / FULL_WINDOW_SIZE;

    sum_ax = 0.0;
    sum_ay = 0.0;
    sum_az = 0.0;
    sum_gx = 0.0;
    sum_gy = 0.0;
    sum_gz = 0.0;
    for (int i = 0; i < FULL_WINDOW_SIZE; ++i)
    {
        // Compute the standard deviation sum for each component
        sum_ax += (data->ax[i] - avg_ax) * (data->ax[i] - avg_ax);
        sum_ay += (data->ay[i] - avg_ay) * (data->ay[i] - avg_ay);
        sum_az += (data->az[i] - avg_az) * (data->az[i] - avg_az);
        sum_gx += (data->gx[i] - avg_gx) * (data->gx[i] - avg_gx);
        sum_gy += (data->gy[i] - avg_gy) * (data->gy[i] - avg_gy);
        sum_gz += (data->gz[i] - avg_gz) * (data->gz[i] - avg_gz);
    }

    std_ax = sqrt(sum_ax / (FULL_WINDOW_SIZE - 1));
    std_ay = sqrt(sum_ay / (FULL_WINDOW_SIZE - 1));
    std_az = sqrt(sum_az / (FULL_WINDOW_SIZE - 1));
    std_gx = sqrt(sum_gx / (FULL_WINDOW_SIZE - 1));
    std_gy = sqrt(sum_gy / (FULL_WINDOW_SIZE - 1));
    std_gz = sqrt(sum_gz / (FULL_WINDOW_SIZE - 1));

    output[0] = avg_ax;
    output[1] = avg_ay;
    output[2] = avg_az;
    output[3] = avg_gx;
    output[4] = avg_gy;
    output[5] = avg_gz;
    output[6] = std_ax;
    output[7] = std_ay;
    output[8] = std_az;
    output[9] = std_gx;
    output[10] = std_gy;
    output[11] = std_gz;
    output[12] = max_ax;
    output[13] = max_ay;
    output[14] = max_az;
    output[15] = max_gx;
    output[16] = max_gy;
    output[17] = max_gz;
    output[18] = min_ax;
    output[19] = min_ay;
    output[20] = min_az;
    output[21] = min_gx;
    output[22] = min_gy;
    output[23] = min_gz;
}