@echo off

setlocal enableextensions enabledelayedexpansion 

set CLANG=clang-cl.exe

echo Deleting build directory
rmdir /s/q build
echo Making build directory
mkdir build

echo Collecting NEngine source files
set NENGINE_SOURCE_DIR=%cd%\src
set NENGINE_SOURCE_LIST=
set NENGINE_HEADER_LIST=
for /f "tokens=*" %%f in ('dir /b "%NENGINE_SOURCE_DIR%\*.c"') do (
	set NENGINE_SOURCE_LIST=!NENGINE_SOURCE_LIST! %NENGINE_SOURCE_DIR%\%%f
)

for /f "tokens=*" %%f in ('dir /b "%NENGINE_SOURCE_DIR%\*.h"') do (
	set NENGINE_HEADER_LIST=!NENGINE_HEADER_LIST! %NENGINE_SOURCE_DIR%\%%f
)

REM remove extra space at the beginning
set NENGINE_SOURCE_LIST=%NENGINE_SOURCE_LIST:~1%
set NENGINE_HEADER_LIST=%NENGINE_HEADER_LIST:~1%

echo Collecting App source files
set APP_SOURCE_LIST=
set APP_SOURCE_DIR=%cd%\app
for /f "tokens=*" %%f in ('dir /b "%APP_SOURCE_DIR%\*.c"') do (
	set APP_SOURCE_LIST=!APP_SOURCE_LIST! %APP_SOURCE_DIR%\%%f
)
set APP_SOURCE_LIST=%APP_SOURCE_LIST:~1%

set DLL_NAME=nengine.dll
set EXE_NAME=app.exe
set NENGINE_COMPILER_DEFINITIONS=-DNE_EXPORT=1 -D_NDEBUG=0 -DNE_VERBOSE_ALLOCATIONS=0 -D_CRT_SECURE_NO_WARNINGS
set NENGINE_INCLUDE_DIRECTORIES=-I%NENGINE_SOURCE_DIR% -I%VULKAN_SDK%\Include
set NENGINE_COMPILER_FLAGS=/W4 /WX /std:c11 /TC
set NENGINE_LINKER_FLAGS=/link bufferoverflowu.lib user32.lib vulkan-1.lib /LIBPATH:%VULKAN_SDK%\Lib /DLL

set APP_COMPILER_DEFINITIONS=-DNE_EXPORT=0
set APP_COMPILER_FLAGS=/W4 /WX /std:c11 /TC
set APP_INCLUDE_DIRECTORIES=-I%cd%\app -I%cd%\src
set APP_LINKER_FLAGS=/link nengine.lib

pushd build
echo Building nengine.dll
set CMD=%CLANG% %NENGINE_COMPILER_FLAGS% %NENGINE_COMPILER_DEFINITIONS% %NENGINE_INCLUDE_DIRECTORIES%  /Fe%DLL_NAME% %NENGINE_SOURCE_LIST% %NENGINE_LINKER_FLAGS%
echo %CMD%
%CMD%

echo Building app.exe
set CMD=%CLANG% %APP_COMPILER_FLAGS% %APP_COMPILER_DEFINITIONS% %APP_INCLUDE_DIRECTORIES% %APP_SOURCE_LIST% /Fe%EXE_NAME% %APP_LINKER_FLAGS%
echo %CMD%
%CMD%
popd
