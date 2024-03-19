#pragma once

#include "defines.h"

#if NE_DEBUG
void *ne_allocate_debug(u64 size, const i8 *file, i32 line);
void ne_free_debug(void *ptr, const i8 *file, i32 line);
#define ne_allocate(size) ne_allocate_debug(size, __FILE__, __LINE__)
#define ne_free(ptr) ne_free_debug(ptr, __FILE__, __LINE__)
#endif
