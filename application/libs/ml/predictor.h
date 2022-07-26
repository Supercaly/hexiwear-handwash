#ifndef PREDICTOR_H_
#define PREDICTOR_H_

#include "label.h"
#include "raw_sensor_data.h"
#include "wrist.h"

#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"

// Constants
#define TENSOR_IN_SIZE 6 * FULL_WINDOW_SIZE
#define TENSOR_OUT_SIZE 3
#define TENSOR_ARENA_SIZE 20000

enum PredictorError
{
    OK,
    VERSION_MISMATCH,
    CANT_ALLOCATE_TENSOR,
    ALREADY_INIT,
    NOT_INITIALIZED,
    INVOKE_ERROR
};

static const char *predictor_error_to_cstr(PredictorError e)
{
    switch (e)
    {
    case PredictorError::OK:
        return "no error";
    case PredictorError::VERSION_MISMATCH:
        return "version mismatch";
    case PredictorError::CANT_ALLOCATE_TENSOR:
        return "cannot allocate tensor";
    case PredictorError::NOT_INITIALIZED:
        return "not initialized";
    case PredictorError::INVOKE_ERROR:
        return "invocation error";
    default:
        return "unknown error";
    }
}

static PredictorError tflite_status_to_predictor_error(TfLiteStatus s)
{
    switch (s)
    {
    case TfLiteStatus::kTfLiteOk:
        return PredictorError::OK;
    default:
        return PredictorError::INVOKE_ERROR;
    }
}

class Predictor
{
public:
    Predictor();
    ~Predictor();

    PredictorError init();
    PredictorError predict_label(RawSensorData *raw, Wrist wrist, Label *label);

private:
    const tflite::Model *_model;
    tflite::ErrorReporter *_error_reporter;
    tflite::MicroInterpreter *_interpreter;
    TfLiteTensor *_input;
    TfLiteTensor *_output;
    bool _init;
    uint8_t *_tensor_arena;

    Label probability_to_label(int8_t *proba);
};

#endif // PREDICTOR_H_