#pragma once

#include <shared/std.h>

#ifdef __cplusplus
extern "C"
{
#endif

	typedef enum LogSeverity
	{
		LogSeverity_Info,
		LogSeverity_Success,
		LogSeverity_Warning,
		LogSeverity_Error,
	} LogSeverity;

	// You need to define this somewhere
	void logger_func(LogSeverity severity, char const* string, usize size_bytes);

	void log_entry(LogSeverity severity, char const* format, ...);

#define PAW_LOG_INFO(fmt, ...) log_entry(LogSeverity_Info, fmt, __VA_ARGS__)
#define PAW_LOG_SUCCESS(fmt, ...) log_entry(LogSeverity_Success, fmt, __VA_ARGS__)
#define PAW_LOG_WARNING(fmt, ...) log_entry(LogSeverity_Warning, fmt, __VA_ARGS__)
#define PAW_LOG_ERROR(fmt, ...) log_entry(LogSeverity_Error, fmt, __VA_ARGS__)

#ifdef __cplusplus
}
#endif