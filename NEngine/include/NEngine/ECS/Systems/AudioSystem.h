#pragma once

#include <wrl/client.h>
#include <xaudio2.h>

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
    HRESULT
    OpenAudioFile(const std::string &path);
    HRESULT PlayAudio();
    void PlayQueuedFiles();

    std::vector<ECS::Entity> mEntities;
    DefaultEntityManager *mEntityManager;

    std::queue<std::string> mAudioQueue;
    std::set<std::string> mFilesAlreadyInQueue;
    //std::thread mAudioThread;
    //bool mKeepAlive;
    Microsoft::WRL::ComPtr<IXAudio2> mXAudio;
    IXAudio2MasteringVoice *mMasterVoice;

    struct XAudioData
    {
        WAVEFORMATEXTENSIBLE wfx;
        XAUDIO2_BUFFER buffer;
        std::unique_ptr<unsigned char[]> rawBuffer;
    } mXAudioData;
};
}  // namespace NEngine::ECS::Systems