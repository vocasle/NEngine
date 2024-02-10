#include "nengine.h"

#include <stdio.h> // TODO: Replace with in-house print functionality

NE_API void ne_print_version(void)
{
	printf("NEngine %d.%d.%d\n", NE_VERSION_MAJOR, NE_VERSION_MINOR, NE_VERSION_PATCH);
}
