#include "power_interface.h"
#include "list.h"
#include "power_api.h"
#include "lbuf.h"
/* #include "ble_interface.h" */

#pragma str_literal_override("bt_poweroff_reboot.rodata")

struct power {
    struct list_head entry;
    void *priv;
    const struct power_operation *ops;
};

struct power_hdl {
    /* struct list_head head_on; */
    struct list_head head_off;
    const struct power_driver *driver;
    u8 pending;
    u8 lock;
    u8 is_poweroff;
    u8 config;
    u32 timeout;
    void (*callback)(u32);

    //periodic
    u32 ticks;
    struct list_head head;
};

static struct power_hdl hdl SEC(.non_volatile_ram);

#define __this (&hdl)

#define bt_ticks        sys_timer_schedule

static u8 nv_memory_pool[0x100] SEC(.non_volatile_ram);

void nv_memory_apply(void)
{
    lbuf_init(nv_memory_pool, sizeof(nv_memory_pool));
}

void nv_memory_release(void)
{
    lbuf_free(nv_memory_pool);
}

void *nv_memory_malloc(int size)
{
    void *p;

    p = lbuf_alloc((struct lbuff_head *)nv_memory_pool, size);

    if (p)  {
        my_memset(p, 0, size);
    } else {
        /* puts("nv memory null\n"); */
        while (1);
    }

    return p;
}

void nv_memory_free(void *p)
{
    lbuf_free(p);
}

static void __power_resume(u32 usec)
{
    struct power *p, *n;

    __this->pending = 0;
    list_for_each_entry_safe(p, n, &__this->head_off, entry) {
        if (p->ops) {
            p->ops->resume(p->priv, usec);
        }
    }
}

AT_NON_VOLATILE_RAM_CODE
static void __power_on(u32 usec)
{
    struct power *p, *n;

    __this->pending = 0;
    list_for_each_entry_safe(p, n, &__this->head_off, entry) {
        if (p->ops) {
            p->ops->on(p->priv);
        }
    }
}


//diable flash power  (code must run in RAM!!!)
//recovery flash power (code must run in RAM!!!)
AT_NON_VOLATILE_RAM_CODE
static void __power_off()
{
    u32 usec;
    u32 timeout = -1;
    struct power *p;

    OS_ENTER_CRITICAL();

    list_for_each_entry(p, &__this->head_off, entry) {
        if (p->ops && p->ops->get_timeout) {
            usec = p->ops->get_timeout(p->priv);
            /* printf("usec %x\n", usec); */
            /* put_u32hex(usec); */
        } else {
            usec = -1;
        }
        if (usec == 0) {
            OS_EXIT_CRITICAL();
            return;
        }
        if (timeout > usec) {
            timeout = usec;
        }
        /* putchar('!'); */
    }

    if (timeout == -1) {
        OS_EXIT_CRITICAL();
        /* puts("xxxxxxxx\n"); */
        return;
    }
    /* putchar('&'); */
    /* printf("timeout: %d - %d - %d\n", timeout, POWER_DOWN_TIMEOUT_MIN, __this->config); */

    if ((timeout > SLEEP_TIMEOUT_MIN_US)
        && __this->config & SLEEP_EN) {
        /* puts("power down\n"); */
        /* printf("power donw : %08d\n", timeout); */

        timeout = __this->driver->set_suspend_timer(timeout);
        /* put_u32hex(timeout); */

        if (timeout == 0) {
            OS_EXIT_CRITICAL();
            return;
        }
        __this->callback = __power_resume;
        list_for_each_entry(p, &__this->head_off, entry) {
            if (p->ops && p->ops->suspend_probe) {
                p->ops->suspend_probe(p->priv);
            }
        }
        list_for_each_entry(p, &__this->head_off, entry) {
            if (p->ops && p->ops->suspend_post) {
                p->ops->suspend_post(p->priv, timeout);
            }
        }
        __this->driver->suspend_enter();

        __asm__ volatile("idle");
        __builtin_pi32_nop();
        __builtin_pi32_nop();
        __builtin_pi32_nop();

        __this->driver->suspend_exit(timeout);
        if (__this->callback) {
            __this->callback(timeout);
        }

    } else if ((timeout > DEEP_SLEEP_TIMEOUT_MIN_US)
               && __this->config & DEEP_SLEEP_EN) {

        /* puts("power off\n");put_u32hex(timeout); */
        __this->timeout = timeout;
        /* timeout = (timeout < DEEP_SLEEP_TICKS_UNIT) ? timeout : DEEP_SLEEP_TICKS_UNIT; */
        timeout = __this->driver->set_off_timer(timeout);

        if (timeout == 0) {
            /* putchar('@'); */
            OS_EXIT_CRITICAL();
            return;
        }

        __this->callback = __power_on;

        list_for_each_entry(p, &__this->head_off, entry) {
            if (p->ops) {
                p->ops->off_probe(p->priv);
            }
        }
        list_for_each_entry(p, &__this->head_off, entry) {
            if (p->ops) {
                p->ops->off_post(p->priv, timeout);
            }
        }

        /* putchar('3'); */
        __this->driver->off_enter();
        while (1) {
            /* putchar('#'); */
        }
    }
    OS_EXIT_CRITICAL();
}
/*******************************************************************/
/*
 *-------------------API
 */

AT_NON_VOLATILE_RAM_CODE
void poweroff_recover()
{
    u8 reason;

    reason = 0;

    __this->driver->off_exit(__this->timeout);

    if (__this->callback) {
        __this->callback(__this->timeout);
    }
    /* bt_ticks(); */
}

AT_NON_VOLATILE_RAM_CODE
void power_on(void *priv)
{
    struct bt_power *power = (struct bt_power *)priv;

    /* puts("\nbt_power_on list_empty");put_u8hex(&__this->head_on); */
    __this->pending = 0;
    /* list_del(&power->entry); */
    /* list_add_tail(&power->entry, &__this->head_on); */
    /* power->wakeup = 1;  */
}


AT_NON_VOLATILE_RAM_CODE
void power_off(void *priv)
{
    struct bt_power *power = (struct bt_power *)priv;

    /* list_del(&power->entry); */
    /* list_add_tail(&power->entry, &__this->head_off); */

    if (list_empty(&__this->head_off)) {
        return;
    }

    if (__this->lock) {
        __this->pending = 1;
        /* putchar('+'); */
    } else {
        /* putchar('-'); */
        __power_off();
    }
    /* } */
}

void power_off_lock()
{
    __this->lock = 1;
}

void power_off_unlock()
{
    __this->lock = 0;
    if (__this->pending) {
        __this->pending = 0;
        /* putchar('*'); */
        __power_off();
    }
}

void *power_get(void *priv, const struct power_operation *ops)
{
    struct power *power;

    power = nv_memory_malloc(sizeof(*power));
    if (!power) {
        ASSERT(power != NULL);
        return NULL;
    }
    /* puts("********bt_power_get:"); */

    power->ops = ops;
    power->priv = priv;

    list_add_tail(&power->entry, &__this->head_off);

    return power;
}

void power_put(void *priv)
{
    struct power *power = (struct power *)priv;

    /* puts("*******bt_power_put:"); */

    list_del(&power->entry);

    nv_memory_free(power);
}


AT_VOLATILE_RAM_CODE
void power_init(const struct low_power_param *param)
{
    if (power_is_poweroff_post()) {
        return;
    }

    nv_memory_apply();

    const struct power_driver *driver = __power_ops;

    /* INIT_LIST_HEAD(&__this->head_on); */
    INIT_LIST_HEAD(&__this->head_off);

    __this->config = param->config;

    driver->ioctrl(POWER_OSC_INFO, param->osc_type, param->osc_hz);

    driver->ioctrl(POWER_DELAY_US, param->delay_us / 1000000L);

    driver->ioctrl(POWER_SET_BTOSC_DISABLE, param->btosc_disable);

    if (driver && driver->init) {
        driver->init();
    }

    driver->ioctrl(POWER_SET_RESET_MASK, 1); //reset mask sw
    __this->driver = driver;
}


u8 power_is_poweroff_probe(void)
{
    u32 temp;

    __this->is_poweroff = __power_is_poweroff();

    return __this->is_poweroff;
}

AT_NON_VOLATILE_RAM_CODE
void power_set_proweroff(void)
{
    __this->is_poweroff = 1;
}

AT_NON_VOLATILE_RAM_CODE
u8 power_is_poweroff_post(void)
{
    return __this->is_poweroff;
}


void power_set_soft_poweroff(void)
{
    __this->driver->ioctrl(POWER_SET_SOFTOFF);
}

void power_set_mode(u8 mode)
{
    /* maskrom_clear(); */
    __this->driver->ioctrl(POWER_SET_MODE, mode);
}

void power_set_idle(void)
{
    __this->driver->ioctrl(POWER_SET_IDLE);
}

void power_set_callback(u8 mode, void (*enter)(void), void (*exit)(u32))
{
    if (mode == DEEP_SLEEP_EN) {
        __this->driver->ioctrl(POWER_SET_DEEPSLEEP_CALLBACK, enter, exit);
    }
    if (mode == SLEEP_EN) {
        __this->driver->ioctrl(POWER_SET_SLEEP_CALLBACK, enter, exit);
    }
}
/*******************************************************************/


void sys_resume(void)
{
    return;
}


