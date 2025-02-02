#include "../../core/stack.h" 
#include "stack_test.h"
#include "../test_framework.h"

int cabor_test_stack_push()
{
    cabor_stack* stack = cabor_create_stack(10);
    const char values[] = { 'A', 'B', 'C' };

    int res = 0;

    for (size_t i = 0; i < 3; i++)
    {
        CABOR_CHECK_EQUALS(stack->top_of_stack, i, res);
        cabor_stack_push(stack, &values[i]);
    }

    char** stack_base = stack->memory->vector_mem.mem;
    CABOR_CHECK_EQUALS(*stack_base[0], 'A', res);
    CABOR_CHECK_EQUALS(*stack_base[1], 'B', res);
    CABOR_CHECK_EQUALS(*stack_base[2], 'C', res);

    CABOR_CHECK_EQUALS(stack->top_of_stack, 3, res);

    CABOR_CHECK_EQUALS(stack->capacity, 10, res);

    cabor_destroy_stack(stack);

    return 0;
}

int cabor_test_stack_pop()
{
    cabor_stack* stack = cabor_create_stack(10);
    const char values[] = { 'A', 'B', 'C' };

    for (size_t i = 0; i < 3; i++)
        cabor_stack_push(stack, &values[i]);

    int res = 0;

    char** stack_base = stack->memory->vector_mem.mem;
    CABOR_CHECK_EQUALS(*stack_base[0], 'A', res);
    CABOR_CHECK_EQUALS(*stack_base[1], 'B', res);
    CABOR_CHECK_EQUALS(*stack_base[2], 'C', res);

    char* a;
    char* b;
    char* c;

    CABOR_CHECK_EQUALS(stack->top_of_stack, 3, res);
    cabor_stack_pop(stack, &c);
    CABOR_CHECK_EQUALS(stack->top_of_stack, 2, res);
    cabor_stack_pop(stack, &b);
    CABOR_CHECK_EQUALS(stack->top_of_stack, 1, res);
    cabor_stack_pop(stack, &a);
    CABOR_CHECK_EQUALS(stack->top_of_stack, 0, res);

    CABOR_CHECK_EQUALS(*a, 'A', res);
    CABOR_CHECK_EQUALS(*b, 'B', res);
    CABOR_CHECK_EQUALS(*c, 'C', res);

    CABOR_CHECK_EQUALS(stack->capacity, 10, res);

    cabor_destroy_stack(stack);

    return 0;
}

int cabor_test_stack_peek()
{
    cabor_stack* stack = cabor_create_stack(10);
    const char* values[] = { 'A', 'B', 'C' };

    for (size_t i = 0; i < 3; i++)
        cabor_stack_push(stack, &values[i]);

    int res = 0;
    char* peek;
    cabor_stack_peek(stack, &peek);

    CABOR_CHECK_EQUALS(*peek, 'C', res);

    CABOR_CHECK_EQUALS(stack->capacity, 10, res);

    cabor_destroy_stack(stack);


    return 0;
}
