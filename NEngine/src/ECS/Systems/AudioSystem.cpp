#include "NEngine/ECS/Systems/AudioSystem.h"

#include <chrono>
#include <mutex>

using namespace NEngine::Utils;
using namespace Microsoft::WRL;

std::mutex GAudioMtx;

namespace NEngine::ECS::Systems {
AudioSystem::AudioSystem(DefaultEntityManager &entityManager)
    : mEntityManager(&entityManager),
      mVoiceCallback(mAudioQueue)
{
    HR(XAudio2Create(
        mXAudio.ReleaseAndGetAddressOf(), 0, XAUDIO2_DEFAULT_PROCESSOR))
    HR(mXAudio->RegisterForCallbacks(&mEngineCallback))
    HR(mXAudio->CreateMasteringVoice(&mMasterVoice))

    mVoiceCallback.SetStreamEndCallback(
        [this](std::string path)
        {
            // TODO: Potentially unsafe operation.
            // std::lock_guard<std::mutex> lock(GAudioMtx);
            for (auto entity : mEntities) {
                auto &ac =
                    *mEntityManager->GetComponent<Components::AudioComponent>(
                        entity);
                if (ac.Path == path) {
                    ac.IsPlaying = false;
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
        }
    }

    PlayQueuedFiles();
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
    // mKeepAlive = false;
    // mAudioThread.join();
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
    std::lock_guard<std::mutex> lock(GAudioMtx);
    mAudioQueue.Push(path);
}

#ifdef _XBOX  // Big-Endian
#define fourccRIFF 'RIFF'
#define fourccDATA 'data'
#define fourccFMT 'fmt '
#define fourccWAVE 'WAVE'
#define fourccXWMA 'XWMA'
#define fourccDPDS 'dpds'
#endif

#ifndef _XBOX  // Little-Endian
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'
#endif
HRESULT
FindChunk(HANDLE hFile,
          DWORD fourcc,
          DWORD &dwChunkSize,
          DWORD &dwChunkDataPosition)
{
    HRESULT hr = S_OK;
    if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
        return HRESULT_FROM_WIN32(GetLastError());

    DWORD dwChunkType;
    DWORD dwChunkDataSize;
    DWORD dwRIFFDataSize = 0;
    DWORD dwFileType;
    DWORD bytesRead = 0;
    DWORD dwOffset = 0;

    while (hr == S_OK) {
        DWORD dwRead;
        if (0 == ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL))
            hr = HRESULT_FROM_WIN32(GetLastError());

        if (0 ==
            ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL))
            hr = HRESULT_FROM_WIN32(GetLastError());

        switch (dwChunkType) {
            case fourccRIFF:
                dwRIFFDataSize = dwChunkDataSize;
                dwChunkDataSize = 4;
                if (0 ==
                    ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL))
                    hr = HRESULT_FROM_WIN32(GetLastError());
                break;

            default:
                if (INVALID_SET_FILE_POINTER ==
                    SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT))
                    return HRESULT_FROM_WIN32(GetLastError());
        }

        dwOffset += sizeof(DWORD) * 2;

        if (dwChunkType == fourcc) {
            dwChunkSize = dwChunkDataSize;
            dwChunkDataPosition = dwOffset;
            return S_OK;
        }

        dwOffset += dwChunkDataSize;

        if (bytesRead >= dwRIFFDataSize)
            return S_FALSE;
    }

    return S_OK;
}

HRESULT
ReadChunkData(HANDLE hFile, void *buffer, DWORD buffersize, DWORD bufferoffset)
{
    HRESULT hr = S_OK;
    if (INVALID_SET_FILE_POINTER ==
        SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN))
        return HRESULT_FROM_WIN32(GetLastError());
    DWORD dwRead;
    if (0 == ReadFile(hFile, buffer, buffersize, &dwRead, NULL))
        hr = HRESULT_FROM_WIN32(GetLastError());
    return hr;
}

HRESULT
AudioSystem::OpenAudioFile(const std::string &path)
{
    // Open the file
    HANDLE hFile = CreateFile(Utils::UtilsStrToWstr(path).c_str(),
                              GENERIC_READ,
                              FILE_SHARE_READ,
                              NULL,
                              OPEN_EXISTING,
                              0,
                              NULL);

    if (INVALID_HANDLE_VALUE == hFile)
        return HRESULT_FROM_WIN32(GetLastError());

    if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
        return HRESULT_FROM_WIN32(GetLastError());

    // Locate RIFF chunk
    DWORD dwChunkSize;
    DWORD dwChunkPosition;
    // check the file type, should be fourccWAVE or 'XWMA'
    FindChunk(hFile, fourccRIFF, dwChunkSize, dwChunkPosition);
    DWORD filetype;
    ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition);
    if (filetype != fourccWAVE)
        return S_FALSE;

    // Locate fmt chunk
    FindChunk(hFile, fourccFMT, dwChunkSize, dwChunkPosition);
    ReadChunkData(hFile, &mXAudioData.wfx, dwChunkSize, dwChunkPosition);

    // Locate data chunk
    // fill out the audio data buffer with the contents of the fourccDATA chunk
    FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition);
    mXAudioData.rawBuffer = std::make_unique<unsigned char[]>(dwChunkSize);
    // BYTE *pDataBuffer = new BYTE[dwChunkSize];
    ReadChunkData(
        hFile, mXAudioData.rawBuffer.get(), dwChunkSize, dwChunkPosition);

    // Populate XAUDIO_BUFFER struct
    mXAudioData.buffer.AudioBytes =
        dwChunkSize;  // size of the audio buffer in bytes
    mXAudioData.buffer.pAudioData =
        mXAudioData.rawBuffer.get();  // buffer containing audio data
    mXAudioData.buffer.Flags =
        XAUDIO2_END_OF_STREAM;  // tell the source voice not to
                                // expect any data after this buffer

    return S_OK;
}

HRESULT
AudioSystem::PlayAudio()
{
    IXAudio2SourceVoice *pSourceVoice = nullptr;
    HRESULT hr = 0;
    if (FAILED(hr = mXAudio->CreateSourceVoice(&pSourceVoice,
                                               (WAVEFORMATEX *)&mXAudioData.wfx,
                                               0,
                                               XAUDIO2_MAX_FREQ_RATIO,
                                               &mVoiceCallback)))
        return hr;

    if (FAILED(hr = pSourceVoice->SubmitSourceBuffer(&mXAudioData.buffer)))
        return hr;

    if (FAILED(hr = pSourceVoice->Start(0)))
        return hr;

    return S_OK;
}

void
AudioSystem::PlayQueuedFiles()
{
    std::lock_guard<std::mutex> lock(GAudioMtx);
    while (mAudioQueue.Size() > 0 && !mAudioQueue.IsPlaying()) {
        HR(OpenAudioFile(mAudioQueue.Pop()))
        HR(PlayAudio())
        UTILS_PRINTLN("Playing audio...");
    }
}

VoiceCallback::VoiceCallback(AudioQueue &audioQueue)
    : mAudioQueue(&audioQueue)
{
}

void
VoiceCallback::OnBufferEnd(void *pBufferContext) noexcept
{
}

void
VoiceCallback::OnBufferStart(void *pBufferContext) noexcept
{
    std::lock_guard<std::mutex> lock(GAudioMtx);
    mAudioQueue->isPlaying = true;
}

void
VoiceCallback::OnLoopEnd(void *pBufferContext) noexcept
{
}

void
VoiceCallback::OnStreamEnd() noexcept
{
    std::lock_guard<std::mutex> lock(GAudioMtx);
    if (mAudioQueue->Size() > 0) {
        auto path = mAudioQueue->Pop();
        if (mOnStreamEndCallback) {
            mOnStreamEndCallback(path);
        }
    }
    mAudioQueue->isPlaying = false;
}

void
VoiceCallback::OnVoiceError(void *pBufferContext, HRESULT Error) noexcept
{
}

void
VoiceCallback::OnVoiceProcessingPassEnd() noexcept
{
}

void
VoiceCallback::OnVoiceProcessingPassStart(UINT32 BytesRequired) noexcept
{
}

void
VoiceCallback::SetStreamEndCallback(std::function<void(std::string)> callback)
{
    mOnStreamEndCallback = callback;
}

void
EngineCallback::OnCriticalError(HRESULT Error) noexcept
{
}

void
EngineCallback::OnProcessingPassEnd() noexcept
{
}

void
EngineCallback::OnProcessingPassStart() noexcept
{
}

void
AudioQueue::Push(std::string path)
{
    if (!mFilesAlreadyInQueue.contains(path)) {
        mAudioQueue.push(path);
        mFilesAlreadyInQueue.insert(path);
    }
}

std::string
AudioQueue::Pop()
{
    if (mAudioQueue.empty()) {
        return "";
    }
    auto path = mAudioQueue.front();
    mAudioQueue.pop();
    mFilesAlreadyInQueue.erase(path);
    return path;
}

size_t
AudioQueue::Size() const
{
    return mAudioQueue.size();
}

bool
AudioQueue::IsPlaying() const
{
    return isPlaying;
}

}  // namespace NEngine::ECS::Systems