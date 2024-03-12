#pragma once

#include "defines.h"

struct NE_Window;
struct NE_Window *ne_platform_create_window(const i8 *title, i16 x, i16 y, u16 width, u16 height);
void ne_platform_destroy_window(struct NE_Window *w);
void ne_platform_pump_messages(struct NE_Window *w);

enum NE_LogLevel
{
	NE_LOG_LEVEL_INFO,
	NE_LOG_LEVEL_DEBUG,
	NE_LOG_LEVEL_WARNING,
	NE_LOG_LEVEL_ERROR,
};
void ne_platform_println(const i8 *msg, enum NE_LogLevel level);

void ne_platform_terminate(void);

struct NE_Library *ne_platform_load_library(const i8 *name);
void *ne_platform_get_proc_address(struct NE_Library *lib, const i8 *proc);
void ne_platform_destroy_library(struct NE_Library *lib);

#if NE_DEBUG
void *ne_platform_allocate_dbg(u64 size, const i8 *file, i32 line);
void ne_platform_free_dbg(void *ptr, const i8 *file, i32 line);

#define ne_platform_allocate(size) ne_platform_allocate_dbg(size, __FILE__, __LINE__)
#define ne_platform_free(ptr) ne_platform_free_dbg(ptr, __FILE__, __LINE__)
#else
void *ne_platform_allocate(u64 size);
void ne_platform_free(void *ptr);
#endif

void ne_platform_zero_memory(void *ptr, u64 size);
