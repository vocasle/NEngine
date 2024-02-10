@echo off

if exist build\ (
	goto :clear
	goto :build
) else (
	goto :make_build_dir
)

:clear
rmdir /s/q build
if %ERRORLEVEL% neq 0 goto :build_failed

:make_build_dir
mkdir build
if %ERRORLEVEL% neq 0 goto :build_failed

:build
echo Building NEngine
set FLAGS=-Wall
set DEFINES=-DNENGINE_DLL_EXPORTS
set INCLUDES=-Isrc
set SOURCES=src/nengine.c
clang %FLAGS% %DEFINES% %INCLUDES% %SOURCES% --shared -o build/libnengine.dll
if %ERRORLEVEL% neq 0 goto :build_failed

:build_test
clang %FLAGS% -Lbuild -llibnengine src/main.c -o build/app.exe
if %ERRORLEVEL% neq 0 goto :build_failed

:done
echo Build completed 
exit /b 0

:build_failed
echo Build failed 
