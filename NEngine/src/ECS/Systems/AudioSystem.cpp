#include "NEngine/ECS/Systems/AudioSystem.h"

#include <chrono>
#include <mutex>

using namespace NEngine::Utils;

std::mutex GAudioMtx;

namespace NEngine::ECS::Systems {
AudioSystem::AudioSystem(DefaultEntityManager &entityManager)
    : mEntityManager(&entityManager),
      mKeepAlive(true)
{
    UTILS_PRINTLN("AudioSystem was created on thread: %s",
                  UtilsGetThreadIdAsStr().c_str());

    mAudioThread = std::thread(
        [this]
        {
            UTILS_PRINTLN("Spawning audio thread: %s",
                          UtilsGetThreadIdAsStr().c_str());
            while (mKeepAlive) {
                GAudioMtx.lock();
                const auto sz = mAudioQueue.size();
                GAudioMtx.unlock();

                if (sz > 0) {
                    GAudioMtx.lock();
                    const auto path = mAudioQueue.front();
                    mAudioQueue.pop();
                    GAudioMtx.unlock();
                    PlayAudio(path);
                }
                else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(16));
                }
            }
        });
}
auto
AudioSystem::Update(float dt) -> void
{
    for (auto entity : mEntities) {
        auto &ac =
            *mEntityManager->GetComponent<Components::AudioComponent>(entity);
        if (ac.IsPlaying) {
            AddToQueue(ac.Path);
            ac.IsPlaying = false;
        }
    }
}
auto
AudioSystem::RegisterEntity(Entity entity) -> void
{
    auto it = std::find(std::begin(mEntities), std::end(mEntities), entity);
    if (it != std::end(mEntities)) {
        UTILS_PRINTLN("Entity %ld is already tracked by audio system", entity);
        return;
    }
    if (mEntityManager->HasComponent<Components::AudioComponent>(entity)) {
        mEntities.push_back(entity);
    }
}
auto
AudioSystem::UnregisterEntity(Entity entity) -> void
{
    auto it = std::find(std::begin(mEntities), std::end(mEntities), entity);
    if (it != std::end(mEntities)) {
        mEntities.erase(it);
    }
}
AudioSystem::~AudioSystem()
{
    mKeepAlive = false;
    mAudioThread.join();
}
void
AudioSystem::PlayAudio(const std::string &path)
{
    UTILS_PRINTLN("Playing: %s on thread %s",
                  path.c_str(),
                  UtilsGetThreadIdAsStr().c_str());
    PlaySound(UtilsStrToWstr(path).c_str(), nullptr, SND_FILENAME);
}
void
AudioSystem::AddToQueue(const std::string &path)
{
    GAudioMtx.lock();
    mAudioQueue.push(path);
    GAudioMtx.unlock();
}
}  // namespace NEngine::ECS::Systems