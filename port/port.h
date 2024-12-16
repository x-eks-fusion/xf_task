/**
 * @file port.h
 * @author cangyu (sky.kirto@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-03-05
 *
 * @copyright Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

#ifndef __PORT_H__
#define __PORT_H__

/* ==================== [Includes] ========================================== */
#include "xf_task.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

#ifndef USE_GNU_UC
#define USE_GNU_UC 0
#endif

/* ==================== [Typedefs] ========================================== */

/* ==================== [Global Prototypes] ================================= */

#if !XF_TASK_CONTEXT_DISABLE

void create_context(xf_task_manager_t manager, xf_context_func_t context_entry, void *context, void *stack,
                    size_t stack_size);
void swap_context(xf_task_manager_t manager, void *old_context, void *new_context);

#endif // XF_TASK_CONTEXT_DISABLE

xf_task_time_t task_get_tick(void);
void task_on_idle(unsigned long int max_idle_ms);

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __PORT_H__
