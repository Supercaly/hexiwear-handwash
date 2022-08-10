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

#define STR_H(x) #x
#define STR(x) STR_H(x)
#define SVM_MODEL_PATH "/sd/svm_" STR(WINDOW_SIZE) "/model.model"

void prediction_thread_loop()
{
    DataImporter importer;
    Label actual_label,
        pred_label;
    static RawSensorData raw_chunk;
    Timer features_timer,
        inference_timer;

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
    log_info("open %s\n", SVM_MODEL_PATH);
    if (!svm.init(SVM_MODEL_PATH))
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
        // log_info("computing inference on new chunk of data...\n");

#ifdef HAS_SVM
        features_timer.reset();
        features_timer.start();
        float features[24];
        compute_features(&raw_chunk, features);
        features_timer.stop();

        inference_timer.reset();
        inference_timer.start();
        svm.predict(features, &pred_label);
        inference_timer.stop();
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

        log_info("predicted label: '%s', actual label: '%s'\n"
                 "\tfeature time: %lldus\n"
                 "\tinference time: %lldms\n\n",
                 label_to_cstr(pred_label),
                 label_to_cstr(actual_label),
                 chrono::duration_cast<chrono::microseconds>(features_timer.elapsed_time()).count(),
                 chrono::duration_cast<chrono::milliseconds>(inference_timer.elapsed_time()).count());

        counter++;
    }
}