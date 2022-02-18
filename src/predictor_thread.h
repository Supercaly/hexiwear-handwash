#ifndef PREDICTOR_THREAD_H_
#define PREDICTOR_THREAD_H_

#include "global_thread_vars.h"
#include "label_predictor.h"
#include "log.h"
#include "sample_matrix.h"

void prediction_thread_loop()
{
    Sample_Matrix *local_samples;
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
        local_samples = new Sample_Matrix(*g_sens_collector.getSamples());
        g_sensors_lock.unlock();
        // Exit critical section

        Label label;
        status = predictor->predict(local_samples, 0, &label);
        log_error("%s\n", tflite_error_to_cstr(status));

        delete local_samples;

        g_labels_queue.put(&label);
    }
}

#endif // PREDICTOR_THREAD_H_