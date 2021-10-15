#ifndef _LABEL_PREDICTOR_H_
#define _LABEL_PREDICTOR_H_

#include "label.h"
#include "sensors.h"

#include <math.h>

#include "../training/models/mlp_hw_model/mlp_hw_model.hpp"
#include "uTensor.h"
#include "input_image.h"

class Label_Predictor
{
public:
    Label_Predictor(Mlp_hw_model *model);
    ~Label_Predictor();

    Label predict(float *ax,
        float *ay,
        float *az,
        float *gx,
        float *gy,
        float *gz,
        float hand);

private:
    Mlp_hw_model *_model;
    uTensor::Tensor _actions;
    float *prev_ax, 
        *prev_ay, 
        *prev_az, 
        *prev_gx, 
        *prev_gy, 
        *prev_gz;
    #define PREV_DATA_CAPACITY SENSORS_DATA_CAPACITY/2

    Label action_to_label();
};

#endif // _LABEL_PREDICTOR_H_