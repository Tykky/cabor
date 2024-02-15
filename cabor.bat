@echo off

if EXIST "build/bin/Release/cabor.exe" (
    start /w /b build/bin/Release/cabor.exe %*
) ELSE (
    echo Executable not found in build/bin/Release/cabor.exe
)
exit /B 0