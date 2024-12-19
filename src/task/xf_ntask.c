/**
 * @file xf_ntask.c
 * @author cangyu (sky.kirto@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-12-16
 *
 * @copyright Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

/* ==================== [Includes] ========================================== */

#include "xf_ntask.h"

#include "../kernel/xf_task_base.h"
#include "../port/xf_task_port_internal.h"

/* ==================== [Defines] =========================================== */

#define TAG "ntask"

/* ==================== [Typedefs] ========================================== */


typedef struct _xf_ntask_args_t {
    xf_list_t node;
    void *args;
    unsigned int size;
    unsigned int len;
    const char *name;
} xf_ntask_args_t;

typedef struct _xf_ntask_lc_t {
    uint32_t lc;
    const char *name;
    xf_list_t node;
} xf_ntask_lc_t;

typedef struct _xf_ntask_handle_t {
    xf_task_base_t base;                /*!< 继承父对象 */
    xf_ntask_compare_func_t compare;    /*!< 直到这个函数返回 0，会通过事件信号触发调度 */
    xf_ntask_status_t status;           /*!< 记录 ntask 退出状态 */
    xf_list_t lc_list;                  /*!< 记录 ntask 上下文 */
    xf_list_t args_list;                /*!< 参数收集器 */
} xf_ntask_handle_t;

/* ==================== [Static Prototypes] ================================= */

static void xf_ntask_reset(xf_task_t task);
static xf_task_time_t xf_ntask_update(xf_task_t task);
static void xf_ntask_exec(xf_task_manager_t manager);
static xf_task_t xf_ntask_constructor(xf_task_manager_t manager, xf_task_func_t func, void *func_arg, uint16_t priority,
                                      void *config);
static void *xf_ntask_args_create(xf_list_t *head, const char *name, unsigned int size, unsigned int len);
static void *xf_ntask_args_find(xf_list_t *head, const char *name);
static xf_err_t xf_ntask_args_set(xf_list_t *head, const char *name, void *args);
static xf_ntask_lc_t *xf_ntask_lc_find(xf_list_t *head, const char *name);
static xf_ntask_lc_t *xf_ntask_lc_create(xf_list_t *head, const char *name);
static void xf_ntask_args_remove_all(xf_list_t *head);
static void xf_ntask_lc_remove_all(xf_list_t *head);

/* ==================== [Static Variables] ================================== */

static const xf_task_vfunc_t _ntask_vfunc = {
    .constructor = xf_ntask_constructor,
    .exec = xf_ntask_exec,
    .reset = xf_ntask_reset,
    .update = xf_ntask_update,
};

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

void xf_ntask_vfunc_register(void)
{
    xf_task_vfunc_register(XF_TASK_TYPE_NTASK, &_ntask_vfunc);
}

void xf_ntask_set_compare(xf_task_t task, xf_ntask_compare_func_t compare)
{
    xf_ntask_handle_t *handle = (xf_ntask_handle_t *)task;

    handle->compare = compare;
}

int xf_ntask_args_get_int(xf_task_t *task, const char *name)
{
    xf_ntask_handle_t *handle = (xf_ntask_handle_t *)task;

    void *args = xf_ntask_args_find(&handle->args_list, name);

    return *(int *)args;
}

float xf_ntask_args_get_float(xf_task_t *task, const char *name)
{
    xf_ntask_handle_t *handle = (xf_ntask_handle_t *)task;

    void *args = xf_ntask_args_find(&handle->args_list, name);

    return *(float *)args;
}

void *xf_ntask_args_get_array(xf_task_t *task, const char *name)
{
    xf_ntask_handle_t *handle = (xf_ntask_handle_t *)task;

    void *args = xf_ntask_args_find(&handle->args_list, name);

    return args;
}

xf_err_t xf_ntask_args_set_int(xf_task_t *task, const char *name, int value)
{
    xf_ntask_handle_t *handle = (xf_ntask_handle_t *)task;

    xf_err_t err = xf_ntask_args_set(&handle->args_list, name, (void *)&value);

    if (err == XF_OK) {
        return XF_OK;
    }

    void *args = xf_ntask_args_create(&handle->args_list, name, sizeof(int), 1);

    if (args == NULL) {
        return XF_ERR_NO_MEM;
    }

    *(int *)args = value;

    return XF_OK;
}

xf_err_t xf_ntask_args_set_float(xf_task_t *task, const char *name, float value)
{
    xf_ntask_handle_t *handle = (xf_ntask_handle_t *)task;

    xf_err_t err = xf_ntask_args_set(&handle->args_list, name, (void *)&value);

    if (err == XF_OK) {
        return XF_OK;
    }


    void *args = xf_ntask_args_create(&handle->args_list, name, sizeof(float), 1);

    if (args == NULL) {
        return XF_ERR_NO_MEM;
    }

    *(float *)args = value;

    return XF_OK;
}

xf_err_t xf_ntask_args_set_array(xf_task_t *task, const char *name, void *value, unsigned int size, unsigned int len)
{
    xf_ntask_handle_t *handle = (xf_ntask_handle_t *)task;

    xf_err_t err = xf_ntask_args_set(&handle->args_list, name, value);

    if (err == XF_OK) {
        return XF_OK;
    }


    void *args = xf_ntask_args_create(&handle->args_list, name, size, len);

    if (args == NULL) {
        return XF_ERR_NO_MEM;
    }

    xf_memcpy(args, value, size * len);

    return XF_OK;
}

uint32_t xf_ntask_get_lc(xf_task_t *task, const char *name)
{
    xf_ntask_handle_t *handle = (xf_ntask_handle_t *)task;

    xf_ntask_lc_t *item = xf_ntask_lc_find(&handle->lc_list, name);
    if (item != NULL) {
        return item->lc;
    }
    item = xf_ntask_lc_create(&handle->lc_list, name);

    return item->lc;
}

xf_err_t xf_ntask_set_lc(xf_task_t *task, const char *name, uint32_t lc)
{
    xf_ntask_handle_t *handle = (xf_ntask_handle_t *)task;

    xf_ntask_lc_t *item = xf_ntask_lc_find(&handle->lc_list, name);
    if (item == NULL) {
        return XF_ERR_INVALID_ARG;
    }
    item->lc = lc;
    return XF_OK;
}

bool xf_ntask_lc_is_first(xf_task_t *task, const char *name)
{
    xf_ntask_handle_t *handle = (xf_ntask_handle_t *)task;

    xf_ntask_lc_t *item = xf_list_first_entry(&handle->lc_list, xf_ntask_lc_t, node);
    if (item->name == name) {
        return true;
    }
    return false;

}

xf_ntask_status_t xf_ntask_get_exit_status(xf_task_t *task)
{
    xf_ntask_handle_t *handle = (xf_ntask_handle_t *)task;

    return handle->status;
}

xf_err_t xf_ntask_set_exit_status(xf_task_t *task, xf_ntask_status_t status)
{
    xf_ntask_handle_t *handle = (xf_ntask_handle_t *)task;

    handle->status = status;
    return XF_OK;
}

/* ==================== [Static Functions] ================================== */

static xf_task_t xf_ntask_constructor(xf_task_manager_t manager, xf_task_func_t func, void *func_arg, uint16_t priority,
                                      void *config)
{
    xf_ntask_handle_t *task = (xf_ntask_handle_t *)xf_malloc(sizeof(xf_ntask_handle_t));

    if (task == NULL) {
        XF_LOGE(TAG, "memory alloc failed!");
        return NULL;
    }

    xf_task_base_init(&task->base, manager, XF_TASK_TYPE_NTASK, priority, func, func_arg);

    xf_list_init(&task->lc_list);
    xf_list_init(&task->args_list);
    task->compare = NULL;
    task->status = XF_NTASK_NONE;

    BITS_SET1(task->base.signal, XF_TASK_SIGNAL_EVENT);

    return (xf_task_t)task;
}

static void xf_ntask_reset(xf_task_t task)
{
    xf_ntask_handle_t *handle = (xf_ntask_handle_t *)task;

    xf_task_base_reset(&handle->base);

    xf_ntask_args_remove_all(&handle->args_list);
    xf_ntask_lc_remove_all(&handle->lc_list);
}

static void xf_ntask_time_handle(xf_task_t task, uint32_t time_ticks)
{
    xf_ntask_handle_t *handle = (xf_ntask_handle_t *)task;

    int32_t timeout = time_ticks - handle->base.weakup;
    // 转换超时时间，如果大于零则触发超时
    handle->base.timeout = xf_task_ticks_to_msec(timeout);
    if (timeout >= 0) {
        BITS_SET1(handle->base.signal, XF_TASK_SIGNAL_TIMEOUT);
        handle->base.delay = 0;
        handle->compare = NULL;
    }

}

static void xf_ntask_event_handle(xf_task_t task)
{
    xf_ntask_handle_t *handle = (xf_ntask_handle_t *)task;

    // 对比的调节如果符合则触发
    int res = handle->compare(task);
    if (res == 0) {
        handle->compare = NULL;
        handle->base.delay = 0;
        BITS_SET1(handle->base.signal, XF_TASK_SIGNAL_EVENT);
    }

}

static xf_task_time_t xf_ntask_update(xf_task_t task)
{
    xf_ntask_handle_t *handle = (xf_ntask_handle_t *)task;
    xf_task_time_t time_ticks = xf_task_get_ticks();

    if (handle->compare != NULL) {
        xf_ntask_event_handle(task);
    }

    if (handle->base.delay != 0) {
        xf_ntask_time_handle(task, time_ticks);
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

static void xf_ntask_exec(xf_task_manager_t manager)
{
    xf_ntask_handle_t *task = (xf_ntask_handle_t *)xf_task_manager_get_current_task(manager);

    xf_task_base_set_state(task, XF_TASK_STATE_RUNNING);
    task->base.func(task);

    if (task->status == XF_NTASK_FINSHED) {
        xf_ntask_lc_remove_all(&task->lc_list);
        xf_ntask_args_remove_all(&task->args_list);
        xf_task_delete(task);
    }
    task->status = XF_NTASK_NONE;
}

static void *xf_ntask_args_create(xf_list_t *head, const char *name, unsigned int size, unsigned int len)
{
    xf_ntask_args_t *args = (xf_ntask_args_t *)xf_malloc(sizeof(xf_ntask_args_t) + size * len);
    if (args == NULL) {
        return NULL;
    }
    args->args = (void *)((uint8_t *)args + sizeof(xf_ntask_args_t));
    args->name = name;
    args->size = size;
    args->len = len;
    xf_list_init(&args->node);

    xf_list_add_tail(&args->node, head);

    return args->args;
}

static void *xf_ntask_args_find(xf_list_t *head, const char *name)
{
    xf_ntask_args_t *item;
    xf_list_for_each_entry(item, head, xf_ntask_args_t, node) {
        if (item->name == name) {
            return item->args;
        }
    }
    xf_list_for_each_entry(item, head, xf_ntask_args_t, node) {
        if (xf_strcmp(item->name, name) == 0) {
            return item->args;
        }
    }
    return NULL;
}

static xf_err_t xf_ntask_args_set(xf_list_t *head, const char *name, void *args)
{
    xf_ntask_args_t *item = xf_ntask_args_find(head, name);
    if (item == NULL) {
        return XF_ERR_INVALID_ARG;
    }

    xf_memcpy(item->args, args, item->size * item->len);

    return XF_OK;
}

static void xf_ntask_args_remove_all(xf_list_t *head)
{
    xf_ntask_args_t *item, *_item;
    xf_list_for_each_entry_safe(item, _item, head, xf_ntask_args_t, node) {
        xf_list_del_init(&item->node);
        xf_free(item);
    }
}

static xf_ntask_lc_t *xf_ntask_lc_create(xf_list_t *head, const char *name)
{
    xf_ntask_lc_t *lc = (xf_ntask_lc_t *)xf_malloc(sizeof(xf_ntask_lc_t));
    if (lc == NULL) {
        return NULL;
    }

    xf_list_init(&lc->node);
    lc->lc = 0;
    lc->name = name;

    xf_list_add_tail(&lc->node, head);

    return lc;
}

static xf_ntask_lc_t *xf_ntask_lc_find(xf_list_t *head, const char *name)
{
    xf_ntask_lc_t *item;
    xf_list_for_each_entry(item, head, xf_ntask_lc_t, node) {
        if (item->name == name) {
            return item;
        }
    }

    return NULL;
}

static void xf_ntask_lc_remove_all(xf_list_t *head)
{
    xf_ntask_lc_t *item, *_item;
    xf_list_for_each_entry_safe(item, _item, head, xf_ntask_lc_t, node) {
        xf_list_del_init(&item->node);
        xf_free(item);
    }
}



