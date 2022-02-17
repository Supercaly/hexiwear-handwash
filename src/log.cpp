#include "log.h"

#include "mbed.h"

Serial _serial_out(USBTX, USBRX);

void _log_impl(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    _serial_out.vprintf(format, args);
    va_end(args);
}
