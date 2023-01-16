#include "NEngine/Utils/Utils.h"

#include <debugapi.h>
#include <fcntl.h>
#include <io.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/stat.h>

#include <filesystem>
#include <memory>
#include <thread>

namespace NEngine {
namespace Utils {
class File
{
public:
    File(const std::string &path)
    {
        fopen_s(&m_out, path.c_str(), "w");
    }

    ~File()
    {
        if (m_out) {
            fclose(m_out);
        }
    }

    FILE *
    Get() const
    {
        return m_out;
    }

private:
    FILE *m_out;
};

namespace {
File gLog("log.txt");
};

void
UtilsDebugPrint(const char *fmt, ...)
{
    char out[512];
    va_list args;
    va_start(args, fmt);
    vsnprintf(out, sizeof(out), fmt, args);
    va_end(args);
    OutputDebugStringA(out);
    fprintf(stdout, "%s", out);
    fprintf(gLog.Get(), "%s", out);
}

void
UtilsFatalError(const char *fmt, ...)
{
    char out[512];
    va_list args;
    va_start(args, fmt);
    vsnprintf(out, sizeof(out), fmt, args);
    va_end(args);
    fprintf(stderr, "%s", out);
    fprintf(gLog.Get(), "%s", out);
    OutputDebugStringA(out);
    ExitProcess(EXIT_FAILURE);
}

std::string
UtilsFormatStr(const char *fmt, ...)
{
    std::string out;
    va_list args;
    va_start(args, fmt);
    va_list args_copy;
    va_copy(args_copy, args);
    size_t len = vsnprintf(nullptr, 0, fmt, args_copy);
    out.resize(len);
    vsnprintf(&out[0], len + 1, fmt, args);
    va_end(args_copy);
    va_end(args);
    return out;
}

int
UtilStrFindLastChar(const char *str, const char ch)
{
    int pos = -1;
    const char *begin = str;
    while (str && *str) {
        if (*str == ch)
            pos = (int)(str - begin);
        ++str;
    }
    return pos;
}

void
UtilsStrSub(
    const char *str, uint32_t start, uint32_t end, char out[], uint32_t maxSize)
{
    const uint32_t len = (uint32_t)strlen(str);
    assert(start < len && end < len);
    uint32_t max = len < maxSize ? len : maxSize - 1;
    max = max < end ? max : end;

    for (uint32_t i = 0; i < max; ++i) {
        out[i] = str[i];
    }
    out[max] = 0;
}

std::vector<uint8_t>
UtilsReadData(const char *filepath)
{
    FILE *f;
    fopen_s(&f, filepath, "rb");
    if (!f) {
        UtilsDebugPrint("ERROR: Failed to read data from %s", filepath);
        return {};
    }

    struct stat sb = {};
    if (stat(filepath, &sb) == -1) {
        UtilsDebugPrint("ERROR: Failed to get file stats from %s", filepath);
        return {};
    }
    std::vector<uint8_t> bytes(sb.st_size);
    fread(&bytes[0], sb.st_size, 1, f);
    fclose(f);
    return bytes;
}

void
UtilsCreateVertexBuffer(ID3D11Device *device,
                        const void *data,
                        size_t num,
                        size_t structSize,
                        ID3D11Buffer **ppBuffer)
{
    D3D11_SUBRESOURCE_DATA subresourceData = {};
    subresourceData.pSysMem = data;

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = structSize * num;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.StructureByteStride = structSize;
    bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;

    HR(device->CreateBuffer(&bufferDesc, &subresourceData, ppBuffer))
}

void
UtilsCreateIndexBuffer(ID3D11Device *device,
                       const void *data,
                       size_t num,
                       ID3D11Buffer **ppBuffer)
{
    D3D11_SUBRESOURCE_DATA subresourceData = {};
    subresourceData.pSysMem = data;

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = sizeof(uint32_t) * num;
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bufferDesc.StructureByteStride = 0;
    bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;

    HR(device->CreateBuffer(&bufferDesc, &subresourceData, ppBuffer));
}

void
UtilsWriteData(const char *filepath,
               const char *bytes,
               const size_t sz,
               const bool isBinary)
{
    FILE *out = nullptr;
    fopen_s(&out, filepath, isBinary ? "wb" : "w");

    if (out) {
        for (size_t i = 0; i < sz; ++i) {
            fputc(bytes[i], out);
        }

        fclose(out);
    }
}

void
UtilsUpdateConstantBuffer(ID3D11DeviceContext *context,
                          size_t bufferSize,
                          void *data,
                          ID3D11Buffer *dest)
{
    D3D11_MAPPED_SUBRESOURCE mapped = {};

    if (FAILED(context->Map(
            (ID3D11Resource *)dest, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) {
        UtilsFatalError("ERROR: Failed to map constant buffer\n");
    }
    memcpy(mapped.pData, data, bufferSize);
    context->Unmap((ID3D11Resource *)dest, 0);
}

void
UtilsCreateConstantBuffer(ID3D11Device *device,
                          size_t byteWidth,
                          ID3D11Buffer **pDest)
{
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.ByteWidth = byteWidth;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;

    if (FAILED(device->CreateBuffer(&bufferDesc, NULL, pDest))) {
        UtilsFatalError(
            "ERROR: Failed to create per frame constants cbuffer\n");
    }
}

std::wstring
UtilsStrToWstr(const std::string &str)
{
    size_t numConverted = 0;
    const size_t wstrSz = str.size() + 1;
    std::wstring wstr(wstrSz, 0);
    const errno_t res =
        mbstowcs_s(&numConverted, &wstr[0], wstrSz, &str[0], str.size());
    assert(res == 0 && "Failed to convert string to widestring");
    return wstr;
}

std::string
UtilsWstrToStr(const std::wstring &wstr)
{
    size_t numConverted = 0;
    const size_t strSz = wstr.size() + 1;
    std::string str(strSz, 0);
    const errno_t res =
        wcstombs_s(&numConverted, &str[0], strSz, &wstr[0], wstr.size());
    assert(res == 0 && "Failed to convert widestring to string");
    return str;
}

std::vector<std::wstring>
UtilsGlobFiles(const std::string &dir, const std::string &ext)
{
    std::error_code ec;
    std::filesystem::recursive_directory_iterator it(dir, ec);
    std::vector<std::wstring> files;

    for (auto entry : it) {
        const auto tmp = entry.path().extension();
        if (!entry.is_directory() && entry.path().extension() == ext) {
            files.push_back(entry.path().wstring());
        }
    }
    return files;
}

std::string
UtilsGetLastWin32Error()
{
    auto errCode = GetLastError();
    char *messageBuffer = nullptr;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                                     FORMAT_MESSAGE_FROM_SYSTEM |
                                     FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL,
                                 errCode,
                                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                 (LPSTR)&messageBuffer,
                                 0,
                                 NULL);

    // Copy the error message into a std::string.
    std::string message(messageBuffer, size);

    // Free the Win32's string's buffer.
    LocalFree(messageBuffer);

    return message;
}

std::string
UtilsGetThreadIdAsStr()
{
    std::ostringstream out;
    out << std::this_thread::get_id();
    return out.str();
}

}  // namespace Utils
}  // namespace NEngine
