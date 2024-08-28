/**
 * @file xf_task_queue.h
 * @author cangyu (sky.kirto@qq.com)
 * @brief 消息队列。
 * @version 0.1
 * @date 2024-03-21
 *
 * @copyright Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

#ifndef __XF_TASK_QUEUE_H__
#define __XF_TASK_QUEUE_H__

/* ==================== [Includes] ========================================== */

#include "xf_utils.h"

/**
 * @ingroup group_xf_task_user
 * @defgroup group_xf_task_user_task_queue task_queue
 * @brief 队列。
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */


/* ==================== [Typedefs] ========================================== */

/**
 * @brief 消息队列对象结构体。
 */
typedef struct _xf_task_queue_t {
    uint8_t *head;
    uint8_t *tail;
    uint8_t *writer;
    uint8_t *reader;
    volatile size_t waiting;
    size_t count;
    size_t size;
} xf_task_queue_t;

/**
 * @brief 消息队列传输模式枚举。
 */
typedef enum _xf_task_queue_mode_t {
    XF_TASK_QUEUE_SEND_TO_BACK, /*!< 从后往前发送 */
    XF_TASK_QUEUE_SEND_TO_FRONT /*!< 从前往后发送 */
} xf_task_queue_mode_t;

/* ==================== [Global Prototypes] ================================= */

/**
 * @brief 队列对象初始化。
 *
 * @param queue 队列对象。
 * @param data 队列数据指针。
 * @param size 队列数据大小。
 * @param count 队列数据个数。
 * @return xf_err_t
 *      - XF_ERR_INVALID_ARG 参数错误
 *      - XF_OK 初始化成功
 */
xf_err_t xf_task_queue_init(xf_task_queue_t *const queue, void *data, const size_t size, const size_t count);

/**
 * @brief 重置队列。
 *
 * @param queue 队列对象。
 * @return xf_err_t
 *      - XF_ERR_INVALID_ARG 参数错误
 *      - XF_OK 队列重置成功
 */
xf_err_t xf_task_queue_reset(xf_task_queue_t *const queue);

/**
 * @brief 判断队列是否为空。
 *
 * @param queue 队列对象。
 * @return true 队列为空
 * @return false 队列不为空
 */
bool xf_task_queue_is_empty(const xf_task_queue_t *const queue);

/**
 * @brief 获取队列数据个数
 *
 * @param queue 队列对象
 * @return size_t 队列数据个数
 */
size_t xf_task_queue_count(const xf_task_queue_t *const queue);

/**
 * @brief 获取队列第一个元素。
 *
 * @note 该函数不会删除队列第一个元素。
 *
 * @param queue 队列对象。
 * @return void* 队列第一个元素
 */
void *xf_task_queue_peek(const xf_task_queue_t *const queue);

/**
 * @brief 获取队列剩余空间。
 *
 * @param queue 队列对象。
 * @return size_t 队列剩余空间
 */
size_t xf_task_queue_available(const xf_task_queue_t *const queue);

/**
 * @brief 判断队列是否已满。
 *
 * @param queue 队列对象。
 * @return true 队列已满
 * @return false 队列未满
 */
bool xf_task_queue_is_full(const xf_task_queue_t *const queue);

/**
 * @brief 队列发送数据。
 *
 * @param queue 队列对象。
 * @param item 发送的数据。
 * @param pos 发送模式。
 * @return xf_err_t
 *      - XF_ERR_INVALID_ARG 参数错误
 *      - XF_ERR_BUSY 队列已满
 *      - XF_OK 发送成功
 */
xf_err_t xf_task_queue_send(xf_task_queue_t *const queue, void *item, const xf_task_queue_mode_t pos);

/**
 * @brief 从队列删除第一个元素。
 *
 * @param queue 队列对象。
 * @return xf_err_t
 *      - XF_ERR_INVALID_ARG 参数错误
 *      - XF_OK 删除成功
 */
xf_err_t xf_task_queue_remove_front(xf_task_queue_t *const queue);

/**
 * @brief 从队列接收一个元素。
 *
 * @param queue 队列对象。
 * @param buffer 接收的数据。
 * @return xf_err_t
 *      - XF_ERR_INVALID_ARG 参数错误
 *      - XF_OK 接收成功
 */
xf_err_t xf_task_queue_receive(xf_task_queue_t *const queue, void *const buffer);

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /* extern "C" */
#endif

/**
 * End of group_xf_task_user_task_queue
 * @}
 */

#endif // __XF_TASK_QUEUE_H__
