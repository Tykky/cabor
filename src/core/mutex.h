#pragma once

// Hack with for loop to mimic RAII
// The intended usage is:
// CABOR_SCOPED_LOCK(lock) <== takes in cabor_mutex*
// {
//    ^
//    .
//    // Code inside critical section
//    .
//    V
// } <-- automatically unlocks here
//
// The braces are required here due to how the hack works with for loop.
#define CABOR_SCOPED_LOCK(mutex)\
    cabor_lock(mutex);\
    for (int _cabor_once = 1; _cabor_once; (cabor_unlock(mutex), _cabor_once = 0))

struct cabor_mutex;
typedef struct cabor_mutex cabor_mutex;

cabor_mutex* cabor_create_mutex();
void cabor_destroy_mutex(struct cabor_mutex* cabor_lock);
void cabor_lock(struct cabor_mutex* cabor_lock);
void cabor_unlock(struct cabor_mutex* cabor_lock);
int cabor_try_lock(struct cabor_mutex* cabor_lock);
