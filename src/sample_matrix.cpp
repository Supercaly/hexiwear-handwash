#include "sample_matrix.h"

#include <string.h>

Sample_Matrix::Sample_Matrix() {}

Sample_Matrix::~Sample_Matrix() {}

Sample_Matrix::Sample_Matrix(const Sample_Matrix &other)
{
    memcpy(_ax, other._ax, SAMPLE_MATRIX_ELEMENT_SIZE);
    memcpy(_ay, other._ay, SAMPLE_MATRIX_ELEMENT_SIZE);
    memcpy(_az, other._az, SAMPLE_MATRIX_ELEMENT_SIZE);
    memcpy(_gx, other._gx, SAMPLE_MATRIX_ELEMENT_SIZE);
    memcpy(_gy, other._gy, SAMPLE_MATRIX_ELEMENT_SIZE);
    memcpy(_gz, other._gz, SAMPLE_MATRIX_ELEMENT_SIZE);
}