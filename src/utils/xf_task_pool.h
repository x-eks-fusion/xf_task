/**
 * @file xf_task_pool.h
 * @author cangyu (sky.kirto@qq.com)
 * @brief 任务池。
 * @version 0.1
 * @date 2024-07-08
 *
 * @copyright Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

#ifndef __XF_TASK_POOL_H__
#define __XF_TASK_POOL_H__

/* ==================== [Includes] ========================================== */

#include "xf_task_utils_config.h"

#if XF_TASK_POOL_IS_ENABLE

#include "../kernel/xf_task_kernel.h"

/**
 * @ingroup group_xf_task_user
 * @defgroup group_xf_task_user_task_pool task_pool
 * @brief 任务池。
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/**
 * @brief 任务池句柄。
 */
typedef void *xf_task_pool_t;

/* ==================== [Global Prototypes] ================================= */

/**
 * @brief 创建一个任务池。
 *
 * @param max_works 任务池最大任务数。
 * @param manager 任务管理器。
 * @param type 任务类型。
 * @param config 任务配置。
 * @return xf_task_pool_t 任务池对象，如果创建失败则返回 NULL
 */
xf_task_pool_t xf_task_pool_create_with_manager(uint32_t max_works, xf_task_manager_t manager, xf_task_type_t type,
        void *config);

/**
 * @brief 删除任务池。
 *
 * @param pool 任务池对象。
 * @return xf_err_t
 *      - XF_ERR_INVALID_ARG 参数错误
 *      - XF_OK 任务池删除成功
 */
xf_err_t xf_task_pool_delete(xf_task_pool_t pool);

/**
 * @brief 初始化任务。
 *
 * @param pool 任务池对象。
 * @param func 任务执行的函数。
 * @param func_arg 用户自定义执行函数参数。
 * @param priority 任务优先级。
 * @return xf_task_t 任务对象
 */
xf_task_t xf_task_init_from_pool(xf_task_pool_t pool, xf_task_func_t func, void *func_arg, uint16_t priority);

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /* extern "C" */
#endif

/**
 * End of group_xf_task_user_task_pool
 * @}
 */

#endif // XF_TASK_POOL_IS_ENABLE

#endif // __XF_TASK_POOL_H__
