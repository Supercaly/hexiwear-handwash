#ifndef _LABEL_PREDICTOR_H_
#define _LABEL_PREDICTOR_H_

#include "label.h"
#include "sensors.h"

#include "../training/models/my_model/my_model.hpp"
#include "uTensor.h"
#include "input_image.h"

class Label_Predictor
{
public:
    Label_Predictor(My_model *model);
    ~Label_Predictor();

    Label predict(float *data);

private:
    My_model *_model;
    uTensor::Tensor _digits;

    int index_of_label();
};

#endif // _LABEL_PREDICTOR_H_