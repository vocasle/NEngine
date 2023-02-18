#pragma once

#include <bitset>
#include <cassert>
#include <functional>
#include <unordered_map>

#include "Components/AnimationComponent.h"
#include "Components/AudioComponent.h"
#include "Components/CameraComponent.h"
#include "Components/CollisionComponent.h"
#include "Components/InputComponent.h"
#include "Components/PositionComponent.h"
#include "Components/RenderComponent.h"
#include "Entity.h"
#include "Repo.h"

namespace NEngine::ECS {

// Taken from https://stackoverflow.com/a/18063608/3846281
template <class Component, class Tuple>
struct Index;

template <class Component, class... Types>
struct Index<Component, std::tuple<Component, Types...>>
{
    static const std::size_t value = 0;
};

template <class Component, class U, class... Types>
struct Index<Component, std::tuple<U, Types...>>
{
    static const std::size_t value =
        1 + Index<Component, std::tuple<Types...>>::value;
};

template <typename... Components>
class EntityManager
{
public:
    [[nodiscard]] auto
    CreateEntity() noexcept -> Entity
    {
        assert(mEntityID + 1 < std::numeric_limits<long>::max() &&
               "Max entities reached");
        mEntities.insert(std::make_pair(mEntityID, std::bitset<64>()));
        return mEntityID++;
    }

    template <typename Component>
    [[nodiscard]] auto
    HasComponent(Entity entity) const noexcept -> bool
    {
        CheckEntityExists(entity);
        auto componentMask = Bitmask<Component>().to_ulong();
        return (mEntities.at(entity).to_ulong() & componentMask) ==
               componentMask;
    }

    typedef std::tuple<Components...> tuple_t;

    template <typename Component>
    [[nodiscard]] constexpr static auto
    Bitmask() noexcept -> std::bitset<64>
    {
        auto idx = Index<Component, tuple_t>::value;
        auto bits = std::bitset<64>();
        bits.set(idx);
        return bits;
    }

    template <typename Component>
    [[nodiscard]] auto
    CreateComponent(Entity entity) -> Component &
    {
        auto &component = mRepo.CreateComponent<Component>(entity);
        UpdateComponentMask<Component>(entity);
        NotifyComponentAdd<Component>(entity);
        return component;
    }

    template <typename Component>
    [[nodiscard]] auto
    GetComponent(Entity entity) -> Component *
    {
        return mRepo.GetComponent<Component>(entity);
    }

    template <typename Component>
    [[nodiscard]] auto
    GetComponentsOfType() noexcept -> std::vector<ComponentData<Component>> &
    {
        return mRepo.GetVec<Component>();
    }

#if DEBUG_ENTITY_MANAGER
    template <typename Component>
    [[nodiscard]] auto
    GetNumComponentsOfType() const noexcept -> size_t
    {
        return mRepo.GetVec<Component>().size();
    }
#endif

    auto
    RegisterOnComponentAddCallback(std::function<void(Entity)> callback) -> void
    {
        mComponentAddCallbacks.push_back(callback);
    }
    auto
    RegisterOnComponentRemoveCallback(std::function<void(Entity)> callback)
        -> void
    {
        mComponentRemoveCallbacks.push_back(callback);
    }

    auto
    UnregisterOnComponentAddCallback(std::function<void(Entity)> callback)
        -> void
    {
        auto it = std::find(std::begin(mComponentAddCallbacks),
                            std::end(mComponentAddCallbacks),
                            callback);
        mComponentAddCallbacks.erase(it);
    }
    auto
    UnregisterOnComponentRemoveCallback(std::function<void(Entity)> callback)
        -> void
    {
        auto it = std::find(std::begin(mComponentRemoveCallbacks),
                            std::end(mComponentRemoveCallbacks),
                            callback);
        mComponentRemoveCallbacks.erase(it);
    }

    [[nodiscard]] auto
    GetBitmask(Entity entity) -> unsigned long
    {
        // UTILS_PRINTLN("Bitmask: %s for entity: %d",
        //               mEntities.at(entity).to_string().c_str(),
        //               entity);
        return mEntities.at(entity).to_ulong();
    }

private:
    template <typename Component>
    auto
    NotifyComponentAdd(Entity entity) -> void
    {
        for (auto &cb : mComponentAddCallbacks) {
            cb(entity);
        }
    }

    template <typename Component>
    auto
    NotifyComponentRemove(Entity entity) -> void
    {
        for (auto &cb : mComponentRemoveCallbacks) {
            cb(entity);
        }
    }

    auto
    CheckEntityExists(Entity entity) const noexcept -> void
    {
        assert(mEntities.find(entity) != mEntities.end() && "Invalid entity");
    }

    template <typename Component>
    auto
    UpdateComponentMask(Entity entity) noexcept -> void
    {
        CheckEntityExists(entity);
        const auto oldMask = mEntities.at(entity);
        auto component = Bitmask<Component>();
        const auto newMask = oldMask | component;
        // UTILS_PRINTLN("\n[%ld]\nOld mask: %s\n, new mask: %s",
        //               entity,
        //               oldMask.to_string().c_str(),
        //               newMask.to_string().c_str());
        mEntities.at(entity) = newMask;
    }

    Repo<Components...> mRepo;

    std::unordered_map<Entity, std::bitset<64>> mEntities;
    Entity mEntityID = 0;

    using OnComponentAddCallback = std::function<void(Entity)>;
    using OnComponentRemoveCallback = std::function<void(Entity)>;

    std::vector<OnComponentAddCallback> mComponentAddCallbacks;
    std::vector<OnComponentAddCallback> mComponentRemoveCallbacks;
};

using DefaultEntityManager = EntityManager<Components::PositionComponent,
                                           Components::RenderComponent,
                                           Components::InputComponent,
                                           Components::CameraComponent,
                                           Components::CollisionComponent,
                                           Components::AudioComponent,
                                           Components::AnimationComponent>;

enum ComponentType {
    ComponentType_POSITION = 1 << 0,
    ComponentType_RENDER = 1 << 1,
    ComponentType_INPUT = 1 << 2,
    ComponentType_CAMERA = 1 << 3,
    ComponentType_COLLISION = 1 << 4,
    ComponentType_AUDIO = 1 << 5,
    ComponentType_ANIMATION = 1 << 6,
};
}  // namespace NEngine::ECS
