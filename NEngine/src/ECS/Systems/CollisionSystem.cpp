#include "NEngine/ECS/Systems/CollisionSystem.h"

#include "NEngine/Math/MathUtils.h"

#if NENGINE_USE_DIRECTXMATH
#include <DirectXCollision.h>
using namespace DirectX;
#endif

using namespace NEngine::Math;

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
                if (col1.OnCollision) {
                    col1.OnCollision(e1, e2);
                }
                if (col2.OnCollision) {
                    col2.OnCollision(e2, e1);
                }
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

#if NENGINE_USE_DIRECTXMATH
    const auto bb = BoundingBox(reinterpret_cast<const XMFLOAT3 &>(box.Center),
                                reinterpret_cast<const XMFLOAT3 &>(box.Size));
    const auto containmentType =
        bb.Contains(XMLoadFloat3(reinterpret_cast<const XMFLOAT3 *>(&point)));
    isOverlaping = containmentType == ContainmentType::CONTAINS;
#endif

    return isOverlaping;
}
bool
CollisionSystem::IsOverlaping(const Components::CollisionComponent &lhs,
                              const Components::CollisionComponent &rhs) const
{
    bool isOverlaping = false;
#if NENGINE_USE_DIRECTXMATH
    const auto lhsBb =
        BoundingBox(reinterpret_cast<const XMFLOAT3 &>(lhs.Center),
                    reinterpret_cast<const XMFLOAT3 &>(lhs.Size));
    const auto rhsBb =
        BoundingBox(reinterpret_cast<const XMFLOAT3 &>(rhs.Center),
                    reinterpret_cast<const XMFLOAT3 &>(lhs.Size));
    isOverlaping = lhsBb.Intersects(rhsBb);
#endif
    return isOverlaping;
}

Components::CollisionComponent
CollisionSystem::MoveBox(const Math::vec3 &offset,
                         const Components::CollisionComponent &box) const
{
    auto movedBox = Components::CollisionComponent();
    movedBox.Center = offset;
    movedBox.Size = box.Size;
    return movedBox;
}
}  // namespace NEngine::ECS::Systems