#!/bin/sh

set -e

command -v cmake >/dev/null 2>&1 || { echo >&2 "cmake not found, please install cmake first"; exit 1; }

mkdir -p build/release
mkdir -p build/debug

echo "-- Building cabor in Debug configuration"
cd build/debug
cmake -DCMAKE_BUILD_TYPE="Debug" ../..
cmake --build .

cd ../..

echo "-- Building cabor in Release configuration"
cd build/release
cmake -DCMAKE_BUILD_TYPE="Release" ../..
cmake --build .

exit 0