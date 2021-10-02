#ifndef _LOG_H_
#define _LOG_H_

#define PRINT_LOG

#ifdef PRINT_LOG
#include "mbed.h"
Serial serial_out(USBTX, USBRX);
#endif // PRINT_LOG

#ifdef PRINT_LOG
#define log(...) serial_out.printf(__VA_ARGS__)
#else
#define log(...)
#endif // PRINT_LOG

#ifdef PRINT_LOG
#define error(...) serial_out.printf(__VA_ARGS__)
#else
#define error(...)
#endif // PRINT_LOG

#endif // _LOG_H_