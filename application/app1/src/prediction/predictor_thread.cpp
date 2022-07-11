#include "predictor_thread.h"
#include "common/global_thread_vars.h"

#include "log.h"
#include "predictor.h"
#include "raw_sensor_data.h"

void predictor_thread_loop()
{
    Predictor predictor;
    RawSensorData *local_samples;
    TFliteError status;

    status = predictor.init();
    if (status != TFliteError::OK)
    {
        log_error("Predictor init error: %s\n", tflite_error_to_cstr(status));
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

        Label predicted_label;
        status = predictor.predict(local_samples, g_config.get_wrist(), &predicted_label);
        if (status != TFliteError::OK)
        {
            log_error("Predictor error: %s\n", tflite_error_to_cstr(status));
        }

        delete local_samples;
        g_labels_queue.try_put(&predicted_label);
    }
}