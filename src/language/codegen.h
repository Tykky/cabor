#pragma once

#include "ir.h"
#include "../core/hashmap.h"

#define CABOR_STACK_LOCATION_MAX_STR_SIZE 64

size_t cabor_get_stack_location_size();

typedef struct cabor_stack_location_t
{
    char* location[CABOR_STACK_LOCATION_MAX_STR_SIZE];
} cabor_stack_location;

typedef struct
{
    cabor_vector* locations; // maps ir_var (int) -> stack location
    size_t stack_used;
} cabor_locals;

cabor_locals* cabor_create_locals();
void cabor_destroy_locals(cabor_locals* localsk);

void cabor_init_locals(cabor_ir_data* ir_data, cabor_locals* cabor_locals);
