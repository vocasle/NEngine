#include "NEngine/Engine.h"
#include "NEngine/Utils/Utils.h"
#include "Sandbox/Game.h"

auto
main(int argc, const char *argv[]) -> int
{
    NEngine::Engine engine(argc, argv);

    auto game = std::make_unique<MyGame>();
    engine.PlayGame(*game);
    engine.Deinit();
}