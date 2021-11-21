#ifndef SAMPLE_MATRIX_H
#define SAMPLE_MATRIX_H

// TODO: put put the real value in a global config file.
#define SAMPLE_MATRIX_ELEMENT_SIZE 1000

class Sample_Matrix
{
public:
    Sample_Matrix();
    ~Sample_Matrix();
    Sample_Matrix(const Sample_Matrix &other);

    float _ax[SAMPLE_MATRIX_ELEMENT_SIZE];
    float _ay[SAMPLE_MATRIX_ELEMENT_SIZE];
    float _az[SAMPLE_MATRIX_ELEMENT_SIZE];
    float _gx[SAMPLE_MATRIX_ELEMENT_SIZE];
    float _gy[SAMPLE_MATRIX_ELEMENT_SIZE];
    float _gz[SAMPLE_MATRIX_ELEMENT_SIZE];
};

#endif // SAMPLE_MATRIX_H