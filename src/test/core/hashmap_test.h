#pragma once

#include "../../cabor_defines.h"

#ifdef CABOR_ENABLE_TESTING

#include "../test_framework.h"
#include "../../core/hashmap.h"

int cabor_unit_test_hashmap_insert_and_get();
int cabor_unit_test_hashmap_collison_test();
int cabor_unit_test_hashmap_tiny_collisions();

#endif
