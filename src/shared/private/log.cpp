#include <shared/log.h>

#include <shared/std.h>
#include <shared/string.h>
#include <shared/assert.h>

#include <stdarg.h>

void log_entry_va(LogSeverity severity, char const* format, va_list arg_list)
{
#define BUFFER_SIZE 4096
	// TODO(nathan): This should be thread_local
	static byte working_buffer[BUFFER_SIZE];
	UTF8StringBuilder builder = {working_buffer, BUFFER_SIZE, 0};
	UTF8StringBuilder_appendFormattedVA(&builder, format, arg_list);
	logger_func(severity, (char*)working_buffer, builder.write_index);
}

void log_entry(LogSeverity severity, char const* format, ...)
{
	va_list args;
	va_start(args, format);
	log_entry_va(severity, format, args);
	va_end(args);
}