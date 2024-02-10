#include "nengine.h"

#include <stdio.h> // TODO: Replace with in-house print functionality

DLLEXPORT void ne_print_version(void)
{
	printf("NEngine %d.\%d.%d\n", NENGINE_VERSION_MAJOR, NENGINE_VERSION_MINOR, NENGINE_VERSION_PATCH);
}
