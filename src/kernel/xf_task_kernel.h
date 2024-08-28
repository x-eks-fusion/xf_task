/**
 * @file xf_task_kernel.h
 * @author cangyu (sky.kirto@qq.com)
 * @brief xf_task 内核。
 * @version 0.1
 * @date 2024-07-05
 *
 * @copyright Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

#ifndef __XF_TASK_KERNEL_H__
#define __XF_TASK_KERNEL_H__

/* ==================== [Includes] ========================================== */

#include "xf_task_manager.h"

/**
 * @ingroup group_xf_task_user
 * @defgroup group_xf_task_user_kernel kernel
 * @brief xf_task 内核。
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/**
 * @brief 任务调用函数原型。
 *
 * @param task 任务对象。
 */
typedef void (*xf_task_func_t)(xf_task_t task);

/**
 * @brief 任务状态枚举。
 */
typedef enum _xf_task_state_t {
    _XF_TASK_STATE_NONE = -1,
    XF_TASK_STATE_READY,    /*!< 任务就绪态，任务在不同的优先级链表中等待执行 */
    XF_TASK_STATE_RUNNING,  /*!< 任务运行态，任务正在执行，同一个管理器只有一个执行态任务 */
    XF_TASK_STATE_BLOCKED,  /*!< 任务阻塞态，任务在执行完成后进入阻塞态 */
    XF_TASK_STATE_SUSPEND,  /*!< 任务挂起态，不进入调度循环，只有恢复才能加入调度 */
    XF_TASK_STATE_DELETE,   /*!< 任务删除态，任务即将被删除，管理器空闲的时候，删除这些任务 */
} xf_task_state_t;

/**
 * @brief 任务类型，通过注册表 xf_task_reg.inc 生成相关的内容。
 */
typedef enum _xf_task_type_t {
    _XF_TASK_TYPE_NONE = -1,
#define XF_TASK_REG_ENUM
#include "../task/xf_task_reg.inc"
    _XF_TASK_TYPE_MAX,
} xf_task_type_t;

/* ==================== [Global Prototypes] ================================= */

/**
 * @brief 在指定任务管理器下创建任务。
 *
 * @param manager 指定的任务管理器。
 * @param type 任务类型。
 * @param func 任务执行函数。
 * @param func_arg 任务用户自定义参数。
 * @param priority 任务优先级。
 * @param config 任务配置结构体，根据不同类型任务不同配置。
 * @return xf_task_t 任务对象，返回为 NULL 则创建失败
 */
xf_task_t xf_task_create_with_manager(
    xf_task_manager_t manager, xf_task_type_t type, xf_task_func_t func,
    void *func_arg, uint16_t priority, void *config);

/**
 * @brief 任务删除函数。将任务加入销毁队列，并设置任务为删除态。
 *
 * @param task 任务对象。
 */
void xf_task_delete(xf_task_t task);

/**
 * @brief 任务重置。将任务的一些参数进行重置。
 *
 * @param task 任务对象。
 */
void xf_task_reset(xf_task_t task);

/**
 * @brief 从任务对象获取任务自定义参数。
 *
 * @param task 任务对象。
 * @return void* 用户自定义函数参数（创建的时候定义）
 */
void *xf_task_get_arg(xf_task_t task);

/**
 * @brief 获取任务的超时时间。
 *
 * @param task 任务对象。
 * @return int32_t 正数为超过指定循环的时间，负数为提前执行（可能是trigger触发）的时间
 */
int32_t xf_task_get_timeout(xf_task_t task);

/**
 * @brief 从任务获取管理器对象。
 *
 * @param task 任务对象。
 * @return xf_task_manager_t 获取任务管理器对象
 */
xf_task_manager_t xf_task_get_manager(xf_task_t task);

#if XF_TASK_HUNGER_IS_ENABLE

/**
 * @brief 启用任务感受饥饿的功能。
 *
 * @param task 任务对象。
 * @param hunger_time 任务的饥饿值。单位为 ms，超时后饥饿值会爬升一个优先级。
 */
void xf_task_feel_hungery_enable(xf_task_t task, uint32_t hunger_time);

/**
 * @brief 禁用任务感受饥饿功能。
 *
 * @param task 任务对象。
 */
void xf_task_feel_hungery_disable(xf_task_t task);

#endif // XF_TASK_HUNGER_IS_ENABLE

/**
 * @brief 任务进入挂起。挂起态的任务不会参与调度。
 *
 * @note 任务出于挂起中的时候，时间不算入延时事件中。
 *
 * @param task 任务对象。
 * @return xf_err_t
 *      - XF_ERR_INVALID_ARG 参数错误
 *      - XF_ERR_INVALID_STATE 状态错误，状态不能是删除态
 *      - XF_OK 挂起成功
 */
xf_err_t xf_task_suspend(xf_task_t task);

/**
 * @brief 任务从挂起中恢复，进入调度。
 *
 * @note 恢复的任务默认出于阻塞态。
 *
 * @param task 任务对象。
 * @return xf_err_t
 *      - XF_ERR_INVALID_ARG 参数错误
 *      - XF_ERR_INVALID_STATE 状态错误，状态必须属于挂起态
 *      - XF_OK 恢复成功
 */
xf_err_t xf_task_resume(xf_task_t task);

/**
 * @brief 任务触发。无论是否到达唤醒时间，都会触发。
 *
 * @param task 任务对象。
 * @return xf_err_t
 *      - XF_ERR_INVALID_ARG 参数错误
 *      - XF_OK 设置成功
 */
xf_err_t xf_task_trigger(xf_task_t task);

/**
 * @brief 获取当前任务类型。
 *
 * @param task 任务对象
 * @return xf_task_type_t 返回当前指定任务的类型
 */
xf_task_type_t xf_task_get_type(xf_task_t task);

/**
 * @brief 获取当前任务的状态。
 *
 * @param task 任务对象。
 * @return xf_task_state_t 当前任务的状态。
 */
xf_task_state_t xf_task_get_state(xf_task_t task);

/**
 * @brief 获取当前任务的优先级。
 *
 * @param task 任务对象。
 * @return uint16_t 当前指定任务的优先级
 */
uint16_t xf_task_get_priority(xf_task_t task);

/**
 * @brief 设置当前任务的优先级。
 *
 * @param task 任务对象。
 * @param priority 任务的优先级设置。
 * @return xf_err_t
 *      - XF_ERR_INVALID_ARG 参数错误，
 *      - XF_OK 优先级设置成功
 */
xf_err_t xf_task_set_priority(xf_task_t task, uint16_t priority);

/**
 * @brief 设置任务的延时。
 * @note 不同任务的延时意义不同。
 *
 * @param task 任务对象。
 * @param delay_ms 延时时间，单位为 ms。
 * @return xf_err_t
 *      - XF_ERR_INVALID_ARG 参数错误，
 *      - XF_OK 任务延时设置成功
 */
xf_err_t xf_task_set_delay(xf_task_t task, uint32_t delay_ms);

/**
 * @brief 设置当前任务的执行函数（某些种类任务可能不适用）。
 *
 * @param task 任务对象。
 * @param func 执行的函数。
 * @param arg 用户自定义执行任务的参数。
 * @return xf_err_t
 *      - XF_ERR_INVALID_ARG 参数错误，
 *      - XF_OK 设置函数成功
 */
xf_err_t xf_task_set_func(xf_task_t task, xf_task_func_t func, void *arg);

#if XF_TASK_USER_DATA_IS_ENABLE

/**
 * @brief 设置当前任务的 user_data。
 *
 * @param task 任务对象。
 * @param user_data 用户自定义的数据。
 * @return xf_err_t
 *      - XF_ERR_INVALID_ARG 参数错误，
 *      - XF_OK 设置函数成功
 */
xf_err_t xf_task_set_user_data(xf_task_t task, void *user_data);

/**
 * @brief 获取当前任务的 user_data。
 *
 * @param task 任务对象。
 * @return void* 返回 user_data. 如果为 NULL 则返回失败
 */
void *xf_task_get_user_data(xf_task_t task);

#endif // XF_TASK_USER_DATA_IS_ENABLE

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /* extern "C" */
#endif

/**
 * End of group_xf_task_user_kernel
 * @}
 */

#endif // __XF_TASK_KERNEL_H__
