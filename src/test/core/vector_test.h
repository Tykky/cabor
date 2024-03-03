#pragma once

#include "../../cabor_defines.h"

#ifdef CABOR_ENABLE_TESTING

#include "../../core/vector.h"

int cabor_test_vector_push();
int cabor_test_vector_get();
int cabor_test_vector_peek();
int cabor_test_vector_resize();

#endif // CABOR_ENABLE_TESTING
