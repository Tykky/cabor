
set(SOURCES
    "main.c"
    "core/vector.c"
    "core/vector.h"
    "core/memory.c"
    "core/memory.h"
    "logging/logging.c"
    "logging/logging.h"
    "filesystem/filesystem.h"
    "filesystem/filesystem.c"
    "test/test_framework.c"
    "test/test_framework.h"
    "test/registered_tests.c"
    "test/registered_tests.h"
    "test/core/vector_test.c"
    "test/core/vector_test.h"
    "test/filesystem/filesystem_tests.h"
    "test/filesystem/filesystem_tests.c"
    "debug/cabor_debug.h"
    "cabor_defines.h"
)

if (MSVC)
    # We prevent dependency to vcruntime.dll by staticly linking it
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

add_executable(cabor ${SOURCES}  "test/core/vector_test.c")
set_property(TARGET cabor PROPERTY C_STANDARD 99)

