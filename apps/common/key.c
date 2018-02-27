#include "os/rtos.h"
#include "key.h"
#include "key_drv_ad.h"
#include "key_drv_io.h"
#include "key_drv_matrix.h"
#include "init.h"
#include "sys_timer.h"

/* #define DEBUG_ENABLE */
#include "debug_log.h"


/*按键门槛值*/
#define KEY_BASE_CNT            4
#define KEY_LONG_CNT            75
#define KEY_HOLD_CNT            15
#define KEY_SHORT_CNT           7
#define KEY_DOUBLE_CLICK_CNT    35 //35*10ms

/*按键状态*/
#define KEY_SHORT_UP            0x0
#define KEY_LONG                0x1
#define KEY_HOLD                0x2
#define KEY_LONG_UP             0x3


struct key_handle {
    void *timer;
    u8(*handler)(u8, u8, u8);

    RTOS_MSGQ q;
    void *fifo[40];

    //system timer
    struct sys_timer timeout_key_scan;
    u8 enable;
};

static struct key_handle hdl;

#define __this  (&hdl)

static u8 key_type;        ///<按键类型


static void __keymsg_q_init(void)
{
    u32 err;

    err = rtos_q_create(&__this->q, __this->fifo, ARRAY_SIZE(__this->fifo));
    ASSERT(err == OS_NO_ERR, "key msg queue error!");

}

u32 __keymsg_q_pend_null(void)
{
    return NULL;
}
u32 __keymsg_q_pend(void)
{
    if (!__this->enable) {
        return KEY_NUMBER_INVALID;
    }
    u32 res;
    u32 msg;

    res = rtos_q_pend(&__this->q, 0, &msg);

    if (res != OS_NO_ERR) {
        printf("key msg Q pend err %x/%x\n", res, msg);
        return KEY_NUMBER_INVALID;
    }

    return msg;
}

static void __keymsg_q_post(u32 msg)
{
    u32 res;

    res = rtos_q_post(&__this->q, (void *)msg);

    if (res) {
        printf("key msg Q post err %x/%x\n", res, msg);
    }
}

void __keymsg_register_handler_null(u8(*handler)(u8, u8, u8)) {}
void __keymsg_register_handler(u8(*handler)(u8, u8, u8))
{
    __this->handler = handler;
}


/**
 * @brief key2msg_filter
 *
 * @param key_status
 * @param back_last_key
 */
static void key2msg_filter(u8 key_status, u8 back_last_key)
{
    /* os_taskq_post_msg(name, 3, key_type, key_status, back_last_key); */
    u8 msg = KEY_NUMBER_INVALID;

    if (__this->handler) {
        msg = __this->handler(key_type, key_status, back_last_key);
    }

    __keymsg_q_post(msg);
}

/**
 * @brief key_filter: output stable key number filter-out unstable input
 *
 * @param key: current input key number
 *
 * @return stable key number
 */
static u8 key_filter(u8 key)
{
    static tu8 used_key = KEY_NUMBER_INVALID;
    static tu8 old_key;
    static tu8 key_counter;

    if (old_key != key) {
        key_counter = 0;
        old_key = key;
    } else {
        key_counter++;
        if (key_counter == KEY_BASE_CNT) {
            used_key = key;
        }
    }

    return used_key;
}

/**
 * @brief get_key_value
 *
 * @return
 */
static u8 get_key_number(void)
{
    u8 key_number = KEY_NUMBER_INVALID;

#ifdef KEY_DRV_AD_EN
    key_number = __adc_key_drv->get_key_number();
    key_number = key_filter(key_number);
    /* printf("AD key number %d\n", key_number); */
    if (key_number != KEY_NUMBER_INVALID) {
        key_type = KEY_TYPE_AD;
    }
#endif

#ifdef KEY_DRV_IO_EN
    key_number = __io_key_drv->get_key_number();
    log_info("IO key number %d\n", key_number);
    key_number = key_filter(key_number);
    if (key_number != KEY_NUMBER_INVALID) {
        key_type = KEY_TYPE_IO;
    }
#endif

#ifdef KEY_DRV_MATRIX_EN
    key_number = __matrix_key_drv->get_key_number();
    log_info("MATRIX key number %d\n", key_number);
    key_number = key_filter(key_number);
    if (key_number != KEY_NUMBER_INVALID) {
        key_type = KEY_TYPE_MATRIX;
    }
#endif

    return key_number;
}


/**
 * @brief __key_scan
 */
static void __key_scan(void)
{
    static tu8 last_key = KEY_NUMBER_INVALID;
    static tu8 key_press_counter;
    static u8  key_press_flag = 0;
    tu8 cur_key, key_status, back_last_key;


    if (key_press_flag) {
        key_press_flag++;
        if (key_press_flag > KEY_DOUBLE_CLICK_CNT) {
            log_info(" One_Click ");
            key_press_flag = 0;
            key2msg_filter(KEY_SHORT_UP, 0);
        }
    }

    cur_key = KEY_NUMBER_INVALID;
    back_last_key = last_key;
    cur_key = get_key_number();

    /* log_info("get key number %x\n", cur_key); */
    if (cur_key == last_key) {                          //长时间按键
        if (cur_key == KEY_NUMBER_INVALID) {
            return;
        }

        key_press_counter++;

        if (key_press_counter == KEY_LONG_CNT) {        //长按
            key_status = KEY_LONG;
            log_info("[LONG]");
        } else if (key_press_counter == (KEY_LONG_CNT + KEY_HOLD_CNT)) {    //连按
            key_status = KEY_HOLD;
            key_press_counter = KEY_LONG_CNT;
            log_info("[HOLD]");
        } else {
            return;
        }
    } else { //cur_key = NO_KEY, 抬键
        last_key = cur_key;
        if ((key_press_counter < KEY_LONG_CNT) && (cur_key != KEY_NUMBER_INVALID)) {
        }
        if ((key_press_counter < KEY_LONG_CNT) && (cur_key == KEY_NUMBER_INVALID)) {    //短按抬起
            key_press_counter = 0;
            key_status = KEY_SHORT_UP;
            log_info("[SHORT UP]");
        } else if ((cur_key == KEY_NUMBER_INVALID) && (key_press_counter >= KEY_LONG_CNT)) { //长按抬起
            key_press_counter = 0;
            key_status = KEY_LONG_UP;
            log_info("[LONG UP]");
        } else {
            key_press_counter = 0;
            return;
        }
    }
    if (0) {
        if ((back_last_key == 0) && (key_status == KEY_SHORT_UP)) {
            if (key_press_flag == 0) {
                key_press_flag = 1;
            }
            if ((key_press_flag > 15) && (key_press_flag <= KEY_DOUBLE_CLICK_CNT)) {
                log_info(" Double_Click ");
                key_press_flag = 0;
                key2msg_filter(key_status, 9);
            }
        } else {
            key2msg_filter(key_status, back_last_key);
        }
    } else {
        key2msg_filter(key_status, back_last_key);
    }
}

static void app_key_scan(struct sys_timer *timer)
{
    __key_scan();
    sys_timer_register(&__this->timeout_key_scan, 10, app_key_scan);
}

/**
 * @brief key_init
 */
static void key_input_init(void)
{
    log_info("\nlate_initcall - key init\n");

    __this->enable = 1;
#ifdef KEY_DRV_AD_EN
    __adc_key_drv->init();
#endif

#ifdef KEY_DRV_IO_EN
    __io_key_drv->init();
#endif

    __keymsg_q_init();

    /* __this->timer = periodic_timer_add(10, __key_scan); */

    sys_timer_register(&__this->timeout_key_scan, 10, app_key_scan);
}
#if (defined KEY_DRV_AD_EN || defined KEY_DRV_IO_EN || defined KEY_DRV_MATRIX_EN)
late_initcall(key_input_init);
#endif


u16 get_cur_battery_value(void)
{
    u16 bat_v_value;
#ifdef KEY_DRV_AD_EN
    __adc_key_drv->ioctrl(GET_SYS_VOLTAGE, &bat_v_value);
    return bat_v_value;
#endif
    return 0;
}

