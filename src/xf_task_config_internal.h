/**
 * @file xf_task_config_internal.h
 * @author cangyu (sky.kirto@qq.com)
 * @brief xf_task 模块内部配置总头文件。
 *        确保 xf_hal_config.h 的所有定义都有默认值。
 * @version 0.1
 * @date 2024-07-04
 *
 * @copyright Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

#ifndef __XF_TASK_CONFIG_INTERNAL_H__
#define __XF_TASK_CONFIG_INTERNAL_H__

/* ==================== [Includes] ========================================== */

#include "xf_task_config.h"
#include "xf_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/**
 * @brief 是否打开 ctask 功能。
 */
#if !defined(XF_TASK_CONTEXT_DISABLE) || (XF_TASK_CONTEXT_DISABLE)
#   define XF_TASK_CONTEXT_IS_ENABLE (0)
#else
#   define XF_TASK_CONTEXT_IS_ENABLE (1)
#endif

/**
 * @brief 设置 xf_task 时间戳类型宏。
 */
#ifndef XF_TASK_TIME_TYPE
#   define XF_TASK_TIME_TYPE uint64_t
#endif

/* ==================== [Typedefs] ========================================== */

/**
 * @brief xf_task 时间戳类型。
 */
typedef XF_TASK_TIME_TYPE xf_task_time_t;

/* ==================== [Global Prototypes] ================================= */

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __XF_TASK_CONFIG_INTERNAL_H__
