#pragma once

#include "../language/parser.h"
#include "../core/vector.h"
#include "../cabor_defines.h"
#include <stdint.h>

typedef enum
{
    CABOR_TYPE_INT,
    CABOR_TYPE_BOOL,
    CABOR_TYPE_UNIT,
    CABOR_TYPE_FUNCTION,
    CABOR_TYPE_FUNCTION_UNKNOWN,
    CABOR_NUM_TYPES
} cabor_type;

cabor_type cabor_typecheck(cabor_ast_allocated_node* node)
{
}
