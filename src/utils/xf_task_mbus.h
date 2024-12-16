/**
 * @file xf_task_mbus.h
 * @author cangyu (sky.kirto@qq.com)
 * @brief 消息总线（发布订阅）。
 * @version 0.1
 * @date 2024-04-08
 *
 * @copyright Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

#ifndef __XF_TASK_MBUS_H__
#define __XF_TASK_MBUS_H__

/* ==================== [Includes] ========================================== */

#include "xf_task_utils_config.h"

#if XF_TASK_MBUS_IS_ENABLE

#include "xf_utils.h"

/**
 * @ingroup group_xf_task_user
 * @defgroup group_xf_task_user_mbus mbus
 * @brief 消息总线（发布订阅）。
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/**
 * @brief mbus 的数据订阅回调函数原型。
 *
 * @param data 传输的数据。
 * @param user_data 用户自定义参数。
 */
typedef void (*xf_task_mbus_func_t)(const void *const data, void *user_data);

/* ==================== [Global Prototypes] ================================= */

/**
 * @brief 注册 topic。
 *
 * @param topic_id 需要注册的 topic id。
 * @param size topic 传输数据大小。
 * @return xf_err_t
 *      - XF_ERR_INITED topic 已经被初始化
 *      - XF_OK topic 注册成功
 */
xf_err_t xf_task_mbus_reg_topic(uint32_t topic_id, uint32_t size);

/**
 * @brief 注销 topic
 *
 * @param topic_id topic 的 id 号
 * @return xf_err_t
 *      - XF_ERR_NOT_FOUND topic 不存在
 *      - XF_OK topic 注销成功
 */
xf_err_t xf_task_mbus_unreg_topic(uint32_t topic_id);

/**
 * @brief 异步发布指定的 topic ，不会阻塞代码运行。
 *
 * @param topic_id 需要发布的 topic id。
 * @param data 传输数据（传递地址方式）。
 * @return xf_err_t
 *      - XF_ERR_INVALID_ARG 参数错误
 *      - XF_ERR_NOT_FOUND topic 不存在
 *      - XF_OK topic 发布成功
 */
xf_err_t xf_task_mbus_pub_async(uint32_t topic_id, void *data);

/**
 * @brief 同步发布，直接执行订阅者的回调，执行速度快。
 *
 * @param topic_id 需要发布的 topic id。
 * @param data 传输数据（传递地址方式）。
 * @return xf_err_t
 *      - XF_ERR_INVALID_ARG 参数错误
 *      - XF_ERR_NOT_FOUND topic 不存在
 *      - XF_OK topic 发布成功
 */
xf_err_t xf_task_mbus_pub_sync(uint32_t topic_id, void *data);

/**
 * @brief 订阅指定的 topic。
 *
 * @param topic_id 订阅的 topic id。
 * @param mbus_cb  收到消息后处理的回调。
 * @param user_data 用户的数据。
 * @return xf_err_t
 *      - XF_ERR_INVALID_ARG 参数错误
 *      - XF_ERR_NOT_FOUND topic 不存在
 *      - XF_OK topic 订阅成功
 */
xf_err_t xf_task_mbus_sub(uint32_t topic_id, xf_task_mbus_func_t mbus_cb, void *user_data);

/**
 * @brief 解除订阅。
 *
 * @param topic_id 解除订阅的 topic id。
 * @param mbus_cb  解除的回调。
 * @return xf_err_t
 *      - XF_ERR_INVALID_ARG 参数错误
 *      - XF_ERR_NOT_FOUND topic 不存在
 *      - XF_OK topic 解除订阅成功
 */
xf_err_t xf_task_mbus_unsub(uint32_t topic_id, xf_task_mbus_func_t mbus_cb);

/**
 * @brief 解除 topic下所有订阅。
 *
 * @param topic_id 解除订阅的 topic id。
 * @return xf_err_t
 *      - XF_ERR_NOT_FOUND topic 不存在
 *      - XF_OK topic 解除订阅成功
 */
xf_err_t xf_task_mbus_unsub_all(uint32_t topic_id);

/**
 * @brief 处理异步的消息。
 * 
 * @note 给异步订阅使用的，需要循环调用。
 */
void xf_task_mbus_handle(void);

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /* extern "C" */
#endif

/**
 * End of group_xf_task_user_mbus
 * @}
 */

#endif // XF_TASK_MBUS_IS_ENABLE

#endif // __XF_TASK_MBUS_H__
