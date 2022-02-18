#include "predictor_thread.h"
#include "global_thread_vars.h"
#include "label_predictor.h"
#include "log.h"
#include "raw_sensor_data.h"

void predictor_thread_loop()
{
    RawSensorData *local_samples;
    std::unique_ptr<Label_Predictor> predictor(new Label_Predictor);
    Tflite_Error status;

    status = predictor->init();
    if (status != Tflite_Error::OK)
    {
        log_error("%s\n", tflite_error_to_cstr(status));
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

        Label label;
        status = predictor->predict(local_samples, 0, &label);
        log_error("%s\n", tflite_error_to_cstr(status));

        delete local_samples;

        g_labels_queue.put(&label);
    }
}