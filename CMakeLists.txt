cmake_minimum_required(VERSION 3.22)
project(cabor LANGUAGES C)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src)

if (MSVC)
	file(COPY ${CMAKE_SOURCE_DIR}/cabor_test_programs DESTINATION ${CMAKE_BINARY_DIR}/bin/Debug)
	file(COPY ${CMAKE_SOURCE_DIR}/cabor_test_programs DESTINATION ${CMAKE_BINARY_DIR}/bin/Release)
	file(COPY ${CMAKE_SOURCE_DIR}/cabor_test_programs DESTINATION ${CMAKE_BINARY_DIR}/bin)
else()
	file(COPY ${CMAKE_SOURCE_DIR}/cabor_test_programs DESTINATION ${CMAKE_BINARY_DIR}/bin)
endif()
