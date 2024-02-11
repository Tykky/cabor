@echo off

if not exist build mkdir build
cd build

cmake ..
cmake --build . --config Debug
cmake --build . --config Release

cd ..
exit /B 0