#include "NEngine/Scene.h"

namespace NEngine {

auto
Scene::AddToScene(const ECS::GameObject &entity) -> void
{
    mEntities.push_back(entity);
}
auto
Scene::RemoveFromScene(ECS::Entity entityID) -> void
{
    auto it = std::find_if(std::begin(mEntities),
                           std::end(mEntities),
                           [entityID](const ECS::GameObject &gameObject) -> bool
                           { return gameObject.ID == entityID; });
    if (it != std::end(mEntities)) {
        mEntities.erase(it);
    }
}
auto
Scene::RemoveFromScene(const std::string &entityName) -> void
{
    auto it =
        std::find_if(std::begin(mEntities),
                     std::end(mEntities),
                     [entityName](const ECS::GameObject &gameObject) -> bool
                     { return gameObject.Name == entityName; });
    if (it != std::end(mEntities)) {
        mEntities.erase(it);
    }
}

auto
Scene::FindEntityByName(const std::string &entityName) -> ECS::GameObject *
{
    auto it =
        std::find_if(std::begin(mEntities),
                     std::end(mEntities),
                     [entityName](const ECS::GameObject &gameObject) -> bool
                     { return gameObject.Name == entityName; });
    return it == std::end(mEntities) ? nullptr : &*it;
}

auto
Scene::Visit(std::function<void(const ECS::GameObject &)> callback) const
    -> void
{
    for (auto &go : mEntities) {
        callback(go);
    }
}

}  // namespace NEngine