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

