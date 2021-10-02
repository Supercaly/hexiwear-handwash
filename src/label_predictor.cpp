#include "label_predictor.h"

Label_Predictor::Label_Predictor(My_model *model) : _model(model)
{
    _digits = new uTensor::RamTensor({1, 10}, flt);
}

Label_Predictor::~Label_Predictor()
{
}

Label Label_Predictor::predict(float *data)
{
    uTensor::Tensor input_image = new uTensor::RomTensor({1, 28, 28, 1}, flt, arr_input_image);
    uTensor::Tensor t = new uTensor::RomTensor({6, SENSORS_DATA_CAPACITY}, flt, data);

    // Serial a(USBTX, USBRX);
    // for (uint32_t i = 0; i < 10; ++i)
    // {
    //     for (uint32_t j = 0; j < 6; ++j)
    //     {
    //         float v = static_cast<float>(t(6 * i + j));
    //         a.printf("%f ", v);
    //         if (j == 5)
    //         {
    //             a.printf("\n");
    //         }
    //     }
    // }

    _model->set_inputs({{My_model::input_0, input_image}})
        .set_outputs({{My_model::output_0, _digits}})
        .eval();

    Label label = Label(index_of_label() % 3);

    return label;
}

int Label_Predictor::index_of_label()
{
    float max_value = static_cast<float>(_digits(0));
    int max_index = 0;

    for (uint32_t i = 1; i < (*_digits)->num_elems(); ++i)
    {
        float value = static_cast<float>(_digits(i));
        if (value >= max_value)
        {
            max_value = value;
            max_index = i;
        }
    }
    return max_index;
}