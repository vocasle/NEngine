#include "nengine.h"
#include "platform.h"

#include <stdio.h> // TODO: Replace with in-house print functionality
#include <stdarg.h>
#include <stdlib.h>

NE_API void ne_print_version(void)
{
	ne_println_info("NEngine %d.%d.%d", NE_VERSION_MAJOR, NE_VERSION_MINOR, NE_VERSION_PATCH);
}

static i32 ne_buffer_size(const i8 *fmt, va_list args)
{
	i32 size = vsnprintf(NULL, 0, fmt, args);
	return size + 1;
}

static void ne_println(enum NE_LogLevel level, const i8 *fmt, va_list args)
{
	const i32 buffer_sz = ne_buffer_size(fmt, args);
	i8 *buf = malloc(buffer_sz);
	vsnprintf(buf, buffer_sz, fmt, args);
	ne_platform_println(buf, level);
}

NE_API void ne_println_info(const i8 *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	ne_println(NE_LOG_LEVEL_INFO, fmt, args);
	va_end(args);
}

NE_API void ne_println_debug(const i8 *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	ne_println(NE_LOG_LEVEL_DEBUG, fmt, args);
	va_end(args);
}

NE_API void ne_println_warning(const i8 *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	ne_println(NE_LOG_LEVEL_WARNING, fmt, args);
	va_end(args);
}

NE_API void ne_println_error(const i8 *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	ne_println(NE_LOG_LEVEL_ERROR, fmt, args);
	va_end(args);
}
