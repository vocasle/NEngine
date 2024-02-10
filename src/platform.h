#pragma once

#include "defines.h"

struct NE_Window;
NE_API struct NE_Window *ne_platform_create_window(const i8 *title, i16 x, i16 y, u16 width, u16 height);
NE_API void ne_platform_destroy_window(struct NE_Window *w);
NE_API b8 ne_platform_pump_messages(struct NE_Window *w);
