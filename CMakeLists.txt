cmake_minimum_required(VERSION 3.22)
project(cabor LANGUAGES C)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(external)
add_subdirectory(src)

message(STATUS "cabor external libraries: ${CABOR_EXTERNAL_LIBRARIES}")
message(STATUS "cabor external include directories: ${CABOR_EXTERNAL_INCLUDE_DIRECTORIES}")

target_link_libraries(cabor PRIVATE ${CABOR_EXTERNAL_LIBRARIES})
target_include_directories(cabor PRIVATE ${CABOR_EXTERNAL_INCLUDE_DIRECTORIES})

if (MSVC)
	file(COPY ${CMAKE_SOURCE_DIR}/cabor_test_programs DESTINATION ${CMAKE_BINARY_DIR}/bin/Debug)
	file(COPY ${CMAKE_SOURCE_DIR}/cabor_test_programs DESTINATION ${CMAKE_BINARY_DIR}/bin/Release)
	file(COPY ${CMAKE_SOURCE_DIR}/cabor_test_programs DESTINATION ${CMAKE_BINARY_DIR}/bin)
else()
	file(COPY ${CMAKE_SOURCE_DIR}/cabor_test_programs DESTINATION ${CMAKE_BINARY_DIR}/bin)
endif()
