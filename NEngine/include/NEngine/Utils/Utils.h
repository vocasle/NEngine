#pragma once

#include <assert.h>
#include <d3d11.h>
#include <stdint.h>
#include <stdio.h>

#include <string>
#include <vector>

namespace NEngine {
namespace Utils {
static const WORD MAX_CONSOLE_LINES = 500;

void UtilsDebugPrint(const char *fmt, ...);

void UtilsFatalError(const char *fmt, ...);

std::string UtilsFormatStr(const char *fmt, ...);

int UtilStrFindLastChar(const char *str, const char ch);

void UtilsStrSub(const char *str,
                 uint32_t start,
                 uint32_t end,
                 char out[],
                 uint32_t maxSize);

std::vector<uint8_t> UtilsReadData(const char *filepath);

void UtilsWriteData(const char *filepath,
                    const char *bytes,
                    const size_t sz,
                    const bool isBinary = false);

void UtilsCreateIndexBuffer(ID3D11Device *device,
                            const void *data,
                            size_t num,
                            ID3D11Buffer **ppBuffer);
void UtilsCreateVertexBuffer(ID3D11Device *device,
                             const void *data,
                             size_t num,
                             size_t structSize,
                             ID3D11Buffer **ppBuffer);
void UtilsUpdateConstantBuffer(ID3D11DeviceContext *context,
                               size_t bufferSize,
                               void *data,
                               ID3D11Buffer *dest);

void UtilsCreateConstantBuffer(ID3D11Device *device,
                               size_t byteWidth,
                               ID3D11Buffer **pDest);

std::wstring UtilsStrToWstr(const std::string &str);
std::string UtilsWstrToStr(const std::wstring &wstr);

std::vector<std::wstring> UtilsGlobFiles(const std::string &dir,
                                         const std::string &ext);

}  // namespace Utils
}  // namespace NEngine

#define UTILS_FATAL_ERROR(msg, ...)  \
    NEngine::Utils::UtilsFatalError( \
        "ERROR: %s:%d: %s",          \
        __FILE__,                    \
        __LINE__,                    \
        NEngine::Utils::UtilsFormatStr(msg, __VA_ARGS__).c_str())

#define COM_FREE(This) (This->Release())

#define HR(x)                                                   \
    if (FAILED(x)) {                                            \
        NEngine::Utils::UtilsDebugPrint(                        \
            "%s:%d - Operation failed.\n", __FILE__, __LINE__); \
    }

#if NENGINE_DEBUG
#define UTILS_ASSERT(x, msg) assert((x) && (msg))
#define UTILS_PRINT(msg, ...) \
    NEngine::Utils::UtilsDebugPrint(msg, __VA_ARGS__)
#define UTILS_PRINTLN(msg, ...) \
    NEngine::Utils::UtilsDebugPrint("%s\n", \
            NEngine::Utils::UtilsFormatStr(msg, __VA_ARGS__).c_str())
#else
#define UTILS_ASSERT(...)
#define UTILS_PRINT(...)
#define UTILS_PRINTLN(...)
#endif
