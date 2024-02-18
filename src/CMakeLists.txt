
set(SOURCES
    "main.c"
    "core/vector.c"
    "core/vector.h"
    "core/memory.c"
    "core/memory.h"
    "logging/logging.c"
    "logging/logging.h"
    "test/test_framework.c"
    "test/test_framework.h"
)

add_executable(cabor ${SOURCES} "core/memory.c")
set_property(TARGET cabor PROPERTY C_STANDARD 99)
