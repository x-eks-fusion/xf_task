/**
 * @file xf_task_pool.c
 * @author cangyu (sky.kirto@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-07-08
 *
 * @copyright Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

/* ==================== [Includes] ========================================== */

#include "xf_task_utils_config.h"

#if XF_TASK_POOL_IS_ENABLE

#include "xf_task_pool.h"
#include "../kernel/xf_task_base.h"
#include "../task/xf_task_default.h"
#include "../task/xf_ntask.h"

/* ==================== [Defines] =========================================== */

#define TAG "task_pool"

/* ==================== [Typedefs] ========================================== */

typedef struct _xf_task_pool_handle_t {
    uint32_t max_works;
    xf_list_t pool_list;
    xf_list_t used_list;
} xf_task_pool_handle_t;

typedef struct _xf_pool_task_t {
    xf_list_t node;
    xf_task_t task;
    xf_task_pool_handle_t *pool;
} xf_pool_task_t;


/* ==================== [Static Prototypes] ================================= */

static void xf_task_pool_default_task(xf_task_t task);
static void xf_task_delete_(xf_task_t task);

/* ==================== [Static Variables] ================================== */

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

xf_task_pool_t xf_task_pool_create_with_manager(uint32_t max_works, xf_task_manager_t manager, xf_task_type_t type,
        void *config)
{
    XF_ASSERT(max_works, NULL, TAG, "max_works must not be 0");
    XF_ASSERT(manager, NULL, TAG, "manager must not be NULL");
    XF_ASSERT(type < _XF_TASK_TYPE_MAX, NULL, TAG, "manager must less than %d", _XF_TASK_TYPE_MAX);
    XF_ASSERT(config, NULL, TAG, "config must not be NULL");

    xf_task_pool_handle_t *pool = (xf_task_pool_handle_t *)xf_malloc(sizeof(xf_task_pool_handle_t) +
                                  sizeof(xf_pool_task_t) * max_works);
    if (pool == NULL)
    {
        XF_LOGE(TAG, "memory alloc failed!");
        return NULL;
    }

    pool->max_works = max_works;
    xf_list_init(&pool->pool_list);
    xf_list_init(&pool->used_list);

    xf_pool_task_t *pool_task = (xf_pool_task_t *)((uint8_t *)pool + sizeof(xf_task_pool_handle_t));

    for (size_t i = 0; i < max_works; i++) {
        pool_task[i].task = xf_task_create_with_manager(manager, type, xf_task_pool_default_task, NULL, 0, config);
        xf_task_base_t *task_base = (xf_task_base_t *)pool_task[i].task;
        task_base->user_data = &pool_task[i];
        task_base->delete = xf_task_delete_;
        pool_task[i].pool = pool;
        xf_task_suspend(pool_task[i].task);
        xf_list_init(&pool_task[i].node);
        xf_list_add_tail(&pool_task[i].node, &pool->pool_list);
    }

    return pool;
}

xf_err_t xf_task_pool_delete(xf_task_pool_t pool)
{
    XF_ASSERT(pool, XF_ERR_INVALID_ARG, TAG, "pool must not be NULL");
    xf_task_pool_handle_t *pool_handle = (xf_task_pool_handle_t *)pool;

    xf_pool_task_t *task_pool;
    xf_list_for_each_entry(task_pool, &pool_handle->pool_list, xf_pool_task_t, node) {
        xf_task_base_t *handle = (xf_task_base_t *)task_pool->task;
        handle->delete = xf_task_destructor;
        xf_task_delete(task_pool->task);
    }
    xf_list_for_each_entry(task_pool, &pool_handle->used_list, xf_pool_task_t, node) {
        xf_task_base_t *handle = (xf_task_base_t *)task_pool->task;
        handle->delete = xf_task_destructor;
        xf_task_delete(task_pool->task);
    }
    xf_free(pool);

    return XF_OK;
}

xf_task_t xf_task_init_from_pool(xf_task_pool_t pool, xf_task_func_t func, void *func_arg, uint16_t priority)
{
    XF_ASSERT(pool, NULL, TAG, "pool must not be NULL");
    XF_ASSERT(func, NULL, TAG, "func must not be NULL");
    XF_ASSERT(priority < XF_TASK_PRIORITY_LEVELS, NULL, TAG, "priority must less than %d", XF_TASK_PRIORITY_LEVELS);

    xf_task_pool_handle_t *pool_handle = (xf_task_pool_handle_t *)pool;

    xf_pool_task_t *pool_task = xf_list_first_entry(&pool_handle->pool_list, xf_pool_task_t, node);

    xf_list_del_init(&pool_task->node);
    xf_list_add_tail(&pool_task->node, &pool_handle->used_list);

    xf_task_reset(pool_task->task);
    xf_task_base_t *handle = (xf_task_base_t *)pool_task->task;
    handle->func = func;
    handle->arg = func_arg;
    handle->priority = priority;

    return pool_task->task;
}

/* ==================== [Static Functions] ================================== */

static void xf_task_pool_default_task(xf_task_t task)
{
    UNUSED(task);
}

static void xf_task_delete_(xf_task_t task)
{
    xf_task_base_t *task_base = (xf_task_base_t *)task;
    xf_pool_task_t *pool_task = (xf_pool_task_t *)task_base->user_data;
    xf_task_pool_handle_t *pool = (xf_task_pool_handle_t *)pool_task->pool;

    // 此时已经出于删除态，
    // 需要强行修改当前状态，不然后续的挂起操作无法生效
    task_base->state = XF_TASK_STATE_READY;
    xf_task_suspend(task);

    xf_list_del_init(&pool_task->node);
    xf_list_add_tail(&pool_task->node, &pool->pool_list);
}

#endif
