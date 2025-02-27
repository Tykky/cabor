set(SOURCES
    "main.c"
    "core/vector.c"
    "core/vector.h"
    "core/memory.c"
    "core/memory.h"
    "core/mutex.h"
    "core/mutex.c"
    "core/stack.h"
    "core/stack.c"
    "core/cabortime.h"
    "core/cabortime.c"
    "logging/logging.c"
    "logging/logging.h"
    "filesystem/filesystem.h"
    "filesystem/filesystem.c"
    "language/tokenizer.h"
    "language/tokenizer.c"
    "language/parser.h"
    "language/parser.c"
    "test/test_framework.c"
    "test/test_framework.h"
    "test/registered_tests.c"
    "test/registered_tests.h"
    "test/core/vector_test.c"
    "test/core/vector_test.h"
    "test/core/stack_test.h"
    "test/core/stack_test.c"
    "test/filesystem/filesystem_tests.h"
    "test/filesystem/filesystem_tests.c"
    "test/language/tokenizer_test.c"
    "test/language/tokenizer_test.h"
    "test/language/parser_test.c"
    "test/language/parser_test.h"
    "debug/cabor_debug.h"
    "cabor_defines.h"
	"network/network.h"
    "network/network.c"
)

if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /0x")
endif()

set(CMAKE_C_STANDARD 99)
add_executable(cabor ${SOURCES})
set_property(TARGET cabor PROPERTY C_STANDARD 99)

