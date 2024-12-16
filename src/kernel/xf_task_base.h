/**
 * @file xf_task_base.h
 * @author cangyu (sky.kirto@qq.com)
 * @brief 本头文件外界无法调用，manager 基于 base 进行调度。
 *        开放部分在 xf_task_kernel.h .
 * @version 0.1
 * @date 2024-07-05
 *
 * @copyright Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

#ifndef __XF_TASK_BASE_H__
#define __XF_TASK_BASE_H__

/* ==================== [Includes] ========================================== */

#include "xf_task_kernel.h"

/**
 * @ingroup group_xf_task_internal
 * @defgroup group_xf_task_internal_base base
 * @brief 任务父对象。
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/**
 * @name XF_TASK_SIGNAL
 * @brief 内部传递消息使用，外部无法设置。
 * @{
 */
#define XF_TASK_SIGNAL_READY            (1UL << 0)  /*!< 准备信号，通知调度器该任务可进入就绪 */
#define XF_TASK_SIGNAL_TIMEOUT          (1UL << 1)  /*!< 超时信号，会在任务更新中转变为准备信号 */
#define XF_TASK_SIGNAL_EVENT            (1UL << 2)  /*!< 事件信号，会在任务更新中转变为准备信号 */
#define XF_TASK_SIGNAL_RESUME           (1UL << 3)  /*!< 恢复信号，通知任务从挂起状态被恢复 */
/**
 * End of XF_TASK_SIGNAL
 * @} 
 */

/**
 * @brief 外部设置的标志位，内部只会读取不会设置.
 */
#if XF_TASK_HUNGER_IS_ENABLE
#define XF_TASK_FALG_FEEL_HUNGERY       (1UL << 0) /*!< 饥饿标志，表示该任务具有饥饿值 */
#endif

/* ==================== [Typedefs] ========================================== */

/**
 * @brief 构造任务函数指针，函数对象由它构造。
 *
 * @param manager 任务所从属的任务管理器。
 * @param func  任务执行的函数。
 * @param func_arg 任务用户定义的参数。
 * @param priority 任务优先级。
 * @param config 任务参数配置（不同任务配置不同）。
 * @return xf_task_t 返回 NULL 则表示任务构造失败，构造成功则返回任务对象
 */
typedef xf_task_t (*xf_task_create_t)(xf_task_manager_t manager, xf_task_func_t func, void *func_arg, uint16_t priority,
                                      void *config);

/**
 * @brief 删除任务函数指针。函数对象的生命周期结尾时调用。
 *
 * @param task 任务对象。
 */
typedef void (*xf_task_delete_t)(xf_task_t task);

/**
 * @brief 任务重置函数。当任务执行不如意希望任务能重新执行的时候调用。
 *
 * @param task 任务对象。
 */
typedef void (*xf_task_reset_t)(xf_task_t task);

/**
 * @brief 任务更新函数。当任务出于阻塞状态时运行，更新当前任务状态。
 *
 * @param task 任务对象。
 * @return xf_task_time_t 当前更新参考的时间戳
 */
typedef xf_task_time_t (*xf_task_update_t)(xf_task_t task);

/**
 * @brief 任务执行函数。调度器用于恢复/执行当前任务。
 *
 * @param manager 任务管理器对象。
 */
typedef void(*xf_task_exec_t)(xf_task_manager_t manager);

/**
 * @brief 虚函数表，不在 base 对象实现，由子对象完成实现。
 */
typedef struct _xf_task_vfunc_t {
    const xf_task_create_t constructor; /*!< 构造任务虚函数 */
    const xf_task_reset_t reset;        /*!< 重置任务虚函数 */
    const xf_task_update_t update;      /*!< 更新任务虚函数 */
    const xf_task_exec_t exec;          /*!< 执行任务虚函数 */
} xf_task_vfunc_t;

/**
 * @brief task 的父对象，保存了 task 的公共属性。
 */
typedef struct _xf_task_base_t {
    xf_list_t node;                 /*!< 任务节点，挂载在 manager 上 */
    xf_task_manager_t manager;      /*!< 保存 task 所属的 manager ，以便更快访问 manager */
    xf_task_func_t func;            /*!< 每个任务所执行的内容 */
    void *arg;                      /*!< 任务中用户定义参数 */
    uint32_t type:      1;          /*!< 任务类型，见 @ref xf_task_type_t */
    uint32_t state:     3;          /*!< 任务状态，见 @ref xf_task_state_t */
    uint32_t flag:      9;          /*!< 任务标志位，外部设置的标志位，内部只会读取不会设置 */
    uint32_t signal:    9;          /*!< 任务间信号，内部传递消息使用，外部无法设置，
                                     *   见 XF_TASK_SIGNAL_* 宏 */
    uint32_t priority:  10;         /*!< 任务优先级，具体最大值参考 @ref XF_TASK_PRIORITY_LEVELS */
    uint32_t delay;                 /*!< 对类型于有上下文是延时时间，对于没有上下文则是定时周期  */
    xf_task_time_t weakup;          /*!< 唤醒时间，通过延时时间计算而来 */
    xf_task_time_t suspend_time;    /*!< 挂起时间，挂起期间内的时间不会算入延时时间 */
    int32_t timeout;                /*!< 超时时间，正数为超时时间，负数则属于提前唤醒 */
    const xf_task_vfunc_t *vfunc;   /*!< 虚函数指针，由子对象实现具体操作。
                                     *   虚函数指针是实现不同类型任务统一调度的关键 */
    xf_task_delete_t delete;        /*!< 虚函数指针，其内容通常为回收任务内存
                                     *   task pool 中通过替换它实现任务池回收任务 */

#if XF_TASK_HUNGER_IS_ENABLE
    xf_list_t hunger_node;          /*!< 饥饿节点，挂载在 manager 上的 hunger_list 上，
                                     *   以便更快速的遍历饥饿任务 */
    uint32_t hunger_time;           /*!< 任务饥饿度，单位为 ms。超过该时间，任务爬升一个优先级 */
#endif

#if XF_TASK_USER_DATA_IS_ENABLE
    void *user_data;                /*!< 用户传递的参数 */
#endif // XF_TASK_USER_DATA_IS_ENABLE

} xf_task_base_t;

/* ==================== [Global Prototypes] ================================= */

/**
 * @brief task 的虚函数注册。
 *
 * @param type 任务类型，该类型通过注册表 xf_task_reg.inc 实现静态注册任务类型。
 * @param vfunc 用于调用子任务功能的虚函数。
 * @return xf_err_t
 *      - XF_ERR_INVALID_ARG 参数错误
 *      - XF_ERR_INITED 已初始化
 *      - XF_OK 注册完成
 */
xf_err_t xf_task_vfunc_register(xf_task_type_t type, const xf_task_vfunc_t *vfunc);

/**
 * @brief task 注销虚函数注册（一般用不到的函数）。
 *
 * @param type 任务类型，该类型通过注册表 xf_task_reg.inc 实现静态注册任务类型。
 * @return xf_err_t
 *      - XF_ERR_INVALID_ARG 参数错误
 *      - XF_OK 注销完成
 */
xf_err_t xf_task_vfunc_unregister(xf_task_type_t type);

/**
 * @brief task 获取虚函数。
 *
 * @param type 任务，该类型通过注册表 xf_task_reg.inc 实现静态注册任务类型。
 * @return const xf_task_vfunc_t* 返回虚函数结构体地址
 */
const xf_task_vfunc_t *xf_task_get_vfunc(xf_task_type_t type);

/**
 * @brief task 设置状态。
 *
 * @param task task 任务对象。
 * @param state task 任务状态。
 * @return xf_err_t
 *      - XF_ERR_NOT_SUPPORTED 当任务出于删除态或者挂起态，此时设置状态无用
 *      - XF_OK 设置成功
 */
xf_err_t xf_task_base_set_state(xf_task_t task, xf_task_state_t state);

/**
 * @brief task 初始化。
 *
 * @param task_base task base 对象。
 * @param manager task 所从属的任务管理器。
 * @param type task 子任务类型，该类型通过注册表 xf_task_reg.inc 实现静态注册任务类型。
 * @param priority 任务优先级。
 * @param func 任务执行函数。
 * @param arg 任务自定义参数。
 */
void xf_task_base_init(xf_task_base_t *task_base, xf_task_manager_t manager, xf_task_type_t type,
                       uint16_t priority, xf_task_func_t func, void *arg);

/**
 * @brief 重置 task base 部分内容
 *
 * @param task_base task base 对象
 */
void xf_task_base_reset(xf_task_base_t *task_base);

/**
 * @brief 任务销毁。回收任务资源
 *
 * @param task 任务对象
 */
void xf_task_destructor(xf_task_t task);

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /* extern "C" */
#endif

/**
 * End of group_xf_task_internal_base
 * @}
 */

#endif // __XF_TASK_BASE_H__
