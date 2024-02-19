#include "app.h"
#include "nengine.h"

int main(void)
{
	const struct NE_AppConfig app_config = {
		.app_name = "TestApp",
		.win_width = 1280,
		.win_height = 720
	};
	struct NE_App *app = ne_app_new(&app_config);
	ne_app_run(app);
	ne_app_destroy(app);
	ne_println_info("INFO print: PI: %f, greetings: %s, number: %d, char: %c", 3.14, "hello", 5, 'A');
	ne_println_debug("DEBUG print: PI: %f, greetings: %s, number: %d, char: %c", 3.14, "hello", 5, 'A');
	ne_println_warning("WARNING print: PI: %f, greetings: %s, number: %d, char: %c", 3.14, "hello", 5, 'A');
	ne_println_error("ERROR print: PI: %f, greetings: %s, number: %d, char: %c", 3.14, "hello", 5, 'A');

	return 0;
}
