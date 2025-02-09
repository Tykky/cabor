#!/bin/bash

FOLDERS=("cabor_test_programs" "ci" "scripts" "src")
FILES=("build.bat" "build.sh" "cabor.bat" "cabor.sh" "CMakeLists.txt" "Dockerfile" "LICENSE" "README.md" "external/CMakeLists.txt")

tar -czvf "cabor_source.tar.gz" "${FOLDERS[@]}" "${FILES[@]}"
