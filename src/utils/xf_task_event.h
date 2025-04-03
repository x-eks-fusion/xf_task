/**
 * @file xf_task_event.h
 * @author cangyu (sky.kirto@qq.com)
 * @brief 
 * @version 0.1
 * @date 2025-04-03
 * 
 * @copyright Copyright (c) 2025, CorAL. All rights reserved.
 * 
 */

#ifndef __XF_TASK_EVENT_H__
#define __XF_TASK_EVENT_H__

/* ==================== [Includes] ========================================== */

#include "xf_task_utils_config.h"

#include "../kernel/xf_task_kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

typedef struct _xf_task_event_t {
    xf_list_t task_list;
} xf_task_event_t;

typedef enum _xf_task_event_mode_t {
    XF_TASK_EVENT_OR,
    XF_TASK_EVENT_AND,
} xf_task_event_mode_t;

/* ==================== [Global Prototypes] ================================= */

xf_err_t xf_task_event_reg(xf_task_event_t *event, xf_task_t task, uint32_t event_value,
    xf_task_event_mode_t mode);
xf_err_t xf_task_event_unreg(xf_task_event_t *event, xf_task_t task);
xf_err_t xf_task_event_sent(xf_task_event_t *event, uint32_t event_value);



/* ==================== [Macros] ============================================ */

#define xf_event_init(self) {\
    .task_list = XF_LIST_HEAD_INIT(self.task_list),\
}

#if XF_TASK_CONTEXT_IS_ENABLE

#define xf_ctask_event_wait_or(event, task, event_value, timeout) \
    xf_task_event_reg(&event, task, event_value, XF_TASK_EVENT_OR);\
    xf_ctask_delay(timeout);\
    xf_task_event_unreg(&event, task);

#define xf_ctask_event_wait_and(event, task, event_value, timeout) \
    xf_task_event_reg(&event, task, event_value, XF_TASK_EVENT_AND);\
    xf_ctask_delay(timeout);\
    xf_task_event_unreg(&event, task);


#endif

#define xf_ntask_event_wait_or(event, task, event_value, timeout) \
    xf_task_event_reg(&event, task, event_value, XF_TASK_EVENT_OR);\
    xf_ntask_delay(timeout);\
    xf_task_event_unreg(&event, task);

#define xf_ntask_event_wait_and(event, task, event_value, timeout) \
    xf_task_event_reg(&event, task, event_value, XF_TASK_EVENT_AND);\
    xf_ntask_delay(timeout);\
    xf_task_event_unreg(&event, task);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __XF_TASK_EVENT_H__
