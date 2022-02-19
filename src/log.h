#ifndef LOG_H_
#define LOG_H_

#include "config.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#ifdef LOG_ENABLED

    void _log_impl(const char *format, ...);
#define log_info(format, ...) _log_impl("\033[33m[INFO]:\033[0m " format, ##__VA_ARGS__)
#define log_error(format, ...) _log_impl("\033[31m[ERROR]:\033[0m " format, ##__VA_ARGS__)

#else

#define log_info(...) \
    do                \
    {                 \
    } while (0)
#define log_error(...) \
    do                 \
    {                  \
    } while (0)

#endif // LOG_ENABLED

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // LOG_H_