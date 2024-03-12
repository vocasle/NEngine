#include "app.h"
#include "nengine.h"
#include "nemath.h"

int main(void)
{
	const struct NE_AppConfig app_config = {
		.app_name = "TestApp",
		.win_width = 1280,
		.win_height = 720
	};
	struct NE_App *app = ne_app_new(&app_config);

	ne_math_print_version();

	ne_app_run(app);
	ne_app_destroy(app);
	return 0;
}
