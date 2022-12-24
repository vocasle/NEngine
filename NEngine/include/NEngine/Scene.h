#pragma once

#include <functional>
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

    auto Visit(std::function<void(const ECS::GameObject &)> callback) const
        -> void;

private:
    std::vector<ECS::GameObject> mEntities;
};

}  // namespace NEngine