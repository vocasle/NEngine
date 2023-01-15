#pragma once

#include "../EntityManager.h"

namespace NEngine::ECS::Systems {
class AudioSystem
{
public:
    explicit AudioSystem(DefaultEntityManager &entityManager);
    auto Update(float dt) -> void;
    auto RegisterEntity(Entity entity) -> void;
    auto UnregisterEntity(Entity entity) -> void;

private:
    void PlayAudio(const std::string &path);

    std::vector<ECS::Entity> mEntities;
    DefaultEntityManager *mEntityManager;
};
}  // namespace NEngine::ECS::Systems