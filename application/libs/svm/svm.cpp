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

void SVM::predict(float *features, size_t features_size, Label *label)
{
    reset_malloc_count();
    for (int i = 0; i < features_size; i++)
    {
        _nodes[i].index = i + 1;
        _nodes[i].value = features[i];
    }

    double pred = svm_predict(_model, _nodes);
    *label = label_from_double(pred);
}