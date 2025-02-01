#include "stack.h"
#include "memory.h"
#include "../debug/cabor_debug.h"


cabor_stack* cabor_create_stack(size_t stack_capacity)
{
    CABOR_NEW(cabor_stack, stack);
    stack->memory = cabor_create_vector(stack_capacity, CABOR_PTR, true);
    stack->top_of_stack = 0;
    stack->capacity = stack_capacity;
    return stack;
}

void cabor_destroy_stack(cabor_stack* stack)
{
    cabor_destroy_vector(stack->memory);
    CABOR_DELETE(cabor_stack, stack);
}

void cabor_stack_push(cabor_stack* stack, void* element)
{
    if (stack->top_of_stack >= stack->capacity)
    {
        // A hack to grow the capacity of the vector,
        // pushing stack_capacity number of elements effectively
        // doubles the size
        for (size_t i = 0; i < stack->capacity; i++)
        {
            cabor_vector_push_ptr(stack, NULL);
        }
        stack->capacity *= 2;
    }

#ifdef CABOR_ENABLE_ALLOCATOR_FAT_POINTERS
    CABOR_ASSERT(stack->memory->vector_mem.size >= stack->capacity, "Failed to grow stack capacity!");
#endif

    void** stack_base = stack->memory->vector_mem.mem;
    stack_base[(stack->top_of_stack)++] = element;
}

bool cabor_stack_pop(cabor_stack* stack, void** element)
{
    void** stack_base = stack->memory->vector_mem.mem;
    if (stack->top_of_stack > 0)
    {
        *element = stack_base[--(stack->top_of_stack)];
        stack_base[stack->top_of_stack] = NULL;
        return true;
    }
    else
    {
        return false;
    }

}

bool cabor_stack_peek(cabor_stack* stack, void** element)
{
    if (stack->top_of_stack > 0)
    {
        void** stack_base = stack->memory->vector_mem.mem;
        *element = stack_base[stack->top_of_stack - 1];
    }
    else
    {
        return false;
    }
}
