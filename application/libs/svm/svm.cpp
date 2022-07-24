#include "svm.h"

#include "features.h"

SVM::SVM() {}

SVM::~SVM() {}

bool SVM::init(const char *model_path)
{
    _model = svm_load_model(model_path);
    if (_model == NULL)
    {
        return false;
    }
    return true;
}

void SVM::predict(RawSensorData *raw, Label *label)
{
     float features[24];
    compute_features(raw, features);

    for (int i = 0; i < 24; i++)
    {
        _nodes[i].index = i + 1;
        _nodes[i].value = features[i];
    }

    double pred = svm_predict(_model, _nodes);
    *label = label_from_double(pred);
}