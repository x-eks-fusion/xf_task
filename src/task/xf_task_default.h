/**
 * @file xf_task_default.h
 * @author cangyu (sky.kirto@qq.com)
 * @brief 默认任务管理器。用于简化调用。
 * @version 0.1
 * @date 2024-06-21
 *
 * @copyright Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

#ifndef __XF_TASK_DEFAULT_H__
#define __XF_TASK_DEFAULT_H__

/* ==================== [Includes] ========================================== */

#include "../kernel/xf_task_kernel.h"

/**
 * @ingroup group_xf_task_user
 * @defgroup group_xf_task_user_task_manager_default task_manager_default
 * @brief 用于简化调用的默认任务管理器。
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Global Prototypes] ================================= */

/**
 * @brief 创建默认的任务管理器。
 *
 * @param on_idle 空闲回调函数。
 * @return xf_err_t
 *      - XF_FAIL 参数错误
 *      - XF_OK 设置任务就绪成功
 */
xf_err_t xf_task_manager_default_init(xf_task_on_idle_t on_idle);

/**
 * @brief 获取默认的任务管理器。
 *
 * @return xf_task_manager_t 任务管理器对象
 */
xf_task_manager_t xf_task_get_default_manager(void);

/**
 * @brief 开始默认启动任务管理器调度任务。
 */
void xf_task_manager_run_default(void);

/**
 * @brief 基于默认 manager，将任务设置为紧急任务，下次调度立即执行。
 *
 * @param task 设置为紧急任务的任务。
 * @param force 如果设置为 true，则覆盖任务。设置为 false，当前有紧急任务则设置失败。
 * @return xf_err_t
 *      - XF_OK 设置成功
 *      - XF_ERR_INVALID_ARG 无效参数
 *      - XF_ERR_BUSY 设置失败，`force == false` 且当前已存在紧急任务
 */
xf_err_t xf_task_set_urgent_task(xf_task_t task, bool force);

/**
 * @brief 基于默认 manager，创建任务。
 *
 * @param type 任务类型，XF_TASK_TYPE_CTASK 和 XF_TASK_TYPE_NTASK。
 * @param func 任务入口函数。
 * @param func_arg 任务参数。
 * @param priority 任务优先级。
 * @param config 对应任务类型的配置。
 * @return xf_task_t 任务对象
 */
xf_task_t xf_task_create(xf_task_type_t type, xf_task_func_t func, void *func_arg, uint16_t priority, void *config);

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /* extern "C" */
#endif

/**
 * End of group_xf_task_user_task_manager_default
 * @}
 */

#endif // __XF_TASK_DEFAULT_H__
