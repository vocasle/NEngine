#include "nengine.h"
#include "platform.h"

#include <stdio.h> // TODO: Replace with in-house print functionality
#include <stdarg.h>
#include <stdlib.h>

NE_API void ne_print_version(void)
{
	ne_println_info("NEngine %d.%d.%d", NE_VERSION_MAJOR, NE_VERSION_MINOR, NE_VERSION_PATCH);

	i32 *ints = ne_platform_allocate(16);
	ne_platform_zero_memory(ints, 16);
	for (i32 i = 0; i < 4; ++i) {
		ints[i] = i;
	}
	i8 *bytes = (i8 *)ints;
	*(bytes - 1) = 'A';
	ne_platform_free(ints);
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


NE_API void ne_assert(u8 cond, const i8 *expr, const i8 *msg, const i8 *file, i32 line)
{
	if (!cond) {
		ne_println_error("Assertion \"%s\" failed at %s:%d. %s", expr, file, line, msg ? msg : "");
		ne_platform_terminate();
	}
}

NE_API struct NE_Library *ne_load_library(const i8 *name)
{
	return ne_platform_load_library(name);
}

NE_API void *ne_get_proc_address(struct NE_Library *lib, const i8 *proc)
{
	return ne_platform_get_proc_address(lib, proc);
}

NE_API  void ne_destroy_library(struct NE_Library *lib)
{
	ne_platform_destroy_library(lib);
}

