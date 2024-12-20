/**
 * @file xf_task_port.h
 * @author cangyu (sky.kirto@qq.com)
 * @brief xf_task 对接总头文件。
 * @version 0.1
 * @date 2024-03-05
 *
 * @copyright Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

#ifndef __XF_TASK_PORT_H__
#define __XF_TASK_PORT_H__

/* ==================== [Includes] ========================================== */

#include "xf_task_port_config.h"
#include "xf_utils.h"

/**
 * @ingroup group_xf_task_port
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/**
 * @brief 任务管理器句柄。
 */
typedef void *xf_task_manager_t;

/**
 * @brief 上下文切换函数原型。
 *
 * @param arg 通常为任务管理器句柄。
 */
typedef void (*xf_context_func_t)(void *arg);

/**
 * @brief task 时钟对接，为调度器提供心跳。
 *
 * @return 返回时钟的时间戳
 */
typedef xf_task_time_t (*xf_task_clock_t)(void);

#if XF_TASK_CONTEXT_IS_ENABLE

/**
 * @brief 创建上下文的函数指针。
 *
 * @param manager 当前任务管理器（用于获取上下文句柄）。
 * @param context_entry 上下文。
 * @param context 上下文函数。
 * @param stack 上下文堆栈。
 * @param stack_size 上下文堆栈大小。
 */
typedef void (*xf_task_create_context_t)(xf_task_manager_t manager, xf_context_func_t context_entry, void *context,
        void *stack, size_t stack_size);

/**
 * @brief 切换上席文的函数指针。
 *
 * @param manager 当前任务管理器（用于获取上下文句柄）。
 * @param old_context 当前上下文。
 * @param new_context 要切换到的上下文。
 */
typedef void (*xf_task_swap_context_t)(xf_task_manager_t manager, void *old_context, void *new_context);

#endif // XF_TASK_CONTEXT_IS_ENABLE

/* ==================== [Global Prototypes] ================================= */

/**
 * @brief 设置当前任务的心跳。
 *
 * @param clock_per_sec 一秒钟的滴答数。
 * @param clock 获取心跳的数值。
 * @return xf_err_t
 *      - XF_ERR_INVALID_ARG 参数错误
 *      - XF_OK 参数设置成功
 */
xf_err_t xf_task_tick_init(xf_task_clock_t clock);

#if XF_TASK_CONTEXT_IS_ENABLE
/**
 * @brief 对接保存任务上下文函数。
 *
 * @param create_context 创建保存上下文。
 * @param swap_context 切换上下文。
 * @return xf_err_t
 *      - XF_ERR_INVALID_ARG 参数错误
 *      - XF_OK 参数设置成功
 */
xf_err_t xf_task_context_init(xf_task_create_context_t create_context, xf_task_swap_context_t swap_context);
#endif // XF_TASK_CONTEXT_IS_ENABLE

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /* extern "C" */
#endif

/**
 * End of group_xf_task_port
 * @}
 */

#endif // __XF_TASK_PORT_H__

