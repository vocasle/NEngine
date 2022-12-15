#include "NEngine/Engine.h"

#include "NEngine/Utils/Utils.h"

using namespace NEngine::Utils;

namespace NEngine {
Engine::Engine(int argc, const char* argv[])
{
	UtilsDebugPrint("Engine init with args:\n");
	for (int i = 0; i < argc; ++i) {
		UtilsDebugPrint("[%d]: %s\n",i, argv[i]);
	}
}
}