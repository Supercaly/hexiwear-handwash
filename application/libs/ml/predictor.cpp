#include "predictor.h"
#include "model/model.h"

#include "log.h"

Predictor::Predictor() : _init(false)
{
    _tensor_arena = (uint8_t *)malloc(sizeof(uint8_t) * TENSOR_ARENA_SIZE);
    printf("tensor arena location: %d\n", _tensor_arena);
}

Predictor::~Predictor()
{
    delete _memory_planner;
    free(_tensor_arena);
}

PredictorError Predictor::init()
{
    if (_init)
    {
        log_error("Predictor: model already initialized.\n");
        return PredictorError::ALREADY_INIT;
    }

    tflite::InitializeTarget();
    static tflite::MicroErrorReporter static_error_reporter;
    _error_reporter = &static_error_reporter;

    _model = tflite::GetModel(model_tflite);
    if (_model->version() != TFLITE_SCHEMA_VERSION)
    {
        log_error("Predictor: provided model has schema version %lu, "
                  "but supported version is %d.\n",
                  _model->version(), TFLITE_SCHEMA_VERSION);
        return PredictorError::VERSION_MISMATCH;
    }

    static tflite::MicroMutableOpResolver<13> static_ops_resolver;
    static_ops_resolver.AddShape();
    static_ops_resolver.AddStridedSlice();
    static_ops_resolver.AddTranspose();
    static_ops_resolver.AddUnpack();
    static_ops_resolver.AddPack();
    static_ops_resolver.AddFill();
    static_ops_resolver.AddFullyConnected();
    static_ops_resolver.AddAdd();
    static_ops_resolver.AddSplit();
    static_ops_resolver.AddLogistic();
    static_ops_resolver.AddMul();
    static_ops_resolver.AddTanh();
    static_ops_resolver.AddSoftmax();

    _arena_buffer_alloc = tflite::RecordingSingleArenaBufferAllocator::Create(_error_reporter, _tensor_arena, TENSOR_ARENA_SIZE);
    _memory_planner = new tflite::GreedyMemoryPlanner();
    static tflite::MicroAllocator *micro_allocator =
        tflite::MicroAllocator::Create(_arena_buffer_alloc, _memory_planner, _error_reporter);

    static tflite::MicroInterpreter static_interpreter(
        _model, static_ops_resolver, micro_allocator, _error_reporter);
    _interpreter = &static_interpreter;
    TfLiteStatus allocate_status = _interpreter->AllocateTensors();
    if (allocate_status != kTfLiteOk)
    {
        log_error("Predictor: tensor allocation failed.\n");
        return PredictorError::CANT_ALLOCATE_TENSOR;
    }

    _input = _interpreter->input_tensor(0);
    _output = _interpreter->output_tensor(0);

    _init = true;

    log_info("Predictor: initialized\n");
    return PredictorError::OK;
}

PredictorError Predictor::predict_label(RawSensorData *raw, Wrist wrist, Label *label)
{
    PredictorError status = PredictorError::OK;
    TfLiteStatus s;
    int8_t *input_tens_buff;
    int8_t *output_tens_buff;
    int8_t *output;

    if (!_init)
    {
        return PredictorError::NOT_INITIALIZED;
    }

    output = (int8_t *)malloc(sizeof(int8_t) * TENSOR_OUT_SIZE);
    if (output == NULL)
    {
        status = PredictorError::CANT_ALLOCATE_TENSOR;
        goto exit;
    }

    // Copy input data to input tensor
    input_tens_buff = tflite::GetTensorData<int8_t>(_input);
    for (int i = 0; i < FULL_WINDOW_SIZE; i++)
    {
        input_tens_buff[i + (0 * FULL_WINDOW_SIZE)] = (int8_t)raw->ax[i];
        input_tens_buff[i + (1 * FULL_WINDOW_SIZE)] = (int8_t)raw->ay[i];
        input_tens_buff[i + (2 * FULL_WINDOW_SIZE)] = (int8_t)raw->az[i];
        input_tens_buff[i + (3 * FULL_WINDOW_SIZE)] = (int8_t)raw->gx[i];
        input_tens_buff[i + (4 * FULL_WINDOW_SIZE)] = (int8_t)raw->gy[i];
        input_tens_buff[i + (5 * FULL_WINDOW_SIZE)] = (int8_t)raw->gz[i];
    }

    s = _interpreter->Invoke();
    if (s != TfLiteStatus::kTfLiteOk)
    {
        status = tflite_status_to_predictor_error(s);
        goto exit;
    }

    output_tens_buff = tflite::GetTensorData<int8_t>(_output);
    for (int i = 0; i < TENSOR_OUT_SIZE; i++)
    {
        output[i] = output_tens_buff[i];
    }

    log_error("Memory used for inference: %d\n", _arena_buffer_alloc->GetUsedBytes());

    *label = probability_to_label(output);

exit:
    free(output);
    return status;
}

Label Predictor::probability_to_label(int8_t *proba)
{
    int8_t max_value = proba[0];
    uint8_t max_idx = 0;

    for (uint8_t i = 1; i < TENSOR_OUT_SIZE; ++i)
    {
        if (proba[i] > max_value)
        {
            max_value = proba[i];
            max_idx = i;
        }
    }

    return Label(max_idx);
}