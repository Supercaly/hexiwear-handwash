#ifndef _LABEL_PREDICTOR_H_
#define _LABEL_PREDICTOR_H_

#include "label.h"
#include "handwash_model.h"
#include "sensors.h"

#include <math.h>

#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/system_setup.h"

class Label_Predictor
{
public:
    Label_Predictor();
    Label_Predictor(const tflite::Model *model,
                    tflite::ErrorReporter *error_reporter,
                    tflite::MicroInterpreter *interpreter);
    ~Label_Predictor();

    Label predict(float *ax,
                  float *ay,
                  float *az,
                  float *gx,
                  float *gy,
                  float *gz,
                  float hand);

private:
    const tflite::Model *_model;
    tflite::ErrorReporter *_error_reporter;
    tflite::MicroInterpreter *_interpreter;
    TfLiteTensor *_input, *_output;

    Label action_to_label();
};

#endif // _LABEL_PREDICTOR_H_