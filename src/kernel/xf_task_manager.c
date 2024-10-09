/**
 * @file xf_task_manager.c
 * @author cangyu (sky.kirto@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-02-29
 *
 * @copyright Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

/* ==================== [Includes] ========================================== */

#include "xf_task_kernel_config.h"
#include "../port/xf_task_port_internal.h"
#include "xf_task_manager.h"
#include "xf_task_base.h"
#include "xf_utils.h"

/* ==================== [Defines] =========================================== */

#define TAG "manager"

/* ==================== [Typedefs] ========================================== */

typedef struct _xf_task_manager_handle_t {
    xf_task_t current_task;                         /*!< 当前执行任务 */
    xf_task_t urgent_task;                          /*!< 紧急任务 */
    xf_list_t ready_list[XF_TASK_PRIORITY_LEVELS];  /*!< 任务就绪队列 */
    xf_list_t blocked_list;                         /*!< 任务阻塞队列 */
    xf_list_t suspend_list;                         /*!< 任务挂起队列，挂起任务不参与调度，需要手动恢复 */
    xf_list_t destroy_list;                         /*!< 任务销毁队列，进行异步销毁 */
    xf_task_on_idle_t on_idle;                      /*!< 空闲任务回调 */
#if XF_TASK_HUNGER_IS_ENABLE
    xf_list_t hunger_list;                          /*!< 任务饥饿队列，达到其指定值进行跳跃 */
#endif // XF_TASK_HUNGER_IS_ENABLE
#if XF_TASK_CONTEXT_IS_ENABLE
    xf_task_context_t context;                      /*!< 调度器上下文 */
#endif // XF_TASK_CONTEXT_IS_ENABLE
} xf_task_manager_handle_t;


/* ==================== [Static Prototypes] ================================= */

static inline void xf_task_run(xf_task_base_t *task);
static inline void xf_task_update_timeout(xf_task_base_t *task);

/* ==================== [Static Variables] ================================== */

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

xf_task_manager_t xf_task_manager_create(xf_task_on_idle_t on_idle)
{
    xf_task_manager_handle_t *manager = (xf_task_manager_handle_t *)xf_malloc(sizeof(xf_task_manager_handle_t));
    XF_ASSERT(manager, NULL, TAG, "memory alloc failed!");

    manager->current_task = NULL;
    manager->urgent_task = NULL;
    manager->on_idle = on_idle;

    for (size_t i = 0; i < XF_TASK_PRIORITY_LEVELS; i++) {
        xf_list_init(&manager->ready_list[i]);
    }
    xf_list_init(&manager->blocked_list);
    xf_list_init(&manager->destroy_list);
    xf_list_init(&manager->suspend_list);
#if XF_TASK_HUNGER_IS_ENABLE
    xf_list_init(&manager->hunger_list);
#endif // XF_TASK_HUNGER_IS_ENABLE

    return (xf_task_manager_t)manager;
}

xf_err_t xf_task_manager_set_idle(xf_task_manager_t manager, xf_task_on_idle_t on_idle)
{
    XF_ASSERT(manager, XF_ERR_INVALID_ARG, TAG, "manager must not be NULL!");
    XF_ASSERT(on_idle, XF_ERR_INVALID_ARG, TAG, "on_idle must not be NULL!");

    xf_task_manager_handle_t *manager_handle = (xf_task_manager_handle_t *)manager;
    manager_handle->on_idle = on_idle;
    return XF_OK;
}

void xf_task_manager_delete(xf_task_manager_t manager)
{
    XF_ASSERT(manager, XF_RETURN_VOID, TAG, "manager must not be NULL!");
    xf_free(manager);
}

void xf_task_manager_run(xf_task_manager_t manager)
{
    XF_ASSERT(manager, XF_RETURN_VOID, TAG, "manager_handle must not be NULL");

    xf_task_manager_handle_t *manager_handle = (xf_task_manager_handle_t *)manager;
    volatile uint32_t index = 0;
    // 阻塞的最小时间，即是空闲的最大时间
    volatile int32_t max_idle_ms = INT32_MAX;
    // 保存ticks用于后续校准最大空闲
    volatile uint32_t idle_time_ticks = 0;
    volatile bool is_get_func = false;
    xf_task_base_t *task, *_task;

    // 阻塞任务队列处理
    xf_list_for_each_entry_safe(task, _task, &manager_handle->blocked_list, xf_task_base_t, node) {
        // 更新信号
        uint32_t time_ticks = task->vfunc->update(task);

        // 检查信号，如果符合则加入就绪
        if (BITS_CHECK(task->signal, XF_TASK_SIGNAL_READY)) {
            xf_list_del_init(&task->node);
            xf_task_base_set_state(task, XF_TASK_STATE_READY); // 设置为就绪态
            xf_list_add_tail(&task->node, &manager_handle->ready_list[task->priority]);
            BITS_SET0(task->signal, XF_TASK_SIGNAL_READY);
#if XF_TASK_HUNGER_IS_ENABLE
            if (BITS_CHECK(task->flag, XF_TASK_FALG_FEEL_HUNGERY)) {
                xf_list_add_tail(&task->hunger_node, &manager_handle->hunger_list);
            }
#endif // XF_TASK_HUNGER_IS_ENABLE
        }

        // 事件触发任务，这里始终等于0，不会被计算进入
        // 计算阻塞任务中最小时间，如果进入空闲，则这里的时间是空闲任务最大时间
        if (task->delay == 0 || task->timeout > 0 || -task->timeout > max_idle_ms || task->state != XF_TASK_STATE_BLOCKED) {
            continue;
        }
        max_idle_ms = -task->timeout;
        idle_time_ticks = time_ticks;
    }

    // 如果有紧急任务则优先执行紧急任务，并跳过后续调度
    if (NULL != manager_handle->urgent_task) {
        task = (xf_task_base_t *)manager_handle->urgent_task;
        manager_handle->urgent_task = NULL;
        xf_task_run(task);
        return;
    }

    // 就绪任务队列处理
    // 这里决定了它的优先级数值越小优先级越高
    for (index = 0; index < XF_TASK_PRIORITY_LEVELS; index++) {
        if (xf_list_empty(&manager_handle->ready_list[index])) {
            continue;
        }
        // 选取相对最高优先级的任务作为执行任务
        if (false == is_get_func) {
            task = xf_list_first_entry(&manager_handle->ready_list[index], xf_task_base_t, node);
            xf_task_run(task);
            is_get_func = true;
            break;
        }
    }

    // 上述循环正常退出，则说明没有就绪任务，运行空闲任务
    if (false == is_get_func) {
        // 空闲时间，处理一下需要删除的任务
        xf_list_for_each_entry_safe(task, _task, &manager_handle->destroy_list, xf_task_base_t, node) {
            xf_list_del_init(&task->node);
            task->delete (task);
        }
        // 进一步修正空闲时间
        xf_task_time_t ticks = xf_task_get_ticks();
        max_idle_ms -=  xf_task_ticks_to_msec(ticks - idle_time_ticks);
        max_idle_ms = max_idle_ms < 0 ? 0 : max_idle_ms;
        // 执行空闲回调
        if (manager_handle->on_idle != NULL) {
            manager_handle->on_idle(max_idle_ms);
        }
    }
#if XF_TASK_HUNGER_IS_ENABLE
    else {
        // 对事件触发任务一视同仁
        // 对感受饥饿的任务进行临时优先级跳跃
        xf_list_for_each_entry_safe(task, _task, &manager_handle->hunger_list, xf_task_base_t, hunger_node) {
            xf_task_update_timeout(task);

            // 计算爬升等级
            uint32_t level = task->timeout / task->hunger_time;

            // 限制爬升等级
            int priority = (int)task->priority - (int)level;
            if (priority < 0) {
                priority = 0;
            }

            // 重置其优先级
            xf_list_del_init(&task->node);
            xf_list_add(&task->node, &manager_handle->ready_list[priority]);
        }
    }
#endif // XF_TASK_HUNGER_IS_ENABLE

}

xf_task_t xf_task_manager_get_current_task(xf_task_manager_t manager)
{
    XF_ASSERT(manager, NULL, TAG, "manager must not be NULL!");

    xf_task_manager_handle_t *manager_handle = (xf_task_manager_handle_t *)manager;

    return manager_handle->current_task;
}

xf_err_t xf_task_manager_task_ready(xf_task_manager_t manager, xf_task_t task)
{
    XF_ASSERT(manager, XF_ERR_INVALID_ARG, TAG, "manager must not be NULL!");

    xf_task_manager_handle_t *manager_handle = (xf_task_manager_handle_t *)manager;

    xf_task_base_t *task_base = task;

    xf_list_del_init(&task_base->node);

    xf_task_base_set_state(task, XF_TASK_STATE_READY);
    xf_list_add_tail(&task_base->node, &manager_handle->ready_list[task_base->priority]);

    return XF_OK;
}

xf_err_t xf_task_manager_task_suspend(xf_task_manager_t manager, xf_task_t task)
{
    XF_ASSERT(manager, XF_ERR_INVALID_ARG, TAG, "manager must not be NULL!");

    xf_task_manager_handle_t *manager_handle = (xf_task_manager_handle_t *)manager;

    xf_task_base_t *task_base = task;

    xf_list_del_init(&task_base->node);

    xf_task_base_set_state(task, XF_TASK_STATE_SUSPEND);
    xf_list_add_tail(&task_base->node, &manager_handle->suspend_list);

    return XF_OK;
}

xf_err_t xf_task_manager_task_destory(xf_task_manager_t manager, xf_task_t task)
{
    XF_ASSERT(manager, XF_ERR_INVALID_ARG, TAG, "manager must not be NULL!");

    xf_task_manager_handle_t *manager_handle = (xf_task_manager_handle_t *)manager;

    xf_task_base_t *task_base = task;

    xf_list_del_init(&task_base->node);

    xf_task_base_set_state(task, XF_TASK_STATE_DELETE);
    xf_list_add_tail(&task_base->node, &manager_handle->destroy_list);

    return XF_OK;
}

xf_err_t xf_task_manager_task_blocked(xf_task_manager_t manager, xf_task_t task)
{
    XF_ASSERT(manager, XF_ERR_INVALID_ARG, TAG, "manager must not be NULL!");

    xf_task_manager_handle_t *manager_handle = (xf_task_manager_handle_t *)manager;

    xf_task_base_t *task_base = task;

    xf_list_del_init(&task_base->node);

    xf_task_base_set_state(task, XF_TASK_STATE_BLOCKED);
    xf_list_add_tail(&task_base->node, &manager_handle->blocked_list);

    return XF_OK;
}

#if XF_TASK_CONTEXT_IS_ENABLE
xf_task_context_t *xf_task_manager_get_context(xf_task_manager_t manager)
{
    xf_task_manager_handle_t *manager_handle = (xf_task_manager_handle_t *)manager;

    return &manager_handle->context;
}
#endif // XF_TASK_CONTEXT_IS_ENABLE


xf_err_t xf_task_set_urgent_task_with_manager(xf_task_manager_t manager, xf_task_t task, bool force)
{
    XF_ASSERT(manager, XF_ERR_INVALID_ARG, TAG, "manager must not be NULL");
    XF_ASSERT(task, XF_ERR_INVALID_ARG, TAG, "task must not be NULL");

    xf_task_manager_handle_t *manager_handle = (xf_task_manager_handle_t *)manager;

    if (manager_handle->urgent_task != NULL && !force) {
        return XF_ERR_BUSY;
    }

    manager_handle->urgent_task = task;
    xf_task_base_set_state(task, XF_TASK_STATE_READY);

    return XF_OK;
}

/* ==================== [Static Functions] ================================== */


static inline void xf_task_run(xf_task_base_t *task)
{
    xf_task_manager_handle_t *manager = (xf_task_manager_handle_t *)task->manager;

#if XF_TASK_HUNGER_IS_ENABLE
    if (BITS_CHECK(task->flag, XF_TASK_FALG_FEEL_HUNGERY)) {
        xf_list_del_init(&task->hunger_node);
    }
#endif // XF_TASK_HUNGER_IS_ENABLE

    xf_list_del_init(&task->node);                   // 从原有链表中脱离
    manager->current_task = task;                       // 放入当前执行的任务
    xf_task_update_timeout(task);
    task->vfunc->exec(manager);                           // 执行任务
    manager->current_task = NULL;

    // 如果设置成功，则进入阻塞状态。如果设置不成功（删除或挂起）则不管它
    if (xf_task_base_set_state(task, XF_TASK_STATE_BLOCKED) == XF_OK) {
        xf_list_add_tail(&task->node, &manager->blocked_list);
    }
}

static inline void xf_task_update_timeout(xf_task_base_t *task)
{
    xf_task_time_t timeout = xf_task_get_ticks() - task->weakup;
    task->timeout = xf_task_ticks_to_msec(timeout);
}
