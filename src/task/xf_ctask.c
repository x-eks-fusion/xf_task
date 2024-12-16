/**
 * @file xf_ctask.c
 * @author cangyu (sky.kirto@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-02-28
 *
 * @copyright Copyright (c) 2024, CorAL. All rights reserved.
 *
 */
/* ==================== [Includes] ========================================== */

#include "xf_ctask.h"
#include "../utils/xf_task_queue.h"
#include "../port/xf_task_port_internal.h"
#include "../kernel/xf_task_base.h"


#if XF_TASK_CONTEXT_IS_ENABLE

/* ==================== [Defines] =========================================== */

#define TAG "ctask"

/* ==================== [Typedefs] ========================================== */

typedef struct _xf_ctask_handle_t {
    xf_task_base_t base;        /*!< 继承 task_base 父对象 */
    size_t stack_size;          /*!< 任务上下文堆栈大小 */
    xf_task_context_t context;  /*!< 任务上下文对象 */
    void *stack;                /*!< 任务上下文堆栈地址 */
    xf_list_t queue_node;       /*!< 队列等待 */
} xf_ctask_handle_t;

typedef struct _xf_ctask_queue_handle_t {
    xf_task_queue_t queue;
    xf_task_manager_t manager;
    xf_list_t  send_waiting;
    xf_list_t  receive_waiting;
} xf_ctask_queue_handle_t;

/* ==================== [Static Prototypes] ================================= */

static void xf_task_context_entry(void *args);
static void xf_ctask_reset(xf_task_t task);
static void xf_ctask_yield(xf_task_manager_t manager);
static void xf_ctask_resume(xf_task_manager_t manager);
static xf_task_time_t xf_ctask_update(xf_task_t task);
static void xf_ctask_exec(xf_task_manager_t manager);
static xf_task_t xf_ctask_constructor(xf_task_manager_t manager, xf_task_func_t func, void *func_arg, uint16_t priority,
                                      void *config);

/* ==================== [Static Variables] ================================== */

static const xf_task_vfunc_t _ctask_vfunc = {
    .constructor = xf_ctask_constructor,
    .reset = xf_ctask_reset,
    .exec = xf_ctask_exec,
    .update = xf_ctask_update
};

/* ==================== [Macros] ============================================ */

void xf_ctask_vfunc_register(void)
{
    xf_task_vfunc_register(XF_TASK_TYPE_CTASK, &_ctask_vfunc);
}

/* ==================== [Global Functions] ================================== */

void xf_ctask_delay_with_manager(xf_task_manager_t manager, uint32_t delay_ms)
{
    XF_ASSERT(manager, XF_RETURN_VOID, TAG, "manager must not be NULL");

    xf_ctask_handle_t *task = xf_task_manager_get_current_task(manager);

    if (task->base.type != XF_TASK_TYPE_CTASK) {
        XF_LOGE(TAG, "only ctask can use this function");
        return;
    }


    int32_t ticks = xf_task_msec_to_ticks(delay_ms);

    task->base.delay = ticks;
    task->base.weakup = xf_task_get_ticks() + ticks;
    xf_ctask_yield(manager);

}

xf_ctask_queue_t xf_ctask_queue_create_with_manager(xf_task_manager_t  manager, const size_t size, const size_t count)
{
    XF_ASSERT(manager, NULL, TAG, "manager must not be NULL");
    XF_ASSERT(size, NULL, TAG, "size must not be 0");
    XF_ASSERT(count, NULL, TAG, "count must not be 0");

    xf_ctask_queue_handle_t *ctask_queue = (xf_ctask_queue_handle_t *)xf_malloc(sizeof(xf_ctask_queue_handle_t) +
                                           size * count);

    if (ctask_queue == NULL) {
        XF_LOGE(TAG, "memory alloc failed!");
        return NULL;
    }

    xf_bzero(ctask_queue, sizeof(xf_ctask_queue_handle_t) + size * count);

    void *data = (uint8_t *)ctask_queue + sizeof(xf_ctask_queue_handle_t);

    ctask_queue->manager = manager;

    xf_task_queue_init(&ctask_queue->queue, data, size, count);
    xf_list_init(&ctask_queue->receive_waiting);
    xf_list_init(&ctask_queue->send_waiting);

    return (xf_ctask_queue_t)ctask_queue;
}

void xf_ctask_queue_delete(xf_ctask_queue_t queue)
{
    XF_ASSERT(queue, XF_RETURN_VOID, TAG, "queue must not be NULL");

    xf_free(queue);
}

xf_err_t xf_ctask_queue_send(xf_ctask_queue_t queue, void *buffer, uint32_t timeout)
{
    XF_ASSERT(queue, XF_ERR_INVALID_ARG, TAG, "queue must not be NULL");
    XF_ASSERT(buffer, XF_ERR_INVALID_ARG, TAG, "buffer must not be NULL");

    xf_task_queue_t *_queue = &((xf_ctask_queue_handle_t *)queue)->queue;
    xf_list_t *send_waiting = &((xf_ctask_queue_handle_t *)queue)->send_waiting;
    xf_list_t *receive_waiting = &((xf_ctask_queue_handle_t *)queue)->receive_waiting;
    xf_task_manager_t manager = ((xf_ctask_queue_handle_t *)queue)->manager;
    xf_task_base_t *task = xf_task_manager_get_current_task(manager);
    xf_list_t *queue_node = &((xf_ctask_handle_t *)task)->queue_node;
    xf_ctask_handle_t *receive_task, *_receive_task;

    // 只有ctask才能调用
    if (XF_TASK_STATE_RUNNING != task->state) {
        XF_LOGE(TAG, "task state must RUNNING");
        return XF_ERR_BUSY;
    }

    if (XF_TASK_TYPE_CTASK != task->state) {
        XF_LOGE(TAG, "task must ctask");
        return XF_ERR_NOT_SUPPORTED;
    }

    while (1) {
        if (xf_task_queue_is_full(_queue)) {
            xf_list_add_tail(queue_node, send_waiting);
            // 这里有可能会被接收方唤醒，从而延时未达到timeout
            xf_ctask_delay_with_manager(manager, timeout);
            // 达到超时返回发送失败
            if (task->timeout >= 0) {
                XF_LOGD(TAG, "queue timeout");
                return XF_ERR_TIMEOUT;
            }
            // 没达到超时进入循环继续进行接下来的超时
            timeout = -task->timeout;
        } else {
            xf_task_queue_send(_queue, buffer, XF_TASK_QUEUE_SEND_TO_BACK);

            // 将 receive_waiting 全部加入就绪，等待调度器选择最合适的任务
            xf_list_for_each_entry_safe(receive_task, _receive_task, receive_waiting, xf_ctask_handle_t, queue_node) {
                xf_list_del_init(&receive_task->queue_node);
                xf_task_trigger(receive_task);
            }

            return XF_OK;
        }
    }
}

xf_err_t xf_ctask_queue_receive(xf_ctask_queue_t queue, void *buffer, uint32_t timeout)
{
    XF_ASSERT(queue, XF_ERR_INVALID_ARG, TAG, "queue must not be NULL");
    XF_ASSERT(buffer, XF_ERR_INVALID_ARG, TAG, "buffer must not be NULL");

    xf_task_queue_t *_queue = &((xf_ctask_queue_handle_t *)queue)->queue;
    xf_list_t *send_waiting = &((xf_ctask_queue_handle_t *)queue)->send_waiting;
    xf_list_t *receive_waiting = &((xf_ctask_queue_handle_t *)queue)->receive_waiting;
    xf_task_manager_t manager = ((xf_ctask_queue_handle_t *)queue)->manager;
    xf_task_base_t *task = xf_task_manager_get_current_task(manager);
    xf_list_t *queue_node = &((xf_ctask_handle_t *)task)->queue_node;
    xf_ctask_handle_t *send_task, *_send_task;

    // 只有ctask才能调用
    if (XF_TASK_STATE_RUNNING != task->state) {
        XF_LOGE(TAG, "task state must RUNNING");
        return XF_ERR_BUSY;
    }

    if (XF_TASK_TYPE_CTASK != task->type) {
        XF_LOGE(TAG, "task must ctask");
        return XF_ERR_NOT_SUPPORTED;
    }

    while (1) {
        if (xf_task_queue_is_empty(_queue)) {
            xf_list_add_tail(queue_node, receive_waiting);
            // 这里有可能会被发送方唤醒，从而延时未达到timeout
            xf_ctask_delay_with_manager(manager, timeout);
            // 达到超时返回发送失败
            if (task->timeout >= 0) {
                XF_LOGD(TAG, "queue timeout");
                return XF_ERR_TIMEOUT;
            }
            // 没达到超时进入循环继续进行接下来的超时
            timeout = -task->timeout;
        } else {
            xf_task_queue_receive(_queue, buffer);

            // 将send_waiting全部加入就绪，等待调度器选择最合适的任务
            xf_list_for_each_entry_safe(send_task, _send_task, send_waiting, xf_ctask_handle_t, queue_node) {
                xf_list_del_init(&send_task->queue_node);
                xf_task_trigger(send_task);
            }

            return XF_OK;
        }
    }
}

/* ==================== [Static Functions] ================================== */

static xf_task_t xf_ctask_constructor(xf_task_manager_t manager, xf_task_func_t func, void *func_arg, uint16_t priority,
                                      void *config)
{
    size_t stack_size = ((xf_ctask_config_t *)config)->stack_size;

    XF_ASSERT(stack_size > 0, NULL, TAG, "args must more than 0");

    xf_ctask_handle_t *task = (xf_ctask_handle_t *)xf_malloc(sizeof(xf_ctask_handle_t) + stack_size);

    if (task == NULL) {
        XF_LOGE(TAG, "memory alloc failed!");
        return NULL;
    }

    task->stack = (void *)((uint8_t *)task + sizeof(xf_ctask_handle_t));

    xf_task_base_init(&task->base, manager, XF_TASK_TYPE_CTASK, priority, func, func_arg);

    task->stack_size = ((xf_ctask_config_t *)config)->stack_size;
    xf_task_context_create(manager, xf_task_context_entry, &task->context, task->stack, task->stack_size);

    xf_list_init(&task->queue_node);


    return (xf_task_t)task;
}


static xf_task_time_t xf_ctask_update(xf_task_t task)
{
    xf_ctask_handle_t *handle = (xf_ctask_handle_t *)task;

    xf_task_time_t time_ticks = xf_task_get_ticks();

    int32_t timeout = time_ticks - handle->base.weakup;

    // 转换超时时间，如果大于零则触发超时
    handle->base.timeout = xf_task_ticks_to_msec(timeout);

    if (timeout >= 0) {
        BITS_SET1(handle->base.signal, XF_TASK_SIGNAL_TIMEOUT);
    }

    // 对超时信号响应
    if (BITS_CHECK(handle->base.signal, XF_TASK_SIGNAL_TIMEOUT)) {
        BITS_SET0(handle->base.signal, XF_TASK_SIGNAL_TIMEOUT);
        BITS_SET1(handle->base.signal, XF_TASK_SIGNAL_READY);
    }

    // 对事件信号响应
    if (BITS_CHECK(handle->base.signal, XF_TASK_SIGNAL_EVENT)) {
        BITS_SET0(handle->base.signal, XF_TASK_SIGNAL_EVENT);
        BITS_SET1(handle->base.signal, XF_TASK_SIGNAL_READY);
    }

    return time_ticks;
}

static void xf_ctask_reset(xf_task_t task)
{
    xf_ctask_handle_t *handle = (xf_ctask_handle_t *)task;

    handle->base.delay = 0;

    xf_task_base_reset(&handle->base);

    xf_list_del_init(&handle->queue_node);

    xf_task_context_create(handle->base.manager, xf_task_context_entry, &handle->context, handle->stack,
                           handle->stack_size);

}

static void xf_task_context_entry(void *args)
{
    xf_task_manager_t manager = (xf_task_manager_t)args;
    xf_ctask_handle_t *task = (xf_ctask_handle_t *)xf_task_manager_get_current_task(manager);

    // 执行任务函数
    (task->base.func)(task);

    // 函数运行到结尾，设置结尾标志位
    xf_task_delete(task);
    // 函数运行完毕，回到调度器
    xf_task_context_swap(manager, &task->context, xf_task_manager_get_context(manager));
}

static void xf_ctask_yield(xf_task_manager_t manager)
{
    xf_ctask_handle_t *task = (xf_ctask_handle_t *)xf_task_manager_get_current_task(manager);

    // 在函数内才能退出函数
    if (XF_TASK_STATE_RUNNING != task->base.state) {
        XF_LOGD(TAG, "task must running");
        return ;
    }

    // 跳出函数，进入调度器
    xf_task_context_swap(manager, &task->context, xf_task_manager_get_context(manager));
}

static void xf_ctask_resume(xf_task_manager_t manager)
{
    xf_ctask_handle_t *task = (xf_ctask_handle_t *)xf_task_manager_get_current_task(manager);

    // 只有在调度器内才能调用恢复
    if (XF_TASK_STATE_RUNNING == task->base.state || XF_TASK_STATE_DELETE == task->base.state) {
        XF_LOGD(TAG, "task must not running and delete");
        return;
    }

    // 跳出调度器，进入函数
    xf_task_base_set_state(task, XF_TASK_STATE_RUNNING);
    xf_task_context_swap(manager, xf_task_manager_get_context(manager), &task->context);
}

static void xf_ctask_exec(xf_task_manager_t manager)
{
    xf_ctask_resume(manager);
}

#endif // XF_TASK_CONTEXT_IS_ENABLE
