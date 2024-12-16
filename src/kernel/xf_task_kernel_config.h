/**
 * @file xf_task_kernel_config.h
 * @author cangyu (sky.kirto@qq.com)
 * @brief xf_task 内核配置(仅 xf_task kernel 内部使用)。
 * @version 0.1
 * @date 2024-07-04
 *
 * @copyright Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

#ifndef __XF_TASK_KERNEL_CONFIG_H__
#define __XF_TASK_KERNEL_CONFIG_H__

/* ==================== [Includes] ========================================== */

#include "../xf_task_config_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/**
 * @brief 配置任务最高优先级，1 ~ 1024 之间。
 */
#ifndef XF_TASK_PRIORITY_LEVELS
#   define XF_TASK_PRIORITY_LEVELS  24
#endif

#if XF_TASK_PRIORITY_LEVELS < 1
#   error "At least one priority is required"
#endif

#if XF_TASK_PRIORITY_LEVELS > 1024
#   error "The priority should not be greater than 1024"
#endif

/**
 * @brief 配置是否启用饥饿值功能。
 */
#if !defined(XF_TASK_HUNGER_ENABLE) || (XF_TASK_HUNGER_ENABLE)
#   define XF_TASK_HUNGER_IS_ENABLE (1)
#else
#   define XF_TASK_HUNGER_IS_ENABLE (0)
#endif

/**
 * @brief 配置是否使用任务用户参数。
 */
#if !defined(XF_TASK_USER_DATA_ENABLE) || (XF_TASK_USER_DATA_ENABLE)
#   define XF_TASK_USER_DATA_IS_ENABLE  (1)
#else
#   define XF_TASK_USER_DATA_IS_ENABLE  (0)
#endif

/* ==================== [Typedefs] ========================================== */

/* ==================== [Global Prototypes] ================================= */

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __XF_TASK_KERNEL_CONFIG_H__
