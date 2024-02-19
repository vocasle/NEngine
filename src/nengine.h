#pragma once

#include "defines.h"

NE_API void ne_print_version(void);

NE_API void ne_println_info(const i8 *fmt, ...);
NE_API void ne_println_debug(const i8 *fmt, ...);
NE_API void ne_println_warning(const i8 *fmt, ...);
NE_API void ne_println_error(const i8 *fmt, ...);
