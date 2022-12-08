#include "NEngine/Helpers/ShaderManager.h"

#include <Windows.h>
#include <d3dcompiler.h>

#include <filesystem>

#include "NEngine/Utils/Utils.h"

namespace NEngine {
namespace Helpers {
using namespace Microsoft::WRL;
using namespace Utils;

void
ShaderManager::Initialize(ID3D11Device *device,
                          const std::string &shaderBlobRootDir,
                          const std::string &shaderSrcRootDir)
{
    UtilsDebugPrint("Shaders source root: %s\n", shaderSrcRootDir.c_str());
    UtilsDebugPrint("Shaders blob root: %s\n", shaderBlobRootDir.c_str());
    m_shaderSrcRootDir = shaderSrcRootDir;
    m_shaderBlobRootDir = shaderBlobRootDir;
    WIN32_FIND_DATA findData = {};
    const std::string shadersRoot =
        UtilsFormatStr("%s\\*", m_shaderBlobRootDir.c_str());
    HANDLE f = FindFirstFile(UtilsStrToWstr(shadersRoot).c_str(), &findData);
    do {
        const std::string shaderName = UtilsWstrToStr(findData.cFileName);
        if (shaderName.find(".cso") != std::string::npos) {
            UtilsDebugPrint("FOUND SHADER: %s\n", shaderName.c_str());
            if (shaderName.find("VS") != std::string::npos) {
                CreateVertexShader(shaderName, device);
            }
            else if (shaderName.find("PS") != std::string::npos) {
                CreatePixelShader(shaderName, device);
            }
        }
    }
    while (FindNextFile(f, &findData));
    FindClose(f);
}

size_t
ShaderManager::GetStrides() const
{
    return m_activeVS.empty() ? 0 : m_inputLayouts.at(m_activeVS).GetStrides();
}

ID3D11InputLayout *
ShaderManager::GetInputLayout() const
{
    return m_activeVS.empty() ? nullptr : m_inputLayouts.at(m_activeVS).Get();
}

ID3D11VertexShader *
ShaderManager::GetVertexShader(const std::string &name)
{
    const auto it = m_vertexShaders.find(name);
    if (it != std::end(m_vertexShaders)) {
        m_activeVS = it->first;
        return it->second.Get();
    }
    return nullptr;
}

ID3D11PixelShader *
ShaderManager::GetPixelShader(const std::string &name) const
{
    const auto it = m_pixelShaders.find(name);
    return it == std::end(m_pixelShaders) ? nullptr : it->second.Get();
}

void
ShaderManager::UpdateVertexShader(const std::string &name,
                                  ID3D11VertexShader *shader)
{
    m_vertexShaders[name] = shader;
}

void
ShaderManager::UpdatePixelShader(const std::string &name,
                                 ID3D11PixelShader *shader)
{
    m_pixelShaders[name] = shader;
}

void
ShaderManager::CreateVertexShader(const std::string &filepath,
                                  ID3D11Device *device)
{
    const auto bytes = UtilsReadData(
        UtilsFormatStr("%s/%s", m_shaderBlobRootDir.c_str(), filepath.c_str())
            .c_str());
    ComPtr<ID3D11VertexShader> vs;

    if (FAILED(device->CreateVertexShader(
            &bytes[0], bytes.size(), NULL, vs.GetAddressOf()))) {
        UtilsDebugPrint("ERROR: Failed to create vertex shader from %s",
                        filepath.c_str());
        return;
    }

    const std::string shaderName =
        filepath.substr(0, filepath.find_last_not_of("cso"));
    m_vertexShaders[shaderName] = vs;
    m_inputLayouts[shaderName] = InputLayout(device, &bytes[0], bytes.size());
}

void
ShaderManager::CreatePixelShader(const std::string &filepath,
                                 ID3D11Device *device)
{
    const auto bytes = UtilsReadData(
        UtilsFormatStr("%s/%s", m_shaderBlobRootDir.c_str(), filepath.c_str())
            .c_str());
    ComPtr<ID3D11PixelShader> ps;

    if (FAILED(device->CreatePixelShader(
            &bytes[0], bytes.size(), NULL, ps.GetAddressOf()))) {
        UtilsDebugPrint("ERROR: Failed to create pixel shader from %s",
                        filepath.c_str());
        return;
    }

    const std::string shaderName =
        filepath.substr(0, filepath.find_last_not_of("cso"));
    m_pixelShaders[shaderName] = ps;
}

void
ShaderManager::Recompile(ID3D11Device *device)
{
    UtilsDebugPrint("Recompiling shaders\n");

    WIN32_FIND_DATA findData = {};
    const std::string shadersRoot =
        UtilsFormatStr("%s/*", m_shaderSrcRootDir.c_str());
    HANDLE f = FindFirstFile(UtilsStrToWstr(shadersRoot).c_str(), &findData);
    do {
        using namespace std::filesystem;
        const path shaderName = findData.cFileName;
        if (shaderName.extension().string() != ".hlsl")
            continue;
        // compile shader
        const std::string shaderNameStr = UtilsFormatStr(
            "%s/%s", m_shaderSrcRootDir.c_str(), shaderName.string().c_str());
        const bool isVS = shaderNameStr.find("VS") != std::string::npos;
        ComPtr<ID3DBlob> shaderBlob = nullptr;
        ComPtr<ID3DBlob> errorBlob = nullptr;
        const std::wstring shaderPathW = UtilsStrToWstr(shaderNameStr);
        UtilsDebugPrint("Compiling from file: %s\n",
                        UtilsWstrToStr(shaderPathW).c_str());
        HR(D3DCompileFromFile(shaderPathW.c_str(),
                              nullptr,
                              D3D_COMPILE_STANDARD_FILE_INCLUDE,
                              "main",
                              isVS ? "vs_5_0" : "ps_5_0",
                              0,
                              0,
                              &shaderBlob,
                              &errorBlob))

        if (errorBlob.Get()) {
            UtilsDebugPrint(
                "ERROR: Failed to hot reload %s, because of compile error. "
                "%s\n",
                shaderName.c_str(),
                static_cast<char *>(errorBlob->GetBufferPointer()));
        }
        else if (shaderBlob.Get()) {
            const std::string sn = shaderName.stem().string();
            if (sn.find("VS") != std::string::npos) {
                if (GetVertexShader(sn)) {
                    ComPtr<ID3D11VertexShader> vs = {};
                    UtilsDebugPrint("Hot reload shader %s\n",
                                    shaderName.string().c_str());
                    const HRESULT hr = device->CreateVertexShader(
                        shaderBlob->GetBufferPointer(),
                        shaderBlob->GetBufferSize(),
                        nullptr,
                        vs.GetAddressOf());
                    UtilsDebugPrint("Hot reloading shader %s. Result: %ld\n",
                                    shaderName.string().c_str(),
                                    hr);
                    HR(hr)
                    UpdateVertexShader(sn, vs.Get());
                }
            }
            else if (sn.find("PS") != std::string::npos) {
                if (GetPixelShader(sn)) {
                    ComPtr<ID3D11PixelShader> ps = {};
                    UtilsDebugPrint("Hot reload shader %s\n",
                                    shaderName.string().c_str());
                    const HRESULT hr = device->CreatePixelShader(
                        shaderBlob->GetBufferPointer(),
                        shaderBlob->GetBufferSize(),
                        nullptr,
                        ps.GetAddressOf());
                    UtilsDebugPrint("Hot reloading shader %s. Result: %ld\n",
                                    shaderName.string().c_str(),
                                    hr);
                    HR(hr)
                    UpdatePixelShader(sn, ps.Get());
                }
            }
        }
    }
    while (FindNextFile(f, &findData));
    FindClose(f);
}

void
ShaderManager::RecompileShaders(Helpers::DeviceResources &deviceResources,
                                const std::vector<ShaderDefine> &defines)
{
    UtilsDebugPrint("Recompiling shaders\n");
    auto macros = std::vector<D3D_SHADER_MACRO>();
    for (const auto& define : defines) {
        macros.push_back({define.Name.c_str(), define.Value.c_str()});
    }
    macros.push_back({nullptr, nullptr});

    WIN32_FIND_DATA findData = {};
    const std::string shadersRoot =
        UtilsFormatStr("%s/*", NENGINE_SHADER_SOURCE_DIR);
    HANDLE f = FindFirstFile(UtilsStrToWstr(shadersRoot).c_str(), &findData);
    do {
        using namespace std::filesystem;
        const path shaderName = findData.cFileName;
        if (shaderName.extension().string() != ".hlsl")
            continue;
        // compile shader
        const std::string shaderNameStr = UtilsFormatStr(
            "%s/%s", NENGINE_SHADER_SOURCE_DIR, shaderName.string().c_str());
        const bool isVS = shaderNameStr.find("VS") != std::string::npos;
        ComPtr<ID3DBlob> shaderBlob = nullptr;
        ComPtr<ID3DBlob> errorBlob = nullptr;
        const std::wstring shaderPathW = UtilsStrToWstr(shaderNameStr);
        UtilsDebugPrint("Compiling from file: %s\n",
                        UtilsWstrToStr(shaderPathW).c_str());
        HR(D3DCompileFromFile(shaderPathW.c_str(),
                              macros.data(),
                              D3D_COMPILE_STANDARD_FILE_INCLUDE,
                              "main",
                              isVS ? "vs_5_0" : "ps_5_0",
                              D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION |
                                  D3DCOMPILE_OPTIMIZATION_LEVEL0,
                              0,
                              &shaderBlob,
                              &errorBlob))

        if (errorBlob.Get()) {
            UtilsDebugPrint(
                "ERROR: Failed to hot reload %s, because of compile error. "
                "%s\n",
                shaderName.c_str(),
                static_cast<char *>(errorBlob->GetBufferPointer()));
        }
        else if (shaderBlob.Get()) {
            const std::string sn = shaderName.stem().string();
            constexpr bool overrideSource = true;
            D3DWriteBlobToFile(
                shaderBlob.Get(),
                UtilsStrToWstr(UtilsFormatStr("%s/%s.cso",
                                              NENGINE_SHADER_BUILD_DIR,
                                              sn.c_str()))
                    .c_str(),
                overrideSource);
        }
    }
    while (FindNextFile(f, &findData));
    FindClose(f);
}

void
ShaderManager::RecompileShaders(Helpers::DeviceResources &deviceResources)
{
    RecompileShaders(deviceResources, {});
}

}  // namespace Helpers
}  // namespace NEngine
