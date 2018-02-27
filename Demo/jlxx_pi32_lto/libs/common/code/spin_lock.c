/* #include "spin_lock.h" */
#include "atomic.h"

struct lock {
    u8 is_lock;
};



void spin_lock_init(struct lock *lock)
{
}

bool spin_is_lock(void)
{
    /* if (lock->is_lock) */
    /* return TRUE; */
}

void spin_lock_on(void)
{
    /* atomic_set(lock->is_lock, 1); */
}

void spin_lock_off(void)
{
    /* atomic_set(lock->is_lock, 0); */
}
