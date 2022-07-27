#include "prediction_thread.h"
#include "data_importer.h"

#include "features.h"
#include "label.h"
#include "log.h"
#include "raw_sensor_data.h"
// Include the correct ml library
#ifdef HAS_SVM
#include "svm.h"
#elif defined(HAS_ML)
#include "predictor.h"
#else
#error cannot find a machine learning library!
#endif // HAS_SVM

void prediction_thread_loop()
{
    DataImporter importer;
    Label actual_label,
        pred_label;
    static RawSensorData raw_chunk;
    Timer inference_timer;

#ifdef HAS_SVM
    SVM svm;
#elif defined(HAS_ML)
    Predictor predictor;
    PredictorError status;
#endif // HAS_SVM

    // Init importer
    if (!importer.init())
    {
        log_error("error initializing DataImporter\n");
        return;
    }
    log_info("DataImporter init!\n");

#ifdef HAS_SVM
    // Init SVM
    if (!svm.init("/sd/model_10.model"))
    {
        log_error("error loading SVM model from file\n");
        return;
    }
    log_info("SVM init!\n");
#elif defined(HAS_ML)
    // Init Predictor
    status = predictor.init();
    if (status != PredictorError::OK)
    {
        log_error("error initializing Predictor: %s\n",
                  predictor_error_to_cstr(status));
        return;
    }
    log_info("Predictor init!\n");
#endif // HAS_SVM

    int counter = 0;
    while (importer.next_chunk(&raw_chunk, &actual_label) && counter < 100)
    {
        log_info("computing inference on new chunk of data...\n");

#ifdef HAS_SVM
        svm.predict(&raw_chunk, &pred_label);
#elif defined(HAS_ML)
        inference_timer.reset();
        inference_timer.start();
        // TODO: Remove wrist option since it's unused
        status = predictor.predict_label(&raw_chunk, Wrist::LEFT, &pred_label);
        if (status != PredictorError::OK)
        {
            log_error("fatal error predicting label: %s\n",
                      predictor_error_to_cstr(status));
            return;
        }
        inference_timer.stop();
#endif // HAS_SVM

        log_info("predicted label: %s actual label: %s "
                 "in: %lldms\n",
                 label_to_cstr(pred_label),
                 label_to_cstr(actual_label),
                 chrono::duration_cast<chrono::milliseconds>(inference_timer.elapsed_time()).count());
        counter++;
    }
}