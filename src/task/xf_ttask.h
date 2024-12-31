/**
 * @file xf_ttask.h
 * @author cangyu (sky.kirto@qq.com)
 * @brief 定时器任务。
 * @version 0.1
 * @date 2024-02-29
 *
 * @copyright Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

#ifndef __XF_TTASK_H__
#define __XF_TTASK_H__

/* ==================== [Includes] ========================================== */

#include "../kernel/xf_task_kernel.h"

/**
 * @ingroup group_xf_task_user
 * @defgroup group_xf_task_user_ttask ttask
 * @brief 定时器任务。
 * @{
 */

#ifdef __cplusplus
extern "C"
{
#endif

/* ==================== [Defines] =========================================== */

/**
 * @brief ttask 类型值。
 * `XF_TASK_TYPE_ttask` 通过 `xf_task_reg.inc` 拼接而来。
 */
#define XF_TASK_TYPE_TTASK XF_TASK_TYPE_ttask
#define XF_TTASK_INFINITE_LOOP ((uint32_t) - 1) /*!< ttask 无限循环 */

/* ==================== [Typedefs] ========================================== */

/**
 * @brief ttask 创建传输参数。
 */
typedef struct _xf_ttask_config_t {
    uint32_t count;    /*!< ttask 循环次数 */
    uint32_t delay_ms; /*!< ttask 循环间隔时间 */
} xf_ttask_config_t;

/* ==================== [Global Prototypes] ================================= */

/**
 * @brief 指定任务管理器创建 ttask。
 *
 * @param manager 任务管理器对象。
 * @param func 任务执行的函数。
 * @param func_arg 用户自定义执行函数参数。
 * @param priority 任务优先级。
 * @param delay_ms 任务延时周期。
 * @param count 任务循环的次数上限。
 * @return xf_task_t task 对象。返回为 NULL 则表示创建失败
 */
static inline xf_task_t xf_ttask_create_with_manager(
    xf_task_manager_t manager, xf_task_func_t func, void *func_arg,
    uint16_t priority, uint32_t delay_ms, uint32_t count)
{
    xf_ttask_config_t config = {.count = count, .delay_ms = delay_ms};
    return xf_task_create_with_manager(manager, XF_TASK_TYPE_TTASK, func, func_arg, priority, &config);
}

/**
 * @brief 指定任务管理器创建循环 ttask。
 *
 * @param manager 任务管理器对象。
 * @param func 任务执行的函数。
 * @param func_arg 用户自定义执行函数参数。
 * @param priority 任务优先级。
 * @param delay_ms 任务延时周期。
 * @return xf_task_t task 对象。返回为 NULL 则表示创建失败
 */
static inline xf_task_t xf_ttask_create_loop_with_manager(
    xf_task_manager_t manager, xf_task_func_t func,
    void *func_arg, uint16_t priority, uint32_t delay_ms)
{
    xf_ttask_config_t config = {.count = XF_TTASK_INFINITE_LOOP, .delay_ms = delay_ms};
    return xf_task_create_with_manager(manager, XF_TASK_TYPE_TTASK, func, func_arg, priority, &config);
}

/**
 * @brief 设置 ttask 循环次数。其不能超过循环次数的上限。
 *
 * @param task 任务对象。
 * @param count 循环次数。
 * @return xf_err_t
 *      - XF_ERR_INVALID_ARG 参数错误
 *      - XF_OK 设置成功
 */
xf_err_t xf_ttask_set_count(xf_task_t task, uint32_t count);

/**
 * @brief 获取 ttask 循环次数。
 *
 * @param task 任务对象。
 * @return uint32_t 循环次数
 */
uint32_t xf_ttask_get_count(xf_task_t task);

/**
 * @brief 设置 ttask 循环次数的上限。
 *
 * @param task 任务对象。
 * @param count_max 循环次数上限。
 * @return xf_err_t
 *      - XF_ERR_INVALID_ARG 参数错误
 *      - XF_OK 设置成功
 */
xf_err_t xf_ttask_set_count_max(xf_task_t task, uint32_t count_max);

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /* extern "C" */
#endif

/**
 * End of group_xf_task_user_ttask
 * @}
 */

#endif // __XF_TASK_NOCONTEXT_H__
