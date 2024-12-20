/**
 * @file xf_ttask.c
 * @author cangyu (sky.kirto@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-02-28
 *
 * @copyright Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

/* ==================== [Includes] ========================================== */

#include "xf_ttask.h"

#include "../kernel/xf_task_base.h"
#include "../port/xf_task_port_internal.h"

/* ==================== [Defines] =========================================== */

#define TAG "ttask"

/* ==================== [Typedefs] ========================================== */

typedef struct _xf_ttask_handle_t {
    xf_task_base_t base;    /*!< 继承父对象 */
    uint32_t count;         /*!< 记录 ttask 剩余循环次数 */
    uint32_t count_max;     /*!< 记录 ttask 循环次数上限 */
} xf_ttask_handle_t;

/* ==================== [Static Prototypes] ================================= */

static void xf_ttask_reset(xf_task_t task);
static xf_task_time_t xf_ttask_update(xf_task_t task);
static void xf_ttask_exec(xf_task_manager_t manager);
static xf_task_t xf_ttask_constructor(xf_task_manager_t manager, xf_task_func_t func, void *func_arg, uint16_t priority,
                                      void *config);

/* ==================== [Static Variables] ================================== */

static const xf_task_vfunc_t _ttask_vfunc = {
    .constructor = xf_ttask_constructor,
    .exec = xf_ttask_exec,
    .reset = xf_ttask_reset,
    .update = xf_ttask_update,
};

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

void xf_ttask_vfunc_register(void)
{
    xf_task_vfunc_register(XF_TASK_TYPE_TTASK, &_ttask_vfunc);
}

xf_err_t xf_ttask_set_count(xf_task_t task, uint32_t count)
{
    XF_ASSERT(task, XF_ERR_INVALID_ARG, TAG, "task must not be NULL");

    xf_ttask_handle_t *handle = (xf_ttask_handle_t *)task;

    if (handle->base.type != XF_TASK_TYPE_TTASK) {
        XF_LOGE(TAG, "task must be ttask");
        return XF_ERR_INVALID_ARG;
    }

    if (count > handle->count_max) {
        XF_LOGE(TAG, "task must less than %d", (int)handle->count_max);
        return XF_ERR_INVALID_ARG;
    }

    handle->count = count;

    return XF_OK;
}

uint32_t xf_ttask_get_count(xf_task_t task)
{
    XF_ASSERT(task, 0, TAG, "task must not be NULL");

    xf_ttask_handle_t *handle = (xf_ttask_handle_t *)task;

    return handle->count;
}

xf_err_t xf_ttask_set_count_max(xf_task_t task, uint32_t count_max)
{
    XF_ASSERT(task, XF_ERR_INVALID_ARG, TAG, "task must not be NULL");

    xf_ttask_handle_t *handle = (xf_ttask_handle_t *)task;

    handle->count_max = count_max;

    return XF_OK;
}

/* ==================== [Static Functions] ================================== */

static xf_task_t xf_ttask_constructor(xf_task_manager_t manager, xf_task_func_t func, void *func_arg, uint16_t priority,
                                      void *config)
{
    xf_ttask_handle_t *task = (xf_ttask_handle_t *)xf_malloc(sizeof(xf_ttask_handle_t));

    if (task == NULL) {
        XF_LOGE(TAG, "memory alloc failed!");
        return NULL;
    }


    xf_ttask_config_t *ttask_config = config;

    uint32_t ticks = xf_task_msec_to_ticks(ttask_config->delay_ms);

    xf_task_base_init(&task->base, manager, XF_TASK_TYPE_TTASK, priority, func, func_arg);

    task->base.delay = ticks;
    task->count = ttask_config->count;
    task->count_max = ttask_config->count;

    task->base.weakup = xf_task_get_ticks() + ticks;

    return (xf_task_t)task;
}

static void xf_ttask_reset(xf_task_t task)
{
    xf_ttask_handle_t *handle = (xf_ttask_handle_t *)task;

    xf_task_base_reset(&handle->base);

    handle->count = handle->count_max;  // 重置计数器
    handle->base.weakup = xf_task_get_ticks() + handle->base.delay; // 重置唤醒时间
}

static void xf_ttask_time_handle(xf_task_t task, uint32_t time_ticks)
{
    xf_ttask_handle_t *handle = (xf_ttask_handle_t *)task;

    int32_t timeout = time_ticks - handle->base.weakup;

    // 计数器到0，停止更新，进入删除状态
    if (handle->count == 0) {
        xf_task_delete(task);
        return;
    }

    // 转换超时时间，如果大于零则触发超时
    handle->base.timeout = xf_task_ticks_to_msec(timeout);
    if (timeout >= 0) {
        BITS_SET1(handle->base.signal, XF_TASK_SIGNAL_TIMEOUT);
        // 根据循环次数重置循环结束点
        handle->count = (handle->count != XF_TTASK_INFINITE_LOOP) ? (handle->count - 1) : (handle->count);
    }

}

static xf_task_time_t xf_ttask_update(xf_task_t task)
{
    xf_ttask_handle_t *handle = (xf_ttask_handle_t *)task;
    xf_task_time_t time_ticks = xf_task_get_ticks();

    if (handle->base.delay != 0) {
        xf_ttask_time_handle(task, time_ticks);
    }

    if (BITS_CHECK(handle->base.signal, XF_TASK_SIGNAL_TIMEOUT)) {
        BITS_SET0(handle->base.signal, XF_TASK_SIGNAL_TIMEOUT);
        BITS_SET1(handle->base.signal, XF_TASK_SIGNAL_READY);
    }

    if (BITS_CHECK(handle->base.signal, XF_TASK_SIGNAL_EVENT)) {
        BITS_SET0(handle->base.signal, XF_TASK_SIGNAL_EVENT);
        BITS_SET1(handle->base.signal, XF_TASK_SIGNAL_READY);
    }

    return time_ticks;
}

static void xf_ttask_exec(xf_task_manager_t manager)
{
    xf_ttask_handle_t *task = (xf_ttask_handle_t *)xf_task_manager_get_current_task(manager);

    xf_task_base_set_state(task, XF_TASK_STATE_RUNNING);
    task->base.func(task);

    if (task->base.delay != 0) {
        task->base.weakup = xf_task_get_ticks() + task->base.delay;
    }
}

