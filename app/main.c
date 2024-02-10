#include "nengine.h"
#include "platform.h"

int main(void)
{
	ne_print_version();
	struct NE_Window *w = ne_platform_create_window("NEngine", 0, 0, 1280, 720);
  ne_platform_pump_messages(w);
	ne_platform_destroy_window(w);
	return 0;
}
