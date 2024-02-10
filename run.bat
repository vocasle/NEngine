@echo off
if exist build\app.exe (
	goto :run_exe
) else (
	goto :no_file
)

:run_exe
build\app.exe
exit /b 0

:no_file
echo ERROR: build/app.exe not found! Run compile.bat first.
