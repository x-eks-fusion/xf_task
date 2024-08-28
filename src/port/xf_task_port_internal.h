/**
 * @file xf_task_port_internal.h
 * @author cangyu (sky.kirto@qq.com)
 * @brief xf_task 对接内部配置。
 * @version 0.1
 * @date 2024-03-05
 *
 * @copyright Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

#ifndef __XF_TASK_PORT_INTERNAL_H__
#define __XF_TASK_PORT_INTERNAL_H__

/* ==================== [Includes] ========================================== */

#include "xf_task_port.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Global Prototypes] ================================= */

xf_task_time_t xf_task_get_ticks(void);

#if XF_TASK_CONTEXT_IS_ENABLE
void xf_task_context_create(xf_task_manager_t manager, xf_context_func_t context_entry, void *context, void *stack,
                            size_t stack_size);
void xf_task_context_swap(xf_task_manager_t manager, void *old_context, void *new_context);
xf_task_context_t *xf_task_manager_get_context(xf_task_manager_t manager);
#endif // XF_TASK_CONTEXT_IS_ENABLE

int32_t xf_task_msec_to_ticks(int32_t msec);
int32_t xf_task_ticks_to_msec(int32_t ticks);
int32_t xf_task_sec_to_ticks(int32_t sec);
int32_t xf_task_ticks_to_sec(int32_t ticks);

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __XF_TASK_PORT_INTERNAL_H__
