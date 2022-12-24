#include "NEngine/Engine.h"
#include "Sandbox/Game.h"

auto
main(int argc, const char *argv[]) -> int
{
    NEngine::Engine engine(argc, argv);

    std::unique_ptr<NEngine::Game> game = std::make_unique<MyGame>();
    engine.PlayGame(*game);
    engine.Deinit();
}