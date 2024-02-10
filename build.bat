@echo off

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