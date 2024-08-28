/**
 * @file xf_task_port_config.h
 * @author cangyu (sky.kirto@qq.com)
 * @brief xf_task 对接配置。
 * @version 0.1
 * @date 2024-07-04
 *
 * @copyright Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

#ifndef __XF_TASK_PORT_CONFIG_H__
#define __XF_TASK_PORT_CONFIG_H__

/* ==================== [Includes] ========================================== */

#include "../xf_task_config_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/**
 * @brief 如果开启上下文， 旧必须设置 XF_TASK_CONTEXT_TYPE 的类型。
 */
#if XF_TASK_CONTEXT_IS_ENABLE
typedef XF_TASK_CONTEXT_TYPE xf_task_context_t;
#   ifndef XF_TASK_CONTEXT_TYPE
#       error "The context object does not give the exact type!"
#   endif // XF_TASK_CONTEXT_TYPE
#endif // XF_TASK_CONTEXT_IS_ENABLE

/* ==================== [Typedefs] ========================================== */

/* ==================== [Global Prototypes] ================================= */

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __XF_TASK_PORT_CONFIG_H__
