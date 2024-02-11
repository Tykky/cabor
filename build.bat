@echo off

set PATH=%PATH%;C:\"Program Files"\Git\bin
set PATH=%PATH%;C:\"Program Files"\CMake\bin
set PATH=%PATH%;C:\"Program Files (x86)"\"Microsoft Visual Studio"\2019\BuildTools\VC\Tools\MSVC\14.29.30133\bin\Hostx64\x64

where /q cmake
if ERRORLEVEL 1 (
    ECHO "cmake.exe not found, please add cmake to PATH and make sure cmake is installed"
    exit /B 1
)

if not exist build mkdir build
cd build

cmake ..
ECHO -- Building cabor in Debug configuration
cmake --build . --config Debug
ECHO -- Building cabor in Release configuration
cmake --build . --config Release

cd ..
exit /B 0