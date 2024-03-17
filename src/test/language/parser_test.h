#pragma once

#include "../../cabor_defines.h"

#ifdef CABOR_ENABLE_TESTING

#include "../test_framework.h"
#include "../../language/parser.h"

int cabor_test_parse_expression_abc();
int cabor_test_parse_expression_cba();

#endif // CABOR_ENABLE_TESTING

