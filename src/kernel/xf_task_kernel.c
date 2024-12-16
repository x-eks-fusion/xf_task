/**
 * @file xf_task_kernel.c
 * @author cangyu (sky.kirto@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-07-05
 *
 * @copyright Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

/* ==================== [Includes] ========================================== */

#include "xf_task.h"
#include "xf_task_base.h"
#include "xf_task_manager.h"
#include "../port/xf_task_port_internal.h"

/* ==================== [Defines] =========================================== */

#define TAG "xf_task"

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

/* ==================== [Static Variables] ================================== */

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

xf_task_t xf_task_create_with_manager(xf_task_manager_t manager, xf_task_type_t type, xf_task_func_t func,
                                      void *func_arg, uint16_t priority, void *config)
{
    XF_ASSERT(manager, NULL, TAG, "manager must not be NULL");
    XF_ASSERT(func, NULL, TAG, "func must not be NULL");
    XF_ASSERT(priority < XF_TASK_PRIORITY_LEVELS, NULL, TAG, "priority must less than %d", XF_TASK_PRIORITY_LEVELS);
    XF_ASSERT(config, NULL, TAG, "args must not be NULL");

    const xf_task_vfunc_t *vfunc = xf_task_get_vfunc(type);
    xf_task_t task = vfunc->constructor(manager, func, func_arg, priority, config);

    return task;
}

void xf_task_delete(xf_task_t task)
{
    XF_ASSERT(task, XF_RETURN_VOID, TAG, "task must not be NULL");

    xf_task_base_t *handle = (xf_task_base_t *)task;
    xf_task_manager_t manager = handle->manager;

    xf_task_manager_task_destory(manager, task);
}

void xf_task_reset(xf_task_t task)
{
    XF_ASSERT(task, XF_RETURN_VOID, TAG, "task must not be NULL");

    xf_task_base_t *handle = (xf_task_base_t *)task;

    handle->vfunc->reset(task);
}

void *xf_task_get_arg(xf_task_t task)
{
    XF_ASSERT(task, NULL, TAG, "task must not be NULL");

    xf_task_base_t *task_base = (xf_task_base_t *)task;

    return task_base->arg;
}

int32_t xf_task_get_timeout(xf_task_t task)
{
    XF_ASSERT(task, 0, TAG, "task must not be NULL");

    xf_task_base_t *task_base = (xf_task_base_t *)task;

    return task_base->timeout;
}

xf_task_manager_t xf_task_get_manager(xf_task_t task)
{
    XF_ASSERT(task, NULL, TAG, "task must not be NULL");

    xf_task_base_t *task_base = (xf_task_base_t *)task;

    return task_base->manager;
}

#if XF_TASK_HUNGER_IS_ENABLE

void xf_task_feel_hungery_enable(xf_task_t task, uint32_t hunger_time)
{
    XF_ASSERT(task, XF_RETURN_VOID, TAG, "task must not be NULL");

    xf_task_base_t *task_base = (xf_task_base_t *)task;

    BITS_SET1(task_base->flag, XF_TASK_FALG_FEEL_HUNGERY);
    task_base->hunger_time = hunger_time;
}

void xf_task_feel_hungery_disable(xf_task_t task)
{
    XF_ASSERT(task, XF_RETURN_VOID, TAG, "task must not be NULL");
    xf_task_base_t *task_base = (xf_task_base_t *)task;
    task_base->hunger_time = 0;
    BITS_SET0(task_base->flag, XF_TASK_FALG_FEEL_HUNGERY);
}

#endif // XF_TASK_HUNGER_IS_ENABLE

xf_err_t xf_task_suspend(xf_task_t task)
{
    XF_ASSERT(task, XF_ERR_INVALID_ARG, TAG, "task must not be NULL");
    xf_task_base_t *task_base = (xf_task_base_t *)task;
    xf_task_manager_t manager = task_base->manager;
    if (task_base->state == XF_TASK_STATE_DELETE) {
        return XF_ERR_INVALID_STATE;
    }

    xf_task_manager_task_suspend(manager, task);
    task_base->suspend_time = xf_task_get_ticks();

    return XF_OK;
}

xf_err_t xf_task_resume(xf_task_t task)
{
    XF_ASSERT(task, XF_ERR_INVALID_ARG, TAG, "task must not be NULL");

    xf_task_base_t *task_base = (xf_task_base_t *)task;
    xf_task_manager_t manager = task_base->manager;
    if (task_base->state != XF_TASK_STATE_SUSPEND) {
        return XF_ERR_INVALID_STATE;
    }

    BITS_SET1(task_base->signal, XF_TASK_SIGNAL_RESUME);
    xf_task_manager_task_blocked(manager, task);

    xf_task_time_t resume_time = xf_task_get_ticks() - task_base->suspend_time;

    task_base->weakup += resume_time;

    return XF_OK;
}

xf_err_t xf_task_trigger(xf_task_t task)
{
    XF_ASSERT(task, XF_ERR_INVALID_ARG, TAG, "task must not be NULL");
    xf_task_base_t *handle = (xf_task_base_t *)task;

    BITS_SET1(handle->signal, XF_TASK_SIGNAL_EVENT);

    return XF_OK;
}

xf_task_type_t xf_task_get_type(xf_task_t task)
{
    XF_ASSERT(task, _XF_TASK_TYPE_NONE, TAG, "task must not be NULL");
    xf_task_base_t *handle = (xf_task_base_t *)task;

    return (xf_task_type_t)handle->type;
}

xf_task_state_t xf_task_get_state(xf_task_t task)
{
    XF_ASSERT(task, _XF_TASK_STATE_NONE, TAG, "task must not be NULL");
    xf_task_base_t *handle = (xf_task_base_t *)task;

    return (xf_task_state_t)handle->state;
}

uint16_t xf_task_get_priority(xf_task_t task)
{
    XF_ASSERT(task, 0, TAG, "task must not be NULL");
    xf_task_base_t *handle = (xf_task_base_t *)task;

    return (uint16_t)handle->priority;
}

xf_err_t xf_task_set_priority(xf_task_t task, uint16_t priority)
{
    XF_ASSERT(task, XF_ERR_INVALID_ARG, TAG, "task must not be NULL");
    XF_ASSERT(priority < XF_TASK_PRIORITY_LEVELS, XF_ERR_INVALID_ARG, TAG, "priority must less than %d",
              XF_TASK_PRIORITY_LEVELS);
    xf_task_base_t *handle = (xf_task_base_t *)task;

    if (handle->priority != priority) {
        handle->priority = priority;
        // 如果是等待队列中，则修改其所在的优先级队列
        if (handle->state == XF_TASK_STATE_READY) {
            xf_task_manager_task_ready(handle->manager, task);
        }
    }

    return XF_OK;
}

xf_err_t xf_task_set_delay(xf_task_t task, uint32_t delay_ms)
{
    XF_ASSERT(task, XF_ERR_INVALID_ARG, TAG, "task must not be NULL");
    xf_task_base_t *handle = (xf_task_base_t *)task;

    handle->delay = delay_ms;

    return XF_OK;
}

xf_err_t xf_task_set_func(xf_task_t task, xf_task_func_t func, void *arg)
{
    XF_ASSERT(task, XF_ERR_INVALID_ARG, TAG, "task must not be NULL");
    xf_task_base_t *handle = (xf_task_base_t *)task;

    if (handle->func != func) {
        handle->func = func;
    }

    if (handle->arg != arg) {
        handle->arg = arg;
    }

    return XF_OK;
}

#if XF_TASK_USER_DATA_IS_ENABLE
xf_err_t xf_task_set_user_data(xf_task_t task, void *user_data)
{
    XF_ASSERT(task, XF_ERR_INVALID_ARG, TAG, "task must not be NULL");
    xf_task_base_t *handle = (xf_task_base_t *)task;

    handle->user_data = user_data;

    return XF_OK;
}

void *xf_task_get_user_data(xf_task_t task)
{
    XF_ASSERT(task, NULL, TAG, "task must not be NULL");
    xf_task_base_t *handle = (xf_task_base_t *)task;

    return handle->user_data;
}
#endif // XF_TASK_USER_DATA_IS_ENABLE

/* ==================== [Static Functions] ================================== */
