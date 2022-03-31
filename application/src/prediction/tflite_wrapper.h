#ifndef TFLITE_WRAPPER_H_
#define TFLITE_WRAPPER_H_

#include "data/label.h"
#include "common/log.h"

#include "../model/handwash_model.h"
#include "tensorflow/tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/tensorflow/lite/micro/system_setup.h"
#include "tensorflow/tensorflow/lite/schema/schema_generated.h"

// Represent an error during Tensorflow lite execution.
enum class TFliteError
{
    OK,
    VERSION_MISMATCH,
    CANT_ALLOCATE_TENSOR,
    NOT_INITIALIZED,
    INVOKE_ERROR
};

// Converts an error to a cstr.
static const char *tflite_error_to_cstr(TFliteError error)
{
    switch (error)
    {
    case TFliteError::OK:
        return "no error";
    case TFliteError::VERSION_MISMATCH:
        return "version mismatch";
    case TFliteError::CANT_ALLOCATE_TENSOR:
        return "cannot allocate tensor";
    case TFliteError::NOT_INITIALIZED:
        return "not initialized";
    case TFliteError::INVOKE_ERROR:
        return "invocation error";
    default:
        return "unknown error";
    }
}

// Wrapper class for Tensorflow lite.
template <size_t input_size, size_t output_size, size_t tensor_arena_size>
class TFliteWrapper
{
public:
    TFliteWrapper() : _init(false) {}
    ~TFliteWrapper() {}

    // Initialize the tensorflow interpreter and load the model.
    TFliteError init(const unsigned char *model_data)
    {
        tflite::InitializeTarget();
        static tflite::MicroErrorReporter static_error_reporter;
        _error_reporter = &static_error_reporter;

        _model = tflite::GetModel(g_handwash_model_data);
        if (_model->version() != TFLITE_SCHEMA_VERSION)
        {
            log_error("TFliteWrapper: provided model has schema version %d, "
                      "but supported version is %d.\n",
                      _model->version(), TFLITE_SCHEMA_VERSION);
            return TFliteError::VERSION_MISMATCH;
        }

        // TODO: Replace all ops resolver
        static tflite::AllOpsResolver static_ops_resolver;

        static tflite::MicroInterpreter static_interpreter(
            _model, static_ops_resolver, _tensor_arena, tensor_arena_size, _error_reporter);
        _interpreter = &static_interpreter;

        TfLiteStatus allocate_status = static_interpreter.AllocateTensors();
        if (allocate_status != kTfLiteOk)
        {
            log_error("TFliteWrapper: tensor allocation failed\n");
            return TFliteError::CANT_ALLOCATE_TENSOR;
        }

        _input = _interpreter->input_tensor(0);
        _output = _interpreter->output_tensor(0);

        _init = true;

        log_info("TFliteWrapper: initialized\n");
        return TFliteError::OK;
    }

    // Predict a label form input data.
    TFliteError predict_label(float *input, Label *predicted)
    {
        float output[output_size];
        TFliteError error = predict(input, output);
        if (error != TFliteError::OK)
        {
            return error;
        }

        if (predicted != NULL)
        {
            *predicted = Label(prob_to_class(output));
        }

        return TFliteError::OK;
    }

private:
    const tflite::Model *_model;
    tflite::ErrorReporter *_error_reporter;
    tflite::MicroInterpreter *_interpreter;
    TfLiteTensor *_input;
    TfLiteTensor *_output;
    bool _init;
    uint8_t _tensor_arena[tensor_arena_size];

    // Predict a probability array from input data.
    TFliteError predict(float *input, float *output)
    {
        if (!_init)
        {
            log_error("TFliteWrapper::predict called without initialization\n");
            return TFliteError::NOT_INITIALIZED;
        }

        for (size_t idx = 0; idx < input_size; ++idx)
        {
            _input->data.f[idx] = input[idx];
        }

        if (_interpreter->Invoke() != kTfLiteOk)
        {
            return TFliteError::INVOKE_ERROR;
        }

        if (output != NULL)
        {
            for (size_t idx = 0; idx < output_size; ++idx)
            {
                output[idx] = _output->data.f[idx];
            }
        }

        return TFliteError::OK;
    }

    // Convert a probability array to a single class.
    uint8_t prob_to_class(float *output)
    {
        float max_value = output[0];
        uint8_t max_idx = 0;

        for (uint8_t i = 1; i < output_size; ++i)
        {
            if (output[i] > max_value)
            {
                max_value = output[i];
                max_idx = i;
            }
        }

        return max_idx;
    }
};

#endif // TFLITE_WRAPPER_H_