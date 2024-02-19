#include "app.h"
#include "platform.h"
#include "nengine.h"

#include <stdlib.h>

struct NE_App
{
	const i8 *title;
	struct NE_Window *wnd;
};

NE_API struct NE_App *ne_app_new(const struct NE_AppConfig *c)
{
	struct NE_App *app = malloc(sizeof *app);
	app->title = c->app_name;
	struct NE_Window *w = ne_platform_create_window(c->app_name, 0, 0, 800, 600);
	app->wnd = w;
	return app;
}

NE_API void ne_app_run(struct NE_App *app)
{
	ne_print_version();
	ne_platform_pump_messages(app->wnd);	
}

NE_API void ne_app_destroy(struct NE_App *app)
{
	ne_platform_destroy_window(app->wnd);
	free(app);
}



