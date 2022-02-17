#ifndef LOG_H_
#define LOG_H_

#include "config.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#ifdef LOG_ENABLED

#define log_info(...) _log_impl(__VA_ARGS__)
#define log_error(...) _log_impl(__VA_ARGS__)
    void _log_impl(const char *format, ...);

#else

#define log_info(...)
#define log_error(...)

#endif // LOG_ENABLED

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // LOG_H_