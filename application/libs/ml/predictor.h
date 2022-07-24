#ifndef PREDICTOR_H_
#define PREDICTOR_H_

#include "label.h"
#include "raw_sensor_data.h"
#include "tflite_wrapper.h"
#include "wrist.h"

// Constants
#define FEATURES_SIZE 25
#define OUT_SIZE 3
#define TENSOR_ARENA_SIZE 3000

class Predictor
{
public:
    Predictor();
    ~Predictor();

    TFliteError init();
    TFliteError predict(RawSensorData *raw, Wrist wrist, Label *label);

private:
    TFliteWrapper<FEATURES_SIZE, OUT_SIZE, TENSOR_ARENA_SIZE> *_tfwrapper;
    float _features[FEATURES_SIZE];

    void compute_features(RawSensorData *data, float wrist);
};

#endif // PREDICTOR_H_