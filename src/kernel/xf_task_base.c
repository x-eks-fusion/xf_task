/**
 * @file xf_task_base.c
 * @author cangyu (sky.kirto@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-07-05
 *
 * @copyright Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

/* ==================== [Includes] ========================================== */

#include "xf_task_base.h"
#include "xf_task.h"
#include "xf_task_manager.h"

/* ==================== [Defines] =========================================== */

#define TAG "task_base"

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

/* ==================== [Static Variables] ================================== */

static const xf_task_vfunc_t *_xf_task_vfunc_group[_XF_TASK_TYPE_MAX] = {0};

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

xf_err_t xf_task_vfunc_register(xf_task_type_t type, const xf_task_vfunc_t *vfunc)
{
    XF_ASSERT(type < _XF_TASK_TYPE_MAX, XF_ERR_INVALID_ARG, TAG, "type must less than %d", _XF_TASK_TYPE_MAX);
    XF_ASSERT(!_xf_task_vfunc_group[type], XF_ERR_INITED, TAG, "vfunc initialized");

    _xf_task_vfunc_group[type] = vfunc;

    return XF_OK;
}

xf_err_t xf_task_vfunc_unregister(xf_task_type_t type)
{
    XF_ASSERT(type < _XF_TASK_TYPE_MAX, XF_ERR_INVALID_ARG, TAG, "type must less than %d", _XF_TASK_TYPE_MAX);

    _xf_task_vfunc_group[type] = NULL;

    return XF_OK;
}

const xf_task_vfunc_t *xf_task_get_vfunc(xf_task_type_t type)
{
    XF_ASSERT(type < _XF_TASK_TYPE_MAX, NULL, TAG, "type must less than %d", _XF_TASK_TYPE_MAX);
    XF_ASSERT(_xf_task_vfunc_group[type], NULL, TAG, "vfunc uninitialized");

    return _xf_task_vfunc_group[type];
}

void xf_task_base_init(xf_task_base_t *task_base, xf_task_manager_t manager, xf_task_type_t type,
                       uint16_t priority, xf_task_func_t func, void *arg)
{
    task_base->manager = manager;
    task_base->func = func;
    task_base->arg = arg;
    task_base->priority = priority;
    task_base->type = type;
    task_base->signal = 0;
    task_base->flag = 0;
    task_base->delay = 0;
    task_base->weakup = 0;
    task_base->suspend_time = 0;
    task_base->timeout = 0;
    task_base->state = XF_TASK_STATE_BLOCKED;
    task_base->vfunc = _xf_task_vfunc_group[type];
    task_base->delete = xf_task_destructor;
    xf_list_init(&task_base->node);
    xf_task_manager_task_blocked(manager, task_base);
#if XF_TASK_HUNGER_IS_ENABLE
    xf_list_init(&task_base->hunger_node);
    task_base->hunger_time = 0;
#endif // XF_TASK_HUNGER_IS_ENABLE
#if XF_TASK_USER_DATA_IS_ENABLE
    task_base->user_data = NULL;
#endif
}

void xf_task_base_reset(xf_task_base_t *task_base)
{
    task_base->state = XF_TASK_STATE_BLOCKED;
    task_base->flag = 0;
    task_base->signal = 0;
    task_base->weakup = 0;
    task_base->suspend_time = 0;
    task_base->timeout = 0;
    xf_list_del_init(&task_base->node);
    xf_task_manager_task_blocked(task_base->manager, task_base);
#if XF_TASK_HUNGER_IS_ENABLE
    xf_list_del_init(&task_base->hunger_node);
    task_base->hunger_time = 0;
#endif // XF_TASK_HUNGER_IS_ENABLE
}

xf_err_t xf_task_base_set_state(xf_task_t task, xf_task_state_t state)
{

    xf_task_base_t *base = (xf_task_base_t *)task;

    if (state == XF_TASK_STATE_DELETE) {
        base->state = XF_TASK_STATE_DELETE;
        return XF_OK;
    }

    if (base->state == XF_TASK_STATE_DELETE) {
        XF_LOGD(TAG, "task will be delete");
        return XF_ERR_NOT_SUPPORTED;
    }

    if (base->state == XF_TASK_STATE_SUSPEND) {
        if (BITS_CHECK(base->signal, XF_TASK_SIGNAL_RESUME)) {
            BITS_SET0(base->signal, XF_TASK_SIGNAL_RESUME);

        } else {
            XF_LOGD(TAG, "task was suspend");
            return XF_ERR_NOT_SUPPORTED;
        }
    }

    base->state = state;

    return XF_OK;
}

void xf_task_destructor(xf_task_t task)
{
    xf_free(task);
    XF_LOGD(TAG, "task was delete");
}

/* ==================== [Static Functions] ================================== */


