#include "NEngine/ECS/Systems/AudioSystem.h"

#include <chrono>
#include <mutex>

std::mutex GAudioMtx;

namespace NEngine::ECS::Systems {
AudioSystem::AudioSystem(DefaultEntityManager &entityManager)
    : mEntityManager(&entityManager)
{
    mAudioThread = std::thread(
        [this]
        {
            while (mKeepAlive) {
                GAudioMtx.lock();
                if (mAudioQueue.size() > 0) {
                    const auto path = mAudioQueue.front();
                    PlayAudio(path);
                    mAudioQueue.pop();
                }
                else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(16));
                }
                GAudioMtx.unlock();
            }
        });

    mAudioThread.join();
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
}
void
AudioSystem::PlayAudio(const std::string &path)
{
    UTILS_PRINTLN("Playing: %s", path.c_str());
    PlaySound(Utils::UtilsStrToWstr(path).c_str(), nullptr, SND_FILENAME);
}
void
AudioSystem::AddToQueue(const std::string &path)
{
    GAudioMtx.lock();
    mAudioQueue.push(path);
    GAudioMtx.unlock();
}
}  // namespace NEngine::ECS::Systems