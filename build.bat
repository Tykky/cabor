@echo off

git=C:\"Program Files"\Git\bin\git.exe
cmake=C:\"Program Files"\CMake\bin\cmake.exe
set PATH=%PATH%;C:\"Program Files (x86)"\"Microsoft Visual Studio"\2019\BuildTools\VC\Tools\MSVC\14.29.30133\bin\Hostx64\x64

if not exist build mkdir build
cd build

cmake ..
ECHO -- Building cabor in Debug configuration
cmake --build . --config Debug
ECHO -- Building cabor in Release configuration
cmake --build . --config Release

cd ..
exit /B 0