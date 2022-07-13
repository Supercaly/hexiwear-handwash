#include "prediction_thread.h"
#include "data_importer.h"

#include "label.h"
#include "predictor.h"
#include "raw_sensor_data.h"

void prediction_thread_loop()
{
    Predictor predictor;
    DataImporter importer;
    Label actual_label,
        pred_label;
    static RawSensorData raw_chunk;
    TFliteError status;

    // Init importer and predictor
    if (!importer.init())
    {
        log_error("error initializing DataImporter\n");
        return;
    }
    status = predictor.init();
    if (status != TFliteError::OK)
    {
        log_error("error initializing Predictor: %s\n",
                  tflite_error_to_cstr(status));
        return;
    }

    while (importer.next_chunk(&raw_chunk, &actual_label))
    {
        log_info("chunk of data read\n");
        status = predictor.predict(&raw_chunk, Wrist::LEFT, &pred_label);
        if (status != TFliteError::OK)
        {
            log_error("fatal error predicting label: %s\n",
                      tflite_error_to_cstr(status));
            return;
        }

        log_info("predicted label: %s actual label: %s\n",
                 label_to_cstr(pred_label),
                 label_to_cstr(actual_label));
    }
}