#pragma once

#include <vector>

#include "ECS/Entity.h"

namespace NEngine {

class Scene
{
public:
    auto AddToScene(const ECS::GameObject &entity) -> void;
    auto RemoveFromScene(ECS::Entity entityID) -> void;
    auto RemoveFromScene(const std::string &entityName) -> void;
    [[nodiscard]] auto FindEntityByName(const std::string &entityName)
        -> ECS::GameObject *;

private:
    std::vector<ECS::GameObject> mEntities;
};

}  // namespace NEngine