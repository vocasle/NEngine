#include "NEngine/ECS/Systems/CollisionSystem.h"

namespace NEngine::ECS::Systems {
CollisionSystem::CollisionSystem(ECS::DefaultEntityManager &em)
    : mEntityManager(&em)
{
}
auto
CollisionSystem::Update(float dt) -> void
{
    std::set<Entity> collidedEntities;
    for (auto e1 : mEntities) {
        const auto &pos1 =
            *mEntityManager->GetComponent<Components::PositionComponent>(e1);
        const auto &col1 =
            *mEntityManager->GetComponent<Components::CollisionComponent>(e1);

        const auto lhs = MoveBox(pos1.Position, col1);

        for (auto e2 : mEntities) {
            if (e1 == e2 || (collidedEntities.contains(e2) &&
                             collidedEntities.contains(e1))) {
                continue;
            }

            const auto &pos2 =
                *mEntityManager->GetComponent<Components::PositionComponent>(
                    e2);
            const auto &col2 =
                *mEntityManager->GetComponent<Components::CollisionComponent>(
                    e2);

            const auto rhs = MoveBox(pos2.Position, col2);

            if (IsOverlaping(lhs, rhs)) {
                collidedEntities.insert(e1);
                collidedEntities.insert(e2);
            }
        }
    }
}
auto
CollisionSystem::RegisterEntity(Entity entity) -> void
{
    auto it = std::find(std::begin(mEntities), std::end(mEntities), entity);
    if (it != std::end(mEntities)) {
        UTILS_PRINTLN("Entity %ld is already tracked by collision system",
                      entity);
        return;
    }
    if (mEntityManager->HasComponent<Components::CollisionComponent>(entity)) {
        mEntities.push_back(entity);
    }
}
auto
CollisionSystem::UnregisterEntity(Entity entity) -> void
{
    auto it = std::find(std::begin(mEntities), std::end(mEntities), entity);
    if (it != std::end(mEntities)) {
        mEntities.erase(it);
    }
}
bool
CollisionSystem::IsOverlaping(const Math::vec3 &point,
                              const Components::CollisionComponent &box) const
{
    bool isOverlaping = false;

    isOverlaping = (box.BoxMin.X <= point.X && point.X <= box.BoxMax.X) &&
                   (box.BoxMin.Y <= point.Y && point.Y <= box.BoxMax.Y) &&
                   (box.BoxMin.Z <= point.Z && point.Z <= box.BoxMax.Z);

    return isOverlaping;
}
bool
CollisionSystem::IsOverlaping(const Components::CollisionComponent &lhs,
                              const Components::CollisionComponent &rhs) const
{
    bool isOverlaping = false;

    isOverlaping =
        (lhs.BoxMin.X <= rhs.BoxMax.X && lhs.BoxMax.X >= rhs.BoxMin.X) &&
        (lhs.BoxMin.Y <= rhs.BoxMax.Y && lhs.BoxMax.Y >= rhs.BoxMin.Y) &&
        (lhs.BoxMin.Z <= rhs.BoxMax.Z && lhs.BoxMax.Z >= rhs.BoxMin.Z);

    return isOverlaping;
}
Components::CollisionComponent
CollisionSystem::MoveBox(const Math::vec3 &offset,
                         const Components::CollisionComponent &box) const
{
    auto movedBox = Components::CollisionComponent();
    movedBox.BoxMin = box.BoxMin + offset;
    movedBox.BoxMax = box.BoxMax + offset;
    return movedBox;
}
}  // namespace NEngine::ECS::Systems