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

#define UTILS_FATAL_ERROR(msg, ...)                             \
	UtilsFatalError("ERROR: %s:%d: %s", __FILE__, __LINE__, \
			UtilsFormatStr(msg, __VA_ARGS__).c_str())

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

/* Dynamic Array */

#define DEFINE_ARRAY_TYPE(DataType, ClassSuffix)                          \
                                                                          \
	void Array##ClassSuffix##Resize(struct Array##ClassSuffix *arr,   \
					size_t capacity)                  \
	{                                                                 \
		DataType *Data =                                          \
			realloc(arr->Data, capacity * sizeof(DataType));  \
		assert(Data);                                             \
		arr->Data = Data;                                         \
		arr->Capacity = capacity;                                 \
	}                                                                 \
                                                                          \
	void Array##ClassSuffix##PushBack(struct Array##ClassSuffix *arr, \
					  DataType v)                     \
	{                                                                 \
		if (!arr->Data || arr->Count + 1 > arr->Capacity) {       \
			size_t size = arr->Data ? arr->Capacity * 2 : 4;  \
			Array##ClassSuffix##Resize(arr, size);            \
		}                                                         \
		arr->Data[arr->Count++] = v;                              \
	}                                                                 \
                                                                          \
	void Array##ClassSuffix##Free(struct Array##ClassSuffix *arr)     \
	{                                                                 \
		arr->Count = 0;                                           \
		free(arr->Data);                                          \
	}                                                                 \
                                                                          \
	void Array##ClassSuffix##FreeCustom(                              \
		struct Array##ClassSuffix *arr,                           \
		void (*CustomFree)(struct Array##ClassSuffix * arr))      \
	{                                                                 \
		for (uint32_t i = 0; i < arr->Count; ++i)                 \
			CustomFree(&arr->Data[i]);                        \
		arr->Count = 0;                                           \
		free(arr->Data);                                          \
	}

#define DECLARE_ARRAY_TYPE(DataType, ClassSuffix)                         \
	struct Array##ClassSuffix {                                       \
		size_t Count;                                             \
		size_t Capacity;                                          \
		DataType *Data;                                           \
	};                                                                \
                                                                          \
	void Array##ClassSuffix##Resize(struct Array##ClassSuffix *arr,   \
					size_t capacity);                 \
                                                                          \
	void Array##ClassSuffix##PushBack(struct Array##ClassSuffix *arr, \
					  DataType v);                    \
                                                                          \
	void Array##ClassSuffix##Free(struct Array##ClassSuffix *arr);    \
                                                                          \
	void Array##ClassSuffix##FreeCustom(                              \
		struct Array##ClassSuffix *arr,                           \
		void (*CustomFree)(struct Array##ClassSuffix * arr))

#define COM_FREE(This) (This->Release())

#define HR(x)                                                            \
	if (FAILED(x)) {                                                 \
		UtilsDebugPrint("%s:%d - Operation failed.\n", __FILE__, \
				__LINE__);                               \
	}

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

}
}
