#pragma once

#include "../../cabor_defines.h"

#ifdef CABOR_ENABLE_TESTING

#include "../test_framework.h"
#include "../../language/codegen.h"
#include "../../language/compiler.h"


int cabor_integration_test_codegen_basic();
int cabor_integration_test_codegen_print_int();

int cabor_compiler_test1();

#endif

