#include "NEngine/Engine.h"
#include "NEngine/Math/MathTest.h"
#include "NEngine/Utils/Utils.h"
#include "Sandbox/Game.h"

auto
main(int argc, const char *argv[]) -> int
{
    using namespace NEngine::Math;

    MathTest();

    // NEngine::Engine engine(argc, argv);

    // std::unique_ptr<NEngine::Game> game = std::make_unique<MyGame>();
    // engine.PlayGame(*game);
    // engine.Deinit();
}