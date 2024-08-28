/**
 * @file xf_task_port.c
 * @author cangyu (sky.kirto@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-03-05
 *
 * @copyright Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

/* ==================== [Includes] ========================================== */

#include "xf_task_port.h"

/* ==================== [Defines] =========================================== */

#define TAG "xf_port"

/* ==================== [Typedefs] ========================================== */
typedef struct _xf_task_tick_t {
    unsigned long int ticks; /* 时钟心跳，每秒钟跳动的次数 */
    xf_task_clock_t clock;
} xf_task_tick_t;

#if XF_TASK_CONTEXT_IS_ENABLE
typedef struct _xf_task_context_func_t {
    xf_task_create_context_t create;
    xf_task_swap_context_t swap;
} xf_task_context_func_t;

#endif // XF_TASK_CONTEXT_IS_ENABLE

typedef struct _xf_task_port_t {
    xf_task_tick_t tick;
#if XF_TASK_CONTEXT_IS_ENABLE
    xf_task_context_func_t context;
#endif // XF_TASK_CONTEXT_IS_ENABLE

} xf_task_port_t;


/* ==================== [Static Prototypes] ================================= */

static void xf_task_reg_init(void);

/* ==================== [Static Variables] ================================== */

static xf_task_port_t s_port = {0};

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */
xf_err_t xf_task_tick_init(unsigned long int clock_per_sec, xf_task_clock_t clock)
{
    XF_ASSERT(clock, XF_ERR_INVALID_ARG, TAG, "clock function must not be NULL");

    xf_bzero(&s_port.tick, sizeof(xf_task_tick_t));

    s_port.tick.ticks = clock_per_sec;
    s_port.tick.clock = clock;

    xf_task_reg_init();

    return XF_OK;

}

xf_task_time_t xf_task_get_ticks(void)
{
    XF_ASSERT(s_port.tick.clock, 0, TAG, "clock function must not be NULL");

    return s_port.tick.clock();
}

#if XF_TASK_CONTEXT_IS_ENABLE
xf_err_t xf_task_context_init(xf_task_create_context_t create_context, xf_task_swap_context_t swap_context)
{
    XF_ASSERT(create_context, XF_ERR_INVALID_ARG, TAG, "create_context function must not be NULL");
    XF_ASSERT(swap_context, XF_ERR_INVALID_ARG, TAG, "swap_context function must not be NULL");

    xf_bzero(&s_port.context, sizeof(xf_task_context_t));

    s_port.context.create = create_context;
    s_port.context.swap = swap_context;

    return XF_OK;
}

void xf_task_context_create(xf_task_manager_t manager, xf_context_func_t context_entry, void *context, void *stack,
                            size_t stack_size)
{
    XF_ASSERT(s_port.context.create, XF_RETURN_VOID, TAG, "create_context function must not be NULL");

    s_port.context.create(manager, context_entry, context, stack, stack_size);
}

void xf_task_context_swap(xf_task_manager_t manager, void *old_context, void *new_context)
{
    XF_ASSERT(s_port.context.swap, XF_RETURN_VOID, TAG, "create_swap function must not be NULL");

    s_port.context.swap(manager, old_context, new_context);
}

#endif // XF_TASK_CONTEXT_IS_ENABLE


int32_t xf_task_msec_to_ticks(int32_t msec)
{
    int64_t ret = (int64_t)((int64_t)msec * (int64_t)s_port.tick.ticks / 1000);
    if (ret > INT32_MAX || ret < INT32_MIN) {
        return INT32_MAX; // 使用int64_t保证返回类型匹配，但返回一个特殊值表示错误或溢出
    } else if (ret < INT32_MIN) {
        return INT32_MIN;
    } else {
        return ret; // 直接返回计算结果
    }
}

int32_t xf_task_ticks_to_msec(int32_t ticks)
{
    int64_t ret = (int64_t)(ticks * 1000 / (int64_t)s_port.tick.ticks);
    if (ret > INT32_MAX || ret < INT32_MIN) {
        return INT32_MAX; // 使用int64_t保证返回类型匹配，但返回一个特殊值表示错误或溢出
    } else if (ret < INT32_MIN) {
        return INT32_MIN;
    } else {
        return ret; // 直接返回计算结果
    }
}

int32_t xf_task_sec_to_ticks(int32_t sec)
{
    int64_t ret = (int64_t)((int64_t)sec * (int64_t)s_port.tick.ticks);
    if (ret > INT32_MAX || ret < INT32_MIN) {
        return INT32_MAX; // 使用int64_t保证返回类型匹配，但返回一个特殊值表示错误或溢出
    } else if (ret < INT32_MIN) {
        return INT32_MIN;
    } else {
        return ret; // 直接返回计算结果
    }
}

int32_t xf_task_ticks_to_sec(int32_t ticks)
{
    int64_t ret = (int64_t)(ticks / (int64_t)s_port.tick.ticks);
    if (ret > INT32_MAX || ret < INT32_MIN) {
        return INT32_MAX; // 使用int64_t保证返回类型匹配，但返回一个特殊值表示错误或溢出
    } else if (ret < INT32_MIN) {
        return INT32_MIN;
    } else {
        return ret; // 直接返回计算结果
    }
}


/* ==================== [Static Functions] ================================== */

#define XF_TASK_REG_EXTERN
#include "../task/xf_task_reg.inc"

static void xf_task_reg_init(void)
{
#define XF_TASK_REG_FUNCTION
#include "../task/xf_task_reg.inc"
}

