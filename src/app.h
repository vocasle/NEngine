#pragma once

#include "defines.h"

struct NE_App;

struct NE_AppConfig
{
	const i8 *app_name;
	u16 win_width;
	u16 win_height;
};

NE_API struct NE_App *ne_app_new(const struct NE_AppConfig *c);
NE_API void ne_app_run(struct NE_App *app);
NE_API void ne_app_destroy(struct NE_App *app);


