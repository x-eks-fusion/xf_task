/**
 * @file xf_task.h
 * @author cangyu (sky.kirto@qq.com)
 * @brief xfuison 多任务实现。
 *  - ctask 有栈协程
 *  - ntask 无栈协程
 * @version 1.0
 * @date 2024-08-06
 *
 * Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

/**
 * @defgroup group_xf_task xf_task
 * @brief 提供基于有栈协程或者无栈协程的多任务支持。
 */

/**
 * @ingroup group_xf_task
 * @defgroup group_xf_task_user 用户接口
 * @brief 调用具体功能的接口。如创建任务、发布订阅、任务池等。
 *
 * xfusion 用户使用 xf_task 时只需 `#include "xf_task.h"` 即可。
 *
 */

/**
 * @ingroup group_xf_task
 * @defgroup group_xf_task_port 移植接口
 * @brief 用于对接 xf_task 的接口。
 *
 * 对接 xf_task 时同样只需 `#include "xf_task.h"` 即可。
 *
 */

/**
 * @ingroup group_xf_task
 * @defgroup group_xf_task_internal 内部接口
 * @brief 组件内部实现某些功能时定义的接口。用户不一定需要。
 */

#ifndef __XF_TASK_H__
#define __XF_TASK_H__

/* ==================== [Includes] ========================================== */

#include "xf_task_config_internal.h"
#include "port/xf_task_port.h"
#include "kernel/xf_task_kernel.h"
#include "task/xf_ctask.h"
#include "task/xf_ntask.h"
#include "task/xf_task_default.h"
#include "utils/xf_task_mbus.h"
#include "utils/xf_task_queue.h"
#include "utils/xf_task_pool.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Global Prototypes] ================================= */

#if XF_TASK_CONTEXT_IS_ENABLE

/**
 * @ingroup group_xf_task_user_ctask
 * @{
 */

/**
 * @brief 在默认的任务管理下，创建 ctask 任务。
 *
 * @param func ctask 任务执行的函数。
 * @param func_arg 用户自定义执行函数参数。
 * @param priority 任务优先级。
 * @param stack_size 任务上下文堆栈大小。
 * @return xf_task_t 任务对象，返回为 NULL 则表示创建失败
 */
static inline
xf_task_t xf_ctask_create(xf_task_func_t func, void *func_arg, uint16_t priority, size_t stack_size)
{
    xf_ctask_config_t config = {.stack_size = stack_size};
    return xf_task_create_with_manager(xf_task_get_default_manager(), XF_TASK_TYPE_CTASK, func, func_arg, priority,
                                       &config);
}

/**
 * @brief 延时函数。
 *
 * @attention 该函数只能在 ctask 任务中使用。
 *
 * @param delay_ms 延时的时间，单位为毫秒。
 */
static inline
void xf_ctask_delay(uint32_t delay_ms)
{
    xf_ctask_delay_with_manager(xf_task_get_default_manager(), delay_ms);
}

/**
 * @brief 创建 ctask 消息队列。
 *
 * @attention 该函数只能在 ctask 任务中使用。
 *
 * @param size 消息队列的大小。
 * @param count 消息队列的数量。
 * @return xf_ctask_queue_t 消息队列对象，返回为 NULL 则表示创建失败
 */
static inline
xf_ctask_queue_t xf_ctask_queue_create(const size_t size, const size_t count)
{
    return xf_ctask_queue_create_with_manager(xf_task_get_default_manager(), size, count);
}

/**
 * End of group_xf_task_user_ctask
 * @}
 */

#endif // XF_TASK_CONTEXT_IS_ENABLE

/**
 * @ingroup group_xf_task_user_ntask
 * @{
 */

/**
 * @brief 在默认的任务管理下，创建 ntask 任务。
 *
 * @param func ntask 任务执行的函数。
 * @param func_arg 用户自定义执行函数参数。
 * @param priority 任务优先级。
 * @param delay_ms 任务延时周期，单位为毫秒。
 * @param count 任务循环的次数上限。
 * @return xf_task_t 任务对象，返回为 NULL 则表示创建失败
 */
static inline
xf_task_t xf_ntask_create(xf_task_func_t func, void *func_arg, uint16_t priority, uint32_t delay_ms, uint32_t count)
{
    xf_ntask_config_t config = {.count = count, .delay_ms = delay_ms};
    return xf_task_create_with_manager(xf_task_get_default_manager(), XF_TASK_TYPE_NTASK, func, func_arg, priority,
                                       &config);
}

/**
 * @brief 在默认的任务管理下，创建 ntask 循环任务。
 *
 * @param func ntask 任务执行的函数。
 * @param func_arg 用户自定义执行函数参数。
 * @param priority 任务优先级。
 * @param delay_ms 任务延时周期，单位为毫秒。
 * @return xf_task_t 任务对象，返回为 NULL 则表示创建失败
 */
static inline
xf_task_t xf_ntask_create_loop(xf_task_func_t func, void *func_arg, uint16_t priority, uint32_t delay_ms)
{
    xf_ntask_config_t config = {.count = XF_NTASK_INFINITE_LOOP, .delay_ms = delay_ms};
    return xf_task_create_with_manager(xf_task_get_default_manager(), XF_TASK_TYPE_NTASK, func, func_arg, priority,
                                       &config);
}

/**
 * End of group_xf_task_user_ntask
 * @}
 */

#if XF_TASK_POOL_IS_ENABLE

#if XF_TASK_CONTEXT_IS_ENABLE

/**
 * @ingroup group_xf_task_user_ctask
 * @{
 */

/**
 * @brief 在指定的任务管理下，创建 ctask 任务池。
 *
 * @param max_works 任务池最大工作任务数。
 * @param manager 任务管理器。
 * @param stack_size 任务上下文堆栈大小。
 * @return xf_task_pool_t 任务池对象，返回为 NULL 则表示创建失败
 */
static inline xf_task_pool_t xf_ctask_pool_create_with_manager(uint32_t max_works, xf_task_manager_t manager,
    size_t stack_size)
{
    xf_ctask_config_t config = {.stack_size = stack_size};
    return xf_task_pool_create_with_manager(max_works, manager, XF_TASK_TYPE_ctask, &config);
}

/**
 * @brief 在默认的任务管理下，创建 ctask 任务池。
 *
 * @param max_works 任务池最大工作任务数。
 * @param stack_size 任务上下文堆栈大小。
 * @return xf_task_pool_t 任务池对象，返回为 NULL 则表示创建失败
 */
static inline xf_task_pool_t xf_ctask_pool_create(uint32_t max_works, size_t stack_size)
{
    xf_ctask_config_t config = {.stack_size = stack_size};
    return xf_task_pool_create_with_manager(max_works, xf_task_get_default_manager(), XF_TASK_TYPE_CTASK, &config);
}

/**
 * End of group_xf_task_user_ctask
 * @}
 */

#endif // XF_TASK_CONTEXT_IS_ENABLE

/**
 * @ingroup group_xf_task_user_ntask
 * @{
 */

/**
 * @brief 在指定的任务管理下，创建 ntask 任务池。
 *
 * @param max_works 任务池最大工作任务数。
 * @param manager 任务管理器。
 * @param delay_ms 任务延时，单位为毫秒。
 * @param count 任务循环的次数上限。
 * @return xf_task_pool_t 任务池对象，返回为 NULL 则表示创建失败
 */
static inline xf_task_pool_t xf_ntask_pool_create_with_manager(uint32_t max_works, xf_task_manager_t manager,
    uint32_t delay_ms, uint32_t count)
{
    xf_ntask_config_t config = {.count = count, .delay_ms = delay_ms};
    return xf_task_pool_create_with_manager(max_works, manager, XF_TASK_TYPE_NTASK, &config);
}

/**
 * @brief 在默认的任务管理下，创建 ntask 任务池。
 *
 * @param max_works 任务池最大工作任务数。
 * @param delay_ms 任务延时，单位为毫秒。
 * @param count 任务循环的次数上限。
 * @return xf_task_pool_t 任务池对象，返回为 NULL 则表示创建失败
 */
static inline xf_task_pool_t xf_ntask_pool_create(uint32_t max_works, uint32_t delay_ms, uint32_t count)
{
    xf_ntask_config_t config = {.count = count, .delay_ms = delay_ms};
    return xf_task_pool_create_with_manager(max_works, xf_task_get_default_manager(), XF_TASK_TYPE_NTASK, &config);
}

/**
 * End of group_xf_task_user_ntask
 * @}
 */

#endif // XF_TASK_POOL_IS_ENABLE

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __XF_TASK_H__
