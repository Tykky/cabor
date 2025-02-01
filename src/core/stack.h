#pragma once

#include "vector.h"
#include <stdbool.h>

typedef struct
{
    cabor_vector* memory;
    size_t top_of_stack;
    size_t capacity;
} cabor_stack;

cabor_stack* cabor_create_stack(size_t stack_capacity);
void cabor_destroy_stack(cabor_stack* stack);
void cabor_stack_push(cabor_stack* stack, void* element);
bool cabor_stack_pop(cabor_stack* stack, void** element);
bool cabor_stack_peek(cabor_stack* stack, void** element);



