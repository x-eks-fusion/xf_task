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
#include "../task/xf_ttask.h"

/* ==================== [Defines] =========================================== */

#define TAG "task_pool"

#define XF_TASK_POOL_DEFAULT_TASK xf_task_empty
#define XF_TASK_POOL_RECYCLE xf_task_empty

/* ==================== [Typedefs] ========================================== */

typedef struct _xf_task_pool_handle_t {
    uint32_t max_works;
    xf_task_t *tasks;
} xf_task_pool_handle_t;

/* ==================== [Static Prototypes] ================================= */

static void xf_task_empty(xf_task_t task);

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
                                  sizeof(xf_task_t) * max_works);
    if (pool == NULL) {
        XF_LOGE(TAG, "memory alloc failed!");
        return NULL;
    }

    pool->max_works = max_works;

    pool->tasks = (xf_task_t *)((uint8_t *)pool + sizeof(xf_task_pool_handle_t));

    for (size_t i = 0; i < max_works; i++) {
        pool->tasks[i] = xf_task_create_with_manager(manager, type, XF_TASK_POOL_DEFAULT_TASK, NULL, 0, config);
        xf_task_base_t *task_base = (xf_task_base_t *)pool->tasks[i];
        task_base->delete = XF_TASK_POOL_RECYCLE;
        xf_task_delete(pool->tasks[i]);
    }

    return pool;
}

xf_err_t xf_task_pool_delete(xf_task_pool_t pool)
{
    XF_ASSERT(pool, XF_ERR_INVALID_ARG, TAG, "pool must not be NULL");
    xf_task_pool_handle_t *pool_handle = (xf_task_pool_handle_t *)pool;

    // 遍历，删除所有任务
    for (size_t i = 0; i < pool_handle->max_works; i++) {
        xf_task_base_t *task_base = (xf_task_base_t *)pool_handle->tasks[i];
        task_base->delete = xf_task_destructor;
        xf_task_delete(task_base);
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

    // 遍历任务，找到被回收的任务，重复使用
    for (size_t i = 0; i < pool_handle->max_works; i++) {
        xf_task_base_t *task_base = (xf_task_base_t *)pool_handle->tasks[i];
        if (task_base->state == XF_TASK_STATE_DELETE) {
            xf_task_reset(task_base);
            task_base->func = func;
            task_base->arg = func_arg;
            task_base->priority = priority;
            return task_base;
        }
    }
    XF_LOGW(TAG, "no task can be init from pool");
    return NULL;
}

/* ==================== [Static Functions] ================================== */

static void xf_task_empty(xf_task_t task)
{
    UNUSED(task);
}

#endif
