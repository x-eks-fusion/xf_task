/**
 * @file xf_task_mbus.c
 * @author cangyu (sky.kirto@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-04-08
 *
 * @copyright Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

/* ==================== [Includes] ========================================== */

#include "xf_task_mbus.h"
#include "xf_task_queue.h"

/* ==================== [Defines] =========================================== */

#if XF_TASK_MBUS_IS_ENABLE

/* ==================== [Typedefs] ========================================== */

typedef struct _xf_task_xtopic_t {
    xf_list_t node;
    xf_list_t sub_list;         // 订阅链表
    xf_task_queue_t pub_queue;  // 发布链表，有缓存有限用缓存，没缓存则创建
    uint32_t id;                // topic id
    uint32_t size;              // topic发布消息大小
} xf_task_mtopic_t;

typedef struct _xf_task_xsub_t {
    xf_list_t node;
    xf_task_mbus_func_t mbus_cb; // 订阅回调
    void *user_data;             // 用户订阅回调参数
} xf_task_msub_t;

/* ==================== [Static Prototypes] ================================= */

static void xf_task_mbus_run(xf_task_mtopic_t *mtopic, void *data);
static xf_err_t xf_task_mbus_find(uint32_t topic_id, xf_task_mtopic_t **topic);

/* ==================== [Static Variables] ================================== */

static xf_list_t _topic_list = XF_LIST_HEAD_INIT(_topic_list);

/* ==================== [Macros] ============================================ */

#define TAG "mbus"
#define DEFAULT_QUEUE_COUNT (2)

/* ==================== [Global Functions] ================================== */


xf_err_t xf_task_mbus_reg_topic(uint32_t topic_id, uint32_t size)
{
    XF_ASSERT(xf_task_mbus_find(topic_id, NULL), XF_ERR_INITED, TAG, "topic:%d is exists", (int)topic_id);

    // 找到指定的manager后，注册mtopic
    xf_task_mtopic_t *mtopic = (xf_task_mtopic_t *)xf_malloc(sizeof(xf_task_mtopic_t) + DEFAULT_QUEUE_COUNT * size);
    void *buf = (void *)((uint8_t *)mtopic + sizeof(xf_task_mtopic_t));

    if (mtopic == NULL) {
        XF_LOGE(TAG, "memory alloc failed!");
        return XF_ERR_NO_MEM;
    }

    xf_list_init(&mtopic->node);
    xf_list_init(&mtopic->sub_list);
    xf_task_queue_init(&mtopic->pub_queue, buf, size, DEFAULT_QUEUE_COUNT);
    mtopic->id = topic_id;
    mtopic->size = size;
    xf_list_add_tail(&mtopic->node, &_topic_list);

    return XF_OK;
}


xf_err_t xf_task_mbus_unreg_topic(uint32_t topic_id)
{
    xf_task_mtopic_t *mtopic = NULL;
    xf_task_msub_t *msub, *_msub;

    if (xf_task_mbus_find(topic_id, &mtopic) == XF_ERR_NOT_FOUND) {
        XF_LOGE(TAG, "topic:%d not found", (int)topic_id);
        return XF_ERR_NOT_FOUND;
    }

    xf_list_for_each_entry_safe(msub, _msub, &mtopic->sub_list, xf_task_msub_t, node) {
        xf_list_del_init(&msub->node);
        xf_free(msub);
    }
    xf_list_del_init(&mtopic->node);
    xf_free(mtopic);

    return XF_OK;
}


xf_err_t xf_task_mbus_pub_async(uint32_t topic_id, void *data)
{
    XF_ASSERT(data, XF_ERR_INVALID_ARG, TAG, "data must not be NULL");

    xf_task_mtopic_t *mtopic = NULL;

    if (xf_task_mbus_find(topic_id, &mtopic) == XF_ERR_NOT_FOUND) {
        XF_LOGE(TAG, "topic:%d not found", (int)topic_id);
        return XF_ERR_NOT_FOUND;
    }

    xf_err_t err = xf_task_queue_send(&mtopic->pub_queue, data, XF_TASK_QUEUE_SEND_TO_BACK);
    return err;
}


xf_err_t xf_task_mbus_pub_sync(uint32_t topic_id, void *data)
{
    XF_ASSERT(data, XF_ERR_INVALID_ARG, TAG, "data must not be NULL");

    xf_task_mtopic_t *mtopic = NULL;

    if (xf_task_mbus_find(topic_id, &mtopic) == XF_ERR_NOT_FOUND) {
        XF_LOGE(TAG, "topic:%d not found", (int)topic_id);
        return XF_ERR_NOT_FOUND;
    }

    xf_task_mbus_run(mtopic, data);

    return XF_OK;
}

xf_err_t xf_task_mbus_sub(uint32_t topic_id, xf_task_mbus_func_t mbus_cb, void *user_data)
{
    XF_ASSERT(mbus_cb, XF_ERR_INVALID_ARG, TAG, "mbus_cb must not be NULL");

    xf_task_mtopic_t *mtopic = NULL;
    xf_task_msub_t *msub = NULL;

    if (xf_task_mbus_find(topic_id, &mtopic) == XF_ERR_NOT_FOUND) {
        XF_LOGE(TAG, "topic:%d not found", (int)topic_id);
        return XF_ERR_NOT_FOUND;
    }

    xf_list_for_each_entry(msub, &mtopic->sub_list, xf_task_msub_t, node) {
        if (msub->mbus_cb == mbus_cb) {
            XF_LOGD(TAG, "mbus_cb is exists!");
            return XF_ERR_INITED;
        }
    }

    // 如果没有重复注册，则创建并加入链表
    msub = (xf_task_msub_t *)xf_malloc(sizeof(xf_task_msub_t));

    if (msub == NULL)
    {
        XF_LOGE(TAG, "memory alloc failed!");
        return XF_ERR_NO_MEM;
    }

    xf_list_init(&msub->node);

    msub->mbus_cb = mbus_cb;
    msub->user_data = user_data;

    xf_list_add_tail(&msub->node, &mtopic->sub_list);

    return XF_OK;
}

xf_err_t xf_task_mbus_unsub(uint32_t topic_id, xf_task_mbus_func_t mbus_cb)
{
    XF_ASSERT(mbus_cb, XF_ERR_INVALID_ARG, TAG, "mbus_cb must not be NULL");

    xf_task_mtopic_t *mtopic = NULL;
    xf_task_msub_t *msub, *_msub;

    if (xf_task_mbus_find(topic_id, &mtopic) == XF_ERR_NOT_FOUND) {
        XF_LOGE(TAG, "topic:%d not found", (int)topic_id);
        return XF_ERR_NOT_FOUND;
    }

    xf_list_for_each_entry_safe(msub, _msub, &mtopic->sub_list, xf_task_msub_t, node) {
        if (msub->mbus_cb == mbus_cb) {
            xf_list_del_init(&msub->node);
            xf_free(msub);
            return XF_OK;
        }
    }

    XF_LOGE(TAG, "mbus_cb not found!");

    return XF_ERR_NOT_FOUND;
}

xf_err_t xf_task_mbus_unsub_all(uint32_t topic_id)
{
    xf_task_mtopic_t *mtopic = NULL;
    xf_task_msub_t *msub, *_msub;

    if (xf_task_mbus_find(topic_id, &mtopic) == XF_ERR_NOT_FOUND) {
        XF_LOGE(TAG, "topic:%d not found", (int)topic_id);
        return XF_ERR_NOT_FOUND;
    }

    xf_list_for_each_entry_safe(msub, _msub, &mtopic->sub_list, xf_task_msub_t, node) {
        xf_list_del_init(&msub->node);
        xf_free(msub);
    }

    return XF_OK;
}

void xf_task_mbus_handle(void)
{
    // 循环执行订阅回调
    xf_task_mtopic_t *mtopic;
    xf_list_for_each_entry(mtopic, &_topic_list, xf_task_mtopic_t, node) {
        while (!xf_task_queue_is_empty(&mtopic->pub_queue)) {
            void *pub_data = xf_task_queue_peek(&mtopic->pub_queue);
            xf_task_mbus_run(mtopic, pub_data);
            xf_task_queue_remove_front(&mtopic->pub_queue);
        }
    }
}

/* ==================== [Static Functions] ================================== */

static void xf_task_mbus_run(xf_task_mtopic_t *mtopic, void *data)
{
    xf_task_msub_t *msub;
    xf_list_for_each_entry(msub, &mtopic->sub_list, xf_task_msub_t, node) {
        msub->mbus_cb(data, msub->user_data);
    }
}

static xf_err_t xf_task_mbus_find(uint32_t topic_id, xf_task_mtopic_t **topic)
{
    xf_task_mtopic_t *mtopic;
    xf_list_for_each_entry(mtopic, &_topic_list, xf_task_mtopic_t, node) {
        if (mtopic->id == topic_id) {
            if (topic != NULL) {
                *topic = mtopic;
            }
            return XF_OK;
        }
    }

    return XF_ERR_NOT_FOUND;
}

#endif // XF_TASK_MBUS_IS_ENABLE
