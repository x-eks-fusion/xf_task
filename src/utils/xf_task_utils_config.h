/**
 * @file xf_task_utils_config.h
 * @author cangyu (sky.kirto@qq.com)
 * @brief xf_task_utils 配置。
 * @version 0.1
 * @date 2024-07-05
 *
 * @copyright Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

#ifndef __XF_TASK_UTILS_CONFIG_H__
#define __XF_TASK_UTILS_CONFIG_H__

/* ==================== [Includes] ========================================== */

#include "../xf_task_config_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/**
 * @brief 是否打开 MBUS 功能。
 */
#if !defined(XF_TASK_MBUS_ENABLE) || (XF_TASK_MBUS_ENABLE)
#   define XF_TASK_MBUS_IS_ENABLE (1)
#else
#   define XF_TASK_MBUS_IS_ENABLE (0)
#endif

/**
 * @brief 是否打开任务池功能。
 */
#if !defined(XF_TASK_POOL_ENABLE) || (XF_TASK_POOL_ENABLE)
#   define XF_TASK_POOL_IS_ENABLE (1)
#else
#   define XF_TASK_POOL_IS_ENABLE (0)
#endif

/* ==================== [Typedefs] ========================================== */

/* ==================== [Global Prototypes] ================================= */

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __XF_TASK_UTILS_CONFIG_H__
