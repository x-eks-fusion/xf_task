/**
 * @file xf_task_event.c
 * @author cangyu (sky.kirto@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-04-03
 *
 * @copyright Copyright (c) 2025, CorAL. All rights reserved.
 *
 */

/* ==================== [Includes] ========================================== */

#include "xf_task_event.h"
#include "../xf_task.h"

/* ==================== [Defines] =========================================== */

#define TAG "mbus"

/* ==================== [Typedefs] ========================================== */

typedef struct _xf_task_event_sub_t {
    xf_list_t node;
    xf_task_t task;
    uint32_t event;
    xf_task_event_mode_t mode;
} xf_task_event_sub_t;

/* ==================== [Static Prototypes] ================================= */

/* ==================== [Static Variables] ================================== */

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

xf_err_t xf_task_event_reg(xf_task_event_t *event, xf_task_t task, uint32_t event_value,
                           xf_task_event_mode_t mode)
{
    XF_ASSERT(event, XF_ERR_INVALID_ARG, TAG, "event must not be NULL");
    xf_task_event_sub_t *sub = (xf_task_event_sub_t *)malloc(sizeof(xf_task_event_sub_t));
    if (sub == NULL) {
        XF_LOGE(TAG, "memory alloc failed!");
        return NULL;
    }
    sub->task = task;
    sub->event = event_value;
    sub->mode = mode;
    xf_list_init(&sub->node);
    xf_list_add(&sub->node, &event->task_list);
    return XF_OK;
}

xf_err_t xf_task_event_unreg(xf_task_event_t *event, xf_task_t task)
{
    XF_ASSERT(event, XF_ERR_INVALID_ARG, TAG, "event must not be NULL");
    xf_task_event_sub_t *sub = NULL, *tmp = NULL;
    xf_list_for_each_entry_safe(sub, tmp, &event->task_list, xf_task_event_sub_t, node) {
        if (sub->task == task) {
            xf_list_del_init(&sub->node);
            xf_free(sub);
            return XF_OK;
        }
    }
    return XF_ERR_NOT_FOUND;
}


xf_err_t xf_task_event_sent(xf_task_event_t *event, uint32_t event_value)
{
    XF_ASSERT(event, XF_ERR_INVALID_ARG, TAG, "event must not be NULL");

    xf_task_event_sub_t *sub = NULL;
    xf_list_for_each_entry(sub, &event->task_list, xf_task_event_sub_t, node) {
        switch (sub->mode) {
        case XF_TASK_EVENT_OR:
            if ((sub->event | event_value) == sub->event) {
                xf_task_trigger(sub->task);
            }
            break;

        case XF_TASK_EVENT_AND:
            if ((sub->event & event_value) == sub->event) {
                xf_task_trigger(sub->task);
            }
            break;

        default:
            break;
        }
    }
    return XF_OK;
}

/* ==================== [Static Functions] ================================== */



