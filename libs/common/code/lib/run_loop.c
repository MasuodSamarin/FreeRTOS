#include "run_loop.h"
#include "jiffies.h"


void __timer_register(struct list_head *head, struct timeout *time,
                      void (*fun)(struct timeout *time))
{
    struct timeout *p;

    list_for_each_entry(p, head, entry) {
        if (p == time) {
            ASSERT(0, "err: timeout entry exist\n");
        }
    }
    CPU_SR_ALLOC();

    OS_ENTER_CRITICAL();

    time->fun = fun;
    list_add_tail(&time->entry, head);

    OS_EXIT_CRITICAL();
}



void __timer_set(struct list_head *head, struct timeout *time)
{
    CPU_SR_ALLOC();

    OS_ENTER_CRITICAL();

    list_del(&time->entry);
    list_add_tail(&time->entry, head);

    OS_EXIT_CRITICAL();
}

void __timer_delete(struct list_head *entry)
{
    CPU_SR_ALLOC();

    OS_ENTER_CRITICAL();

    list_del(entry);

    OS_EXIT_CRITICAL();
}


void __timer_delete_region(struct list_head *head, void *begin, void *end)
{
    struct timeout *p, *n;
    CPU_SR_ALLOC();

    OS_ENTER_CRITICAL();

    list_for_each_entry_safe(p, n, head, entry) {
        if ((void *)p >= begin && (void *)p < end) {
            list_del(&p->entry);
        }
    }

    OS_EXIT_CRITICAL();
}

void __timer_delete_all(struct list_head *head)
{
    struct timeout *p, *n;
    CPU_SR_ALLOC();

    OS_ENTER_CRITICAL();

    list_for_each_entry_safe(p, n, head, entry) {
        list_del(&p->entry);
    }

    OS_EXIT_CRITICAL();
}

void __timer_schedule(struct list_head *head, unsigned long jiffies)
{
    struct timeout *p;

    list_for_each_entry(p, head, entry) {
        if (time_before(p->jiffies, jiffies)) {
            __timer_delete(&p->entry);
            p->fun(p);
            __timer_schedule(head, jiffies);
            return;
        }
    }
}


void __run_loop_register(struct list_head *head, struct run_loop *loop,
                         void(*run)(struct run_loop *loop), void *var)
{
    struct run_loop *p;

    list_for_each_entry(p, head, entry) {
        if (p == loop) {
            printf("err: loop entry exist: %x\n", p);
            while (1);
        }
    }
    loop->run = run;
    loop->var = var;
    list_add_tail(&loop->entry, head);
}


void __run_loop_schedule(struct list_head *head)
{
    struct run_loop *p, *n;

    list_for_each_entry_safe(p, n, head, entry) {
        p->run(p);
    }
}



