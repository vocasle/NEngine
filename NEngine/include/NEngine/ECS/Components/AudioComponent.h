#pragma once

#include <string>

namespace NEngine::ECS::Components {

struct AudioComponent
{
    bool IsPlaying;
    std::string Path;
};

}  // namespace NEngine::ECS::Components