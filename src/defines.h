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

 #ifdef NENGINE_DLL_EXPORTS 
    /*Enabled as "export" while compiling the dll project*/
    #define DLLEXPORT __declspec(dllexport)  
 #else
    /*Enabled as "import" in the Client side for using already created dll file*/
    #define DLLEXPORT __declspec(dllimport)  
 #endif

#define NENGINE_VERSION_MAJOR 0
#define NENGINE_VERSION_MINOR 1
#define NENGINE_VERSION_PATCH 0
