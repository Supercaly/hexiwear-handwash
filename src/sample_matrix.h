#ifndef SAMPLE_MATRIX_H_
#define SAMPLE_MATRIX_H_

// TODO: put put the real value in a global config file.
#define SAMPLE_MATRIX_ELEMENT_SIZE 1000

class Sample_Matrix
{
public:
    Sample_Matrix();
    ~Sample_Matrix();
    Sample_Matrix(const Sample_Matrix &other);

    float ax[SAMPLE_MATRIX_ELEMENT_SIZE];
    float ay[SAMPLE_MATRIX_ELEMENT_SIZE];
    float az[SAMPLE_MATRIX_ELEMENT_SIZE];
    float gx[SAMPLE_MATRIX_ELEMENT_SIZE];
    float gy[SAMPLE_MATRIX_ELEMENT_SIZE];
    float gz[SAMPLE_MATRIX_ELEMENT_SIZE];
};

#endif // SAMPLE_MATRIX_H_