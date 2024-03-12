#pragma once

#include "defines.h"

NE_API void ne_print_version(void);

NE_API void ne_println_info(const i8 *fmt, ...);
NE_API void ne_println_debug(const i8 *fmt, ...);
NE_API void ne_println_warning(const i8 *fmt, ...);
NE_API void ne_println_error(const i8 *fmt, ...);
NE_API void ne_assert(u8 cond, const i8 *expr, const i8 *msg, const i8 *file, i32 line);

struct NE_Library;
NE_API struct NE_Library *ne_load_library(const i8 *name);
NE_API void *ne_get_proc_address(struct NE_Library *lib, const i8 *proc);
NE_API  void ne_destroy_library(struct NE_Library *lib);

#define NE_LOG_DEBUG_ENABLED 1
#define NE_LOG_INFO_ENABLED 1
#define NE_ASSERT_ENABLED 1

#if NE_RELEASE
#define NE_LOG_DEBUG_ENABLED 0
#define NE_LOG_INFO_ENABLED 0
#define NE_ASSERT_ENABLED 0
#endif

#define NE_NOOP do {} while (NE_FALSE)

#if NE_LOG_INFO_ENABLED
#define NE_LOG_INFO(x, ...) ne_println_info((x), __VA_ARGS__)
#else
#define NE_LOG_INFO(x, ...) NE_NOOP
#endif

#if NE_LOG_DEBUG_ENABLED
#define NE_LOG_DEBUG(x, ...) ne_println_debug((x), __VA_ARGS__)
#else
#define NE_LOG_DEBUG(x, ...) NE_NOOP
#endif

#define NE_LOG_WARNING(x, ...) ne_println_warning((x), __VA_ARGS__)
#define NE_LOG_ERROR(x, ...) ne_println_error((x), __VA_ARGS__)

#if NE_ASSERT_ENABLED
#define NE_ASSERT(x) ne_assert(!!(x), #x, NULL, __FILE__, __LINE__)
#define NE_ASSERT_MSG(x, msg) ne_assert(!!(x), #x, (msg), __FILE__, __LINE__)
#else
#define NE_ASSERT(x) NE_NOOP
#define NE_ASSERT_MSG(x, msg) NE_NOOP
#endif
