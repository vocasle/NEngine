#pragma once

#include <vector>

#include "ECS/Entity.h"

namespace NEngine {

class Scene
{
    auto AddToScene(ECS::Entity entity) -> void;
    auto RemoveFromScene(ECS::Entity entity) -> void;

private:
    std::vector<ECS::Entity> mEntities;
};

}  // namespace NEngine