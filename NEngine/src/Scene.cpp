#include "NEngine/Scene.h"

namespace NEngine {
auto
Scene::AddToScene(ECS::Entity entity) -> void
{
    mEntities.push_back(entity);
}
auto
Scene::RemoveFromScene(ECS::Entity entity) -> void
{
    auto it = std::find(std::begin(mEntities), std::end(mEntities), entity);
    if (it != std::end(mEntities)) {
        mEntities.erase(it);
    }
}
}  // namespace NEngine