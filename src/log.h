#ifndef _LOG_H_
#define _LOG_H_

#define LOG_ENABLED

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#ifdef LOG_ENABLED
#define log_info(...) _log(__VA_ARGS__)
#define log_error(...) _log(__VA_ARGS__)
    void _log(const char *format, ...);

#else
#define info(...)
#define error(...)
#endif // LOG_ENABLED

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _LOG_H_