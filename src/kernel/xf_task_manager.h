/**
 * @file xf_task_manager.h
 * @author cangyu (sky.kirto@qq.com)
 * @brief 任务管理器。
 * @version 0.1
 * @date 2024-02-29
 *
 * @copyright Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

#ifndef __XF_TASK_MANAGER_H__
#define __XF_TASK_MANAGER_H__

/* ==================== [Includes] ========================================== */

#include "xf_task_kernel_config.h"
#include "xf_utils.h"

/**
 * @ingroup group_xf_task_user
 * @defgroup group_xf_task_user_manager manager
 * @brief xf_task 任务管理器。
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/**
 * @brief 任务句柄。
 */
typedef void *xf_task_t;

/**
 * @brief 任务管理器句柄。
 */
typedef void *xf_task_manager_t;

/**
 * @brief 空闲任务回调函数原型。
 * 
 * @param max_idle_ms 空闲(睡眠)时间。
 */
typedef void (*xf_task_on_idle_t)(unsigned long int max_idle_ms);

/* ==================== [Global Prototypes] ================================= */

/**
 * @brief 创建任务管理器。
 *
 * @param on_idle 空闲回调函数。
 * @return xf_task_manager_t 任务管理器对象
 */
xf_task_manager_t xf_task_manager_create(xf_task_on_idle_t on_idle);

/**
 * @brief 开始启动任务管理器调度任务。
 *
 * @param manager 任务管理器对象。
 */
void xf_task_manager_run(xf_task_manager_t manager);

/**
 * @brief 将任务设置为紧急任务，下次调度立即执行。
 *
 * @param manager 任务管理器对象。
 * @param task 设置为紧急任务的任务。
 * @param force 如果设置为 true，则覆盖任务。设置为 false，当前有紧急任务则设置失败。
 * @return xf_err_t
 *      - XF_OK 设置成功
 *      - XF_ERR_INVALID_ARG 无效参数
 *      - XF_ERR_BUSY 设置失败，`force == false` 且当前已存在紧急任务
 */
xf_err_t xf_task_set_urgent_task_with_manager(xf_task_manager_t manager, xf_task_t task, bool force);

/**
 * @brief 获取管理器运行的任务。
 *
 * @param manager 任务管理器对象。
 * @return xf_task_t 正在运行的任务对象，返回 NULL 则表示获取失败
 */
xf_task_t xf_task_manager_get_current_task(xf_task_manager_t manager);

/**
 * @brief 设置当前任务为就绪。
 *
 * @param manager 任务管理器对象。
 * @param task 任务对象。
 * @return xf_err_t
 *      - XF_ERR_INVALID_ARG 参数错误
 *      - XF_OK 设置任务就绪成功
 */
xf_err_t xf_task_manager_task_ready(xf_task_manager_t manager, xf_task_t task);

/**
 * @brief 设置当前任务为挂起。
 *
 * @param manager 任务管理器对象。
 * @param task 任务对象。
 * @return xf_err_t
 *      - XF_ERR_INVALID_ARG 参数错误
 *      - XF_OK 设置任务挂起成功
 */
xf_err_t xf_task_manager_task_suspend(xf_task_manager_t manager, xf_task_t task);

/**
 * @brief 设置当前任务为销毁。
 *
 * @param manager 任务管理器对象。
 * @param task 任务对象。
 * @return xf_err_t
 *      - XF_ERR_INVALID_ARG 参数错误
 *      - XF_OK 设置任务销毁成功
 */
xf_err_t xf_task_manager_task_destory(xf_task_manager_t manager, xf_task_t task);

/**
 * @brief 设置当前任务为阻塞。
 *
 * @param manager 任务管理器对象。
 * @param task 任务对象。
 * @return xf_err_t
 *      - XF_ERR_INVALID_ARG 参数错误
 *      - XF_OK 设置任务阻塞成功
 */
xf_err_t xf_task_manager_task_blocked(xf_task_manager_t manager, xf_task_t task);

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /* extern "C" */
#endif

/**
 * End of group_xf_task_user_manager
 * @}
 */

#endif // __XF_TASK_MANAGER_H__
