#ifndef SVM_H_
#define SVM_H_

#include "libsvm/svm.h"

#include "raw_sensor_data.h"
#include "label.h"

class SVM
{
public:
    SVM();
    ~SVM();

    bool init(const char *model_path);

    void predict(RawSensorData *raw, Label *label);

private:
    svm_model *_model;
    struct svm_node _nodes[24];
};

#endif // SVM_H_