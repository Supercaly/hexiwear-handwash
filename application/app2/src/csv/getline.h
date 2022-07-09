#ifndef GETLINE_H_
#define GETLINE_H_

#include "mbed.h"

// Custom implementation of POSIX getline because for some reason we can't compile
// using the standard one.
ssize_t _getline(char **lineptr, size_t *n, FILE *stream);

#endif // GETLINE_H_