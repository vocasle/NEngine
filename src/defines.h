#pragma once

typedef char i8;
typedef unsigned char u8;
typedef short i16;
typedef unsigned short u16;
typedef int i32;
typedef unsigned int u32;
typedef long long i64;
typedef unsigned long long u64;
typedef float f32;
typedef double f64;
typedef _Bool b8;

#define NE_TRUE 1
#define NE_FALSE 0

#define NULL 0

#define NE_ARI_VULKAN 1 // for now we only support Vulkan

 #if NE_EXPORT
    /*Enabled as "export" while compiling the dll project*/
    #define NE_API __declspec(dllexport)  
 #else
    /*Enabled as "import" in the Client side for using already created dll file*/
    #define NE_API __declspec(dllimport)  
 #endif

#define NE_VERSION_MAJOR 0
#define NE_VERSION_MINOR 1
#define NE_VERSION_PATCH 0

#if _WIN32
#define NE_PLATFORM_WIN32 1
#endif

#if _NDEBUG
#define NE_RELEASE 1
#else
#define NE_DEBUG 1
#endif
