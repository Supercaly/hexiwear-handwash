#ifndef LABEL_PREDICTOR_H_
#define LABEL_PREDICTOR_H_

#include "sample_matrix.h"
#include "label.h"
#include "tflite_wrapper.h"

constexpr int k_tensor_arena_size = 3000;

class Label_Predictor
{
public:
    Label_Predictor();
    ~Label_Predictor();

    Tflite_Error init();
    Tflite_Error predict(Sample_Matrix *sample, float hand, Label *label);

private:
    Tflite_Wrapper<25, 3, k_tensor_arena_size> _ml;
};

#endif // LABEL_PREDICTOR_H_