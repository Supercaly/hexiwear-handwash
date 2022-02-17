#include "sample_matrix.h"

#include <string.h>

Sample_Matrix::Sample_Matrix() {}

Sample_Matrix::~Sample_Matrix() {}

Sample_Matrix::Sample_Matrix(const Sample_Matrix &other)
{
    memcpy(ax, other.ax, SAMPLE_MATRIX_ELEMENT_SIZE);
    memcpy(ay, other.ay, SAMPLE_MATRIX_ELEMENT_SIZE);
    memcpy(az, other.az, SAMPLE_MATRIX_ELEMENT_SIZE);
    memcpy(gx, other.gx, SAMPLE_MATRIX_ELEMENT_SIZE);
    memcpy(gy, other.gy, SAMPLE_MATRIX_ELEMENT_SIZE);
    memcpy(gz, other.gz, SAMPLE_MATRIX_ELEMENT_SIZE);
}