#include "prediction_thread.h"
#include "data_importer.h"

#include "features.h"
#include "label.h"
#include "log.h"
#include "raw_sensor_data.h"
#ifdef HAS_SVM
#include "svm.h"
#endif // HAS_SVM

void prediction_thread_loop()
{
    DataImporter importer;
    Label actual_label,
        pred_label;
    static RawSensorData raw_chunk;
#ifdef HAS_SVM
    SVM svm;
#endif // HAS_SVM

    // Init importer
    if (!importer.init())
    {
        log_error("error initializing DataImporter\n");
        return;
    }

#ifdef HAS_SVM
    // Init SVM
    if (!svm.init("/sd/model_10.model"))
    {
        log_error("error loading SVM model from file\n");
        return;
    }
#endif // HAS_SVM

    while (importer.next_chunk(&raw_chunk, &actual_label))
    {
        log_info("chunk of data read\n");

#ifdef HAS_SVM
        svm.predict(&raw_chunk, &pred_label);
#endif // HAS_SVM

        log_info("predicted label: %s actual label: %s\n",
                 label_to_cstr(pred_label),
                 label_to_cstr(actual_label));
    }
}