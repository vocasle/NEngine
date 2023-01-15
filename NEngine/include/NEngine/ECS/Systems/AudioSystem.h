#pragma once

#include <queue>
#include <thread>

#include "../EntityManager.h"

namespace NEngine::ECS::Systems {
class AudioSystem
{
public:
    explicit AudioSystem(DefaultEntityManager &entityManager);
    auto Update(float dt) -> void;
    auto RegisterEntity(Entity entity) -> void;
    auto UnregisterEntity(Entity entity) -> void;
    ~AudioSystem();

private:
    void PlayAudio(const std::string &path);
    void AddToQueue(const std::string &path);

    std::vector<ECS::Entity> mEntities;
    DefaultEntityManager *mEntityManager;

    std::queue<std::string> mAudioQueue;
    std::thread mAudioThread;
    bool mKeepAlive;
};
}  // namespace NEngine::ECS::Systems