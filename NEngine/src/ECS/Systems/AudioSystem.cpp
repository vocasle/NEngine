#include "NEngine/ECS/Systems/AudioSystem.h"

namespace NEngine::ECS::Systems {
AudioSystem::AudioSystem(DefaultEntityManager &entityManager)
    : mEntityManager(&entityManager)
{
}
auto
AudioSystem::Update(float dt) -> void
{
    for (auto entity : mEntities) {
        auto &ac =
            *mEntityManager->GetComponent<Components::AudioComponent>(entity);
        if (ac.IsPlaying) {
            PlayAudio(ac.Path);
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
void
AudioSystem::PlayAudio(const std::string &path)
{
    UTILS_PRINTLN("Playing: %s", path.c_str());
    PlaySound(Utils::UtilsStrToWstr(path).c_str(), nullptr, SND_FILENAME);
}
}  // namespace NEngine::ECS::Systems