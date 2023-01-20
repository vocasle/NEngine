#pragma once

#include <wrl/client.h>
#include <xaudio2.h>

#include <queue>
#include <thread>

#include "../EntityManager.h"

namespace NEngine::ECS::Systems {

class AudioQueue
{
public:
    void Push(std::string path);
    std::string Pop();
    size_t Size() const;
    bool IsPlaying() const;

private:
    std::queue<std::string> mAudioQueue;
    std::set<std::string> mFilesAlreadyInQueue;
    bool isPlaying;

    friend class VoiceCallback;
};

class VoiceCallback : public IXAudio2VoiceCallback
{
public:
    VoiceCallback(AudioQueue &audioQueue);
    virtual void OnBufferEnd(void *pBufferContext) noexcept override;
    virtual void OnBufferStart(void *pBufferContext) noexcept override;
    virtual void OnLoopEnd(void *pBufferContext) noexcept override;
    virtual void OnStreamEnd() noexcept override;
    virtual void OnVoiceError(void *pBufferContext,
                              HRESULT Error) noexcept override;
    virtual void OnVoiceProcessingPassEnd() noexcept override;
    virtual void OnVoiceProcessingPassStart(
        UINT32 BytesRequired) noexcept override;

private:
    AudioQueue *mAudioQueue;
};

struct EngineCallback : public IXAudio2EngineCallback
{
    virtual void OnCriticalError(HRESULT Error) noexcept override;
    virtual void OnProcessingPassEnd() noexcept override;
    virtual void OnProcessingPassStart() noexcept override;
};

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

    Microsoft::WRL::ComPtr<IXAudio2> mXAudio;
    IXAudio2MasteringVoice *mMasterVoice;
    AudioQueue mAudioQueue;
    VoiceCallback mVoiceCallback;
    EngineCallback mEngineCallback;

    struct XAudioData
    {
        WAVEFORMATEXTENSIBLE wfx = {0};
        XAUDIO2_BUFFER buffer = {0};
        std::unique_ptr<unsigned char[]> rawBuffer;
    } mXAudioData;
};
}  // namespace NEngine::ECS::Systems