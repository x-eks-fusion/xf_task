/**
 * @file xf_ctask.h
 * @author cangyu (sky.kirto@qq.com)
 * @brief 有栈协程。
 * @version 0.1
 * @date 2024-02-29
 *
 * @copyright Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

#ifndef __XF_CTASK_H__
#define __XF_CTASK_H__

/* ==================== [Includes] ========================================== */

#include "../kernel/xf_task_kernel.h"

/**
 * @ingroup group_xf_task_user
 * @defgroup group_xf_task_user_ctask ctask
 * @brief 有栈协程。
 * @{
 */

#if XF_TASK_CONTEXT_IS_ENABLE

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/**
 * @brief ctask 类型值。
 * `XF_TASK_TYPE_ctask` 通过 `xf_task_reg.inc` 拼接而来。
 */
#define XF_TASK_TYPE_CTASK XF_TASK_TYPE_ctask

/* ==================== [Typedefs] ========================================== */

/**
 * @brief ctask 任务队列句柄。
 */
typedef void *xf_ctask_queue_t;

/**
 * @brief ctask 的传入参数。
 */
typedef struct _xf_ctask_config_t {
    size_t stack_size;      /*!< ctask 上下文堆栈大小 */
} xf_ctask_config_t;

/* ==================== [Global Prototypes] ================================= */

/**
 * @brief 在指定的任务管理器中创建 ctask。
 *
 * @param manager 指定的任务管理器。
 * @param func 任务执行的函数。
 * @param func_arg 用户自定义执行函数参数。
 * @param priority 任务优先级。
 * @param stack_size 任务上下文堆栈大小。
 * @return xf_task_t 任务对象，返回为 NULL 则表示创建失败
 */
static inline
xf_task_t xf_ctask_create_with_manager(
    xf_task_manager_t manager, xf_task_func_t func, void *func_arg,
    uint16_t priority, size_t stack_size)
{
    xf_ctask_config_t config = {.stack_size = stack_size};
    return xf_task_create_with_manager(manager, XF_TASK_TYPE_CTASK, func, func_arg, priority, &config);
}

/**
 * @brief ctask 专用 delay 函数，在 ctask 中才能使用。不会影响调度器。
 *
 * @param manager 任务管理器对象。
 * @param delay_ms ms 级别的延时，但是只能在 ctask 中使用。
 */
void xf_ctask_delay_with_manager(xf_task_manager_t manager, uint32_t delay_ms);

/**
 * @brief 创建 ctask 的消息队列。此消息队列仅供 ctask 使用。
 *
 * @param manager 任务管理器对象。
 * @param size 消息队列的大小。
 * @param count 消息队列的数量。
 * @return xf_ctask_queue_t 消息队列的对象
 */
xf_ctask_queue_t xf_ctask_queue_create_with_manager(
    xf_task_manager_t manager, const size_t size, const size_t count);

/**
 * @brief 消息队列发送。
 *
 * @param queue 消息队列对象。
 * @param buffer 消息队列发送的数据。
 * @param timeout 超时时间，规定时间内没发送成功则发送失败。
 * @return xf_err_t
 *      - XF_ERR_INVALID_ARG 参数错误
 *      - XF_ERR_BUSY 任务状态不符合执行条件
 *      - XF_ERR_NOT_SUPPORTED 任务类型不是ctask
 *      - XF_ERR_TIMEOUT 消息队列超时
 *      - XF_OK 消息队列正确发送
 */
xf_err_t xf_ctask_queue_send(xf_ctask_queue_t queue, void *buffer, uint32_t timeout);

/**
 * @brief 消息队列接收。
 *
 * @param queue 消息队列对象。
 * @param buffer 消息队列接收的数据。
 * @param timeout  超时时间，规定时间内没发送成功则接收失败。
 * @return xf_err_t
 *      - XF_ERR_INVALID_ARG 参数错误
 *      - XF_ERR_BUSY 任务状态不符合执行条件
 *      - XF_ERR_NOT_SUPPORTED 任务类型不是ctask
 *      - XF_ERR_TIMEOUT 消息队列超时
 *      - XF_OK 消息队列正确接收
 */
xf_err_t xf_ctask_queue_receive(xf_ctask_queue_t queue, void *buffer, uint32_t timeout);

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // XF_TASK_CONTEXT_IS_ENABLE

/**
 * End of group_xf_task_user_ctask
 * @}
 */

#endif // __XF_TASK_CONTEXT_H__
