#include "mutex.h"

#include "memory.h"
#include <uv.h>

typedef struct cabor_mutex
{
    uv_mutex_t uv_lock;
} cabor_mutex;


cabor_mutex* cabor_create_mutex_default_malloc()
{
    cabor_mutex* cabor_lock = malloc(sizeof(cabor_mutex));
    uv_mutex_init(&cabor_lock->uv_lock);
    return cabor_lock;
}

cabor_mutex* cabor_create_mutex()
{
    CABOR_NEW(cabor_mutex, cabor_lock);
    uv_mutex_init(&cabor_lock->uv_lock);
    return cabor_lock;
}

void cabor_destroy_mutex_default_malloc(struct cabor_mutex* cabor_lock)
{
    uv_mutex_destroy(&cabor_lock->uv_lock);
    free(cabor_lock);
}

void cabor_destroy_mutex(cabor_mutex* cabor_lock)
{
    uv_mutex_destroy(&cabor_lock->uv_lock);
    CABOR_DELETE(cabor_mutex, cabor_lock);
}

void cabor_lock(cabor_mutex* cabor_lock)
{
    uv_mutex_lock(cabor_lock);
}

void cabor_unlock(cabor_mutex* cabor_lock)
{
    uv_mutex_unlock(&cabor_lock->uv_lock);
}

int cabor_try_lock(cabor_mutex* cabor_lock)
{
    return uv_mutex_trylock(&cabor_lock->uv_lock);
}
