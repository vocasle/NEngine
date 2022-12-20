#pragma once

#include "Entity.h"

namespace NEngine::ECS {
template <typename T>
struct ComponentData
{
    ComponentData()
        : ComponentData(0)
    {
    }
    ComponentData(Entity entity)
        : Entity(entity),
          Component()
    {
    }
    Entity Entity;
    T Component;
};

template <typename... Ts>
class Repo
{
public:
    template <typename T>
    using cd = ComponentData<T>;

    template <typename T>
    using comp_vec = std::vector<cd<T>>;

    template <typename T>
    [[nodiscard]] auto
    CreateComponent(Entity entity) -> T &
    {
        auto &vec = GetVec<T>();
        vec.push_back(cd<T>(entity));
        return vec.at(vec.size() - 1).Component;
    }

    template <typename T>
    [[nodiscard]] auto
    GetComponent(Entity entity) noexcept -> T *
    {
        auto &vec = GetVec<T>();
        for (auto &el : vec) {
            if (el.Entity == entity) {
                return &el.Component;
            }
        }
        return nullptr;
    }

#if !DEBUG_ENTITY_MANAGER
private:
#endif

    template <typename T>
    auto
    GetVec() noexcept -> comp_vec<T> &
    {
        return std::get<comp_vec<T>>(mData);
    }

    template <typename T>
    auto
    GetVec() const noexcept -> const comp_vec<T> &
    {
        return std::get<comp_vec<T>>(mData);
    }

    std::tuple<comp_vec<Ts>...> mData;
};
}  // namespace NEngine::ECS
