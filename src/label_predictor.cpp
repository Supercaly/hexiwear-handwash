#include "label_predictor.h"

Label_Predictor::Label_Predictor(Mlp_hw_model *model) : _model(model)
{
    prev_ax = (float *)calloc(PREV_DATA_CAPACITY, sizeof(float));
    prev_ay = (float *)calloc(PREV_DATA_CAPACITY, sizeof(float));
    prev_az = (float *)calloc(PREV_DATA_CAPACITY, sizeof(float));
    prev_gx = (float *)calloc(PREV_DATA_CAPACITY, sizeof(float));
    prev_gy = (float *)calloc(PREV_DATA_CAPACITY, sizeof(float));
    prev_gz = (float *)calloc(PREV_DATA_CAPACITY, sizeof(float));
    _actions = new uTensor::RamTensor({1, 3}, flt);
}

Label_Predictor::~Label_Predictor()
{
}

Label Label_Predictor::predict(float *ax, float *ay, float *az, float *gx, float *gy, float *gz, float hand)
{
    float sum_ax=0.0, sum_ay=0.0, sum_az=0.0, sum_gx=0.0, sum_gy=0.0, sum_gz=0.0;
    float std_ax=0.0, std_ay=0.0, std_az=0.0, std_gx=0.0, std_gy=0.0, std_gz=0.0;
    float max_ax=0.0, max_ay=0.0, max_az=0.0, max_gx=0.0, max_gy=0.0, max_gz=0.0;
    float min_ax=0.0, min_ay=0.0, min_az=0.0, min_gx=0.0, min_gy=0.0, min_gz=0.0;

    // NOTE: This works only because the overlap is 0.5 so the prev data
    // and the new have the same size. For different overlap you need two for loops.
    for (int i = 0; i < PREV_DATA_CAPACITY; ++i)
    {
        // Compute the sum of every component for the avg
        sum_ax += ax[i];
        sum_ax += prev_ax[i];
        sum_ay += ay[i];
        sum_ay += prev_ay[i];
        sum_az += az[i];
        sum_az += prev_az[i];
        sum_gx += gx[i];
        sum_gx += prev_gx[i];
        sum_gy += gy[i];
        sum_gy += prev_gy[i];
        sum_gz += gz[i];
        sum_gz += prev_gz[i];

        // compute the max
        if (prev_ax[i] > max_ax)
            max_ax = prev_ax[i];
        if (prev_ay[i] > max_ay)
            max_ay = prev_ay[i];
        if (prev_az[i] > max_az)
            max_az = prev_az[i];
        if (prev_gx[i] > max_gx)
            max_gx = prev_gx[i];
        if (prev_gy[i] > max_gy)
            max_gy = prev_gy[i];
        if (prev_gz[i] > max_gz)
            max_gz = prev_gz[i];
        if (ax[i] > max_ax)
            max_ax = ax[i];
        if (ay[i] > max_ay)
            max_ay = ay[i];
        if (az[i] > max_az)
            max_az = az[i];
        if (gx[i] > max_gx)
            max_gx = gx[i];
        if (gy[i] > max_gy)
            max_gy = gy[i];
        if (gz[i] > max_gz)
            max_gz = gz[i];
            
        // compute the min
        if (prev_ax[i] < max_ax)
            max_ax = prev_ax[i];
        if (prev_ay[i] < max_ay)
            max_ay = prev_ay[i];
        if (prev_az[i] < max_az)
            max_az = prev_az[i];
        if (prev_gx[i] < max_gx)
            max_gx = prev_gx[i];
        if (prev_gy[i] < max_gy)
            max_gy = prev_gy[i];
        if (prev_gz[i] < max_gz)
            max_gz = prev_gz[i];
        if (ax[i] < max_ax)
            max_ax = ax[i];
        if (ay[i] < max_ay)
            max_ay = ay[i];
        if (az[i] < max_az)
            max_az = az[i];
        if (gx[i] < max_gx)
            max_gx = gx[i];
        if (gy[i] < max_gy)
            max_gy = gy[i];
        if (gz[i] < max_gz)
            max_gz = gz[i];
    }

    float avg_ax = sum_ax/SENSORS_DATA_CAPACITY,
        avg_ay=sum_ay/SENSORS_DATA_CAPACITY,
        avg_az=sum_az/SENSORS_DATA_CAPACITY,
        avg_gx=sum_gx/SENSORS_DATA_CAPACITY,
        avg_gy=sum_gy/SENSORS_DATA_CAPACITY,
        avg_gz=sum_gz/SENSORS_DATA_CAPACITY;
    
    sum_ax = 0.0;
    sum_ay = 0.0;
    sum_az = 0.0;
    sum_gx = 0.0;
    sum_gy = 0.0;
    sum_gz = 0.0;
    for (int i = 0; i < PREV_DATA_CAPACITY; ++i)
    {
        // Compute the standard deviation sum for each component
        sum_ax += (ax[i] - avg_ax) * (ax[i] - avg_ax);
        sum_ax += (prev_ax[i] - avg_ax) * (prev_ax[i] - avg_ax);
        sum_ay += (ay[i] - avg_ay) * (ay[i] - avg_ay);
        sum_ay += (prev_ay[i] - avg_ay) * (prev_ay[i] - avg_ay);
        sum_az += (az[i] - avg_az) * (az[i] - avg_az);
        sum_az += (prev_az[i] - avg_az) * (prev_az[i] - avg_az);
        sum_gx += (gx[i] - avg_gx) * (gx[i] - avg_gx);
        sum_gx += (prev_gx[i] - avg_gx) * (prev_gx[i] - avg_gx);
        sum_gy += (gy[i] - avg_gy) * (gy[i] - avg_gy);
        sum_gy += (prev_gy[i] - avg_gy) * (prev_gy[i] - avg_gy);
        sum_gz += (gz[i] - avg_gz) * (gz[i] - avg_gz);
        sum_gz += (prev_gz[i] - avg_gz) * (prev_gz[i] - avg_gz);
    }

    std_ax = sqrt(sum_ax/(SENSORS_DATA_CAPACITY-1));
    std_ay = sqrt(sum_ay/(SENSORS_DATA_CAPACITY-1));
    std_az = sqrt(sum_az/(SENSORS_DATA_CAPACITY-1));
    std_gx = sqrt(sum_gx/(SENSORS_DATA_CAPACITY-1));
    std_gy = sqrt(sum_gy/(SENSORS_DATA_CAPACITY-1));
    std_gz = sqrt(sum_gz/(SENSORS_DATA_CAPACITY-1));

    // Copy the second part of the new data to the prev arrays

    for (int i = 0; i < PREV_DATA_CAPACITY; ++i)
    {
        prev_ax[i] = ax[i + PREV_DATA_CAPACITY];
        prev_ay[i] = ay[i + PREV_DATA_CAPACITY];
        prev_az[i] = az[i + PREV_DATA_CAPACITY];
        prev_gx[i] = gx[i + PREV_DATA_CAPACITY];
        prev_gy[i] = gy[i + PREV_DATA_CAPACITY];
        prev_gz[i] = gz[i + PREV_DATA_CAPACITY];
    }

    Serial a(USBTX, USBRX);
    a.printf("avg: %f %f %f %f %f %f, std: %f %f %f %f %f %f, max: %f %f %f %f %f %f, min: %f %f %f %f %f %f, hand: %d",
        avg_ax,avg_ay,avg_az,avg_gx,avg_gy,avg_gz,
        std_ax,std_ay,std_az,std_gx,std_gy,std_gz,
        max_ax,max_ay,max_az,max_gx,max_gy,max_gz,
        min_ax,min_ay,min_az,min_gx,min_gy,min_gz,
        hand);
    
    float data[25] = {
        avg_ax,avg_ay,avg_az,avg_gx,avg_gy,avg_gz,
        std_ax,std_ay,std_az,std_gx,std_gy,std_gz,
        max_ax,max_ay,max_az,max_gx,max_gy,max_gz,
        min_ax,min_ay,min_az,min_gx,min_gy,min_gz,
        hand,
    };
    uTensor::Tensor input_data = new uTensor::RomTensor({25, 1}, flt, data);

    _model->set_inputs({{Mlp_hw_model::input_0, input_data}})
        .set_outputs({{Mlp_hw_model::output_0, _actions}})
        .eval();

    return action_to_label();
}

Label Label_Predictor::action_to_label()
{
    float max_value = static_cast<float>(_actions(0));
    int max_index = 0;

    Serial a(USBTX, USBRX);
    a.printf("\nResults: ");
    for (uint32_t i = 0; i < (*_actions)->num_elems(); ++i)
    {
        float value = static_cast<float>(_actions(i));
        a.printf("%d:%f ", i, value);
        if (value > max_value)
        {
            max_value = value;
            max_index = i;
        }
    }
    a.printf("\n");
    return Label(max_index);
}