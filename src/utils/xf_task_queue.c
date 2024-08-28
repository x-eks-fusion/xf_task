/**
 * @file xf_task_queue.c
 * @author cangyu (sky.kirto@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-03-21
 *
 * @copyright Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

/* ==================== [Includes] ========================================== */

#include "xf_task_queue.h"

/* ==================== [Defines] =========================================== */

#define TAG "queue"

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

static void copy_data_from_queue(xf_task_queue_t *const queue, void *const buffer);
static void copy_data_to_queue(xf_task_queue_t *const queue, const void *item, const bool pos);
static void move_reader(xf_task_queue_t *const queue);

/* ==================== [Static Variables] ================================== */

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

xf_err_t xf_task_queue_init(xf_task_queue_t *const queue, void *data, const size_t size, const size_t count)
{
    XF_ASSERT(queue, XF_ERR_INVALID_ARG, TAG, "queue must not be NULL");
    XF_ASSERT(data, XF_ERR_INVALID_ARG, TAG, "data must not be NULL");
    XF_ASSERT(size > 0, XF_ERR_INVALID_ARG, TAG, "size must more than 0");
    XF_ASSERT(count > 0, XF_ERR_INVALID_ARG, TAG, "count must more than 0");

    xf_bzero(queue, sizeof(xf_task_queue_t));

    queue->count = count;
    queue->size = size;
    queue->head = (uint8_t *)data;

    xf_err_t res = xf_task_queue_reset(queue);
    if (res != XF_OK)
    {
        XF_LOGE(TAG, "queue reset failed");
        return res;
    }
    

    return XF_OK;
}

xf_err_t xf_task_queue_reset(xf_task_queue_t *const queue)
{
    XF_ASSERT(queue, XF_ERR_INVALID_ARG, TAG, "queue must not be NULL");

    queue->tail = queue->head + (queue->count * queue->size);
    queue->waiting = 0;
    queue->writer = queue->head;
    queue->reader = queue->head + ((queue->count - 1) * queue->size);

    return XF_OK;
}

bool xf_task_queue_is_empty(const xf_task_queue_t *const queue)
{
    XF_ASSERT(queue, false, TAG, "queue must not be NULL");

    return (0 == queue->waiting) ? true : false;
}

size_t xf_task_queue_count(const xf_task_queue_t *const queue)
{
    XF_ASSERT(queue, 0, TAG, "queue must not be NULL");

    return queue->waiting;
}

void *xf_task_queue_peek(const xf_task_queue_t *const queue)
{
    XF_ASSERT(queue, NULL, TAG, "queue must not be NULL");

    uint8_t *ret_value = NULL;

    if (queue->waiting > 0) {
        ret_value = (uint8_t *)(queue->reader + queue->size);
        if (ret_value >= queue->tail) {
            ret_value = queue->head;
        }
    }

    return ret_value;
}

size_t xf_task_queue_available(const xf_task_queue_t *const queue)
{
    XF_ASSERT(queue, 0, TAG, "queue must not be NULL");

    return queue->count - queue->waiting;
}

bool xf_task_queue_is_full(const xf_task_queue_t *const queue)
{
    XF_ASSERT(queue, false, TAG, "queue must not be NULL");

    if (queue->waiting == queue->count) {
        return queue->waiting == queue->count;
    }

    return false;
}

xf_err_t xf_task_queue_send(xf_task_queue_t *const queue, void *item, const xf_task_queue_mode_t pos)
{
    XF_ASSERT(queue, XF_ERR_INVALID_ARG, TAG, "queue must not be NULL");
    XF_ASSERT(pos == 1 || pos == 0, XF_ERR_INVALID_ARG,
              TAG, "pos must be XF_TASK_QUEUE_SEND_TO_BACK or XF_TASK_QUEUE_SEND_TO_FRONT");

    // 检测没满，则复制进队列
    if (queue->waiting < queue->count) {
        copy_data_to_queue(queue, item, (bool)pos);
        return XF_OK;
    }
    return XF_ERR_BUSY;
}

xf_err_t xf_task_queue_remove_front(xf_task_queue_t *const queue)
{
    XF_ASSERT(queue, XF_ERR_INVALID_ARG, TAG, "queue must not be NULL");

    if (queue->waiting > 0) {
        move_reader(queue);
        queue->waiting--;
        return XF_OK;
    }
    return XF_ERR_BUSY;
}

xf_err_t xf_task_queue_receive(xf_task_queue_t *const queue, void *const buffer)
{
    XF_ASSERT(queue, XF_ERR_INVALID_ARG, TAG, "queue must not be NULL");
    if (queue->waiting > 0) {
        copy_data_from_queue(queue, buffer);
        queue->waiting--;
        return XF_OK;
    }
    return XF_ERR_BUSY;
}

/* ==================== [Static Functions] ================================== */

static void copy_data_from_queue(xf_task_queue_t *const queue, void *const buffer)
{
    move_reader(queue);
    xf_memcpy(buffer, queue->reader, queue->size);
}

static void copy_data_to_queue(xf_task_queue_t *const queue, const void *item, const bool pos)
{
    if (0 == pos) {
        xf_memcpy((void *)queue->writer, item, queue->size);
        queue->writer += queue->size;
        if (queue->writer >= queue->tail) {
            queue->writer = queue->head;
        }
    } else {
        xf_memcpy((void *)queue->reader, item, queue->size);
        queue->reader -= queue->size;
        if (queue->reader < queue->head) {
            queue->reader = queue->tail - queue->size;
        }
    }
    queue->waiting++;
}

static void move_reader(xf_task_queue_t *const queue)
{
    queue->reader += queue->size;
    if (queue->reader >= queue->tail) {
        queue->reader = queue->head;
    }
}
