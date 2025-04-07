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
    xf_list_t event_list; //
} xf_task_event_t;

typedef enum _xf_task_event_mode_t {
    XF_TASK_EVENT_OR,
    XF_TASK_EVENT_AND,
} xf_task_event_mode_t;

/* ==================== [Global Prototypes] ================================= */

/**
 * @brief 将等待的事件注册到事件队列进行等待，一个任务在一个事件对象上只能同时注册一次
 *
 * @param event 事件对象，由外界声明结构体并传入
 * @param task 事件所阻塞的任务
 * @param event_value 事件等待的值，一个位代表一个事件
 * @param mode 等待模式，有或和与，是多事件触发时的等待的逻辑
 * @return xf_err_t 返回XF_OK表示注册成功
 */
xf_err_t xf_task_event_reg(xf_task_event_t *event, xf_task_t task, uint32_t event_value,
                           xf_task_event_mode_t mode);

/**
 * @brief 取消事件注册
 *
 * @param event 事件对象
 * @param task 事件所归属的任务
 * @return xf_err_t 返回XF_OK表示注册成功
 */
xf_err_t xf_task_event_unreg(xf_task_event_t *event, xf_task_t task);

/**
 * @brief 发送事件。激活等待事件的任务
 * 
 * @param event 事件对象
 * @param event_value 发送事件的值，可以是多个不同事件，通过|进行组合
 * @return xf_err_t 返回XF_OK表示发送成功 
 */
xf_err_t xf_task_event_send(xf_task_event_t *event, uint32_t event_value);

/* ==================== [Macros] ============================================ */

#define xf_event_init(self) {\
    .event_list = XF_LIST_HEAD_INIT(self.event_list),\
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
    xf_err_t _err = xf_task_event_reg(&event, task, event_value, XF_TASK_EVENT_OR);\
    if (_err == XF_OK) {\
        xf_ntask_delay(timeout);\
        xf_task_event_unreg(&event, task);\
    }



#define xf_ntask_event_wait_and(event, task, event_value, timeout) \
    xf_err_t _err = xf_task_event_reg(&event, task, event_value, XF_TASK_EVENT_AND);\
    if (_err == XF_OK) {\
        xf_ntask_delay(timeout);\
        xf_task_event_unreg(&event, task);\
    }

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __XF_TASK_EVENT_H__
