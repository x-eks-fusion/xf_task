/**
 * @file xf_ntask.h
 * @author cangyu (sky.kirto@qq.com)
 * @brief 无栈协程。
 * @version 0.1
 * @date 2024-02-29
 *
 * @copyright Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

#ifndef __XF_NTASK_H__
#define __XF_NTASK_H__

/* ==================== [Includes] ========================================== */

#include "../kernel/xf_task_kernel.h"

/**
 * @ingroup group_xf_task_user
 * @defgroup group_xf_task_user_ntask ntask
 * @brief 无栈协程。
 * @{
 */

#ifdef __cplusplus
extern "C"
{
#endif

/* ==================== [Defines] =========================================== */

/**
 * @brief ntask 类型值。
 * `XF_TASK_TYPE_ntask` 通过 `xf_task_reg.inc` 拼接而来。
 */
#define XF_TASK_TYPE_NTASK XF_TASK_TYPE_ntask
#define XF_NTASK_INFINITE_LOOP ((uint32_t) - 1) /*!< ntask 无限循环 */

/* ==================== [Typedefs] ========================================== */

/**
 * @brief ntask 创建传输参数。
 */
typedef struct _xf_ntask_config_t {
    uint32_t count;    /*!< ntask 循环次数 */
    uint32_t delay_ms; /*!< ntask 循环间隔时间 */
} xf_ntask_config_t;

/**
 * @brief ntask 信号量结构体。
 */
typedef struct _xf_ntask_sem_t {
    uint32_t count;
} xf_ntask_sem_t;

/* ==================== [Global Prototypes] ================================= */

/**
 * @brief 指定任务管理器创建 ntask。
 *
 * @param manager 任务管理器对象。
 * @param func 任务执行的函数。
 * @param func_arg 用户自定义执行函数参数。
 * @param priority 任务优先级。
 * @param delay_ms 任务延时周期。
 * @param count 任务循环的次数上限。
 * @return xf_task_t task 对象。返回为 NULL 则表示创建失败
 */
static inline xf_task_t xf_ntask_create_with_manager(
    xf_task_manager_t manager, xf_task_func_t func, void *func_arg,
    uint16_t priority, uint32_t delay_ms, uint32_t count)
{
    xf_ntask_config_t config = {.count = count, .delay_ms = delay_ms};
    return xf_task_create_with_manager(manager, XF_TASK_TYPE_NTASK, func, func_arg, priority, &config);
}

/**
 * @brief 指定任务管理器创建循环 ntask。
 *
 * @param manager 任务管理器对象。
 * @param func 任务执行的函数。
 * @param func_arg 用户自定义执行函数参数。
 * @param priority 任务优先级。
 * @param delay_ms 任务延时周期。
 * @return xf_task_t task 对象。返回为 NULL 则表示创建失败
 */
static inline xf_task_t xf_ntask_create_loop_with_manager(
    xf_task_manager_t manager, xf_task_func_t func,
    void *func_arg, uint16_t priority, uint32_t delay_ms)
{
    xf_ntask_config_t config = {.count = XF_NTASK_INFINITE_LOOP, .delay_ms = delay_ms};
    return xf_task_create_with_manager(manager, XF_TASK_TYPE_NTASK, func, func_arg, priority, &config);
}

/**
 * @brief 设置 ntask 循环次数。其不能超过循环次数的上限。
 *
 * @param task 任务对象。
 * @param count 循环次数。
 * @return xf_err_t
 *      - XF_ERR_INVALID_ARG 参数错误
 *      - XF_OK 设置成功
 */
xf_err_t xf_ntask_set_count(xf_task_t task, uint32_t count);

/**
 * @brief 获取 ntask 循环次数。
 *
 * @param task 任务对象。
 * @return uint32_t 循环次数
 */
uint32_t xf_ntask_get_count(xf_task_t task);

/**
 * @brief  ntask 循环次数加一。
 *
 * @param task 任务对象。
 * @return xf_err_t
 *      - XF_ERR_INVALID_ARG 参数错误
 *      - XF_ERR_NOT_SUPPORTED 不支持无限循环
 *      - XF_OK 设置成功
 */
xf_err_t xf_ntask_count_add_once(xf_task_t task);

/**
 * @brief 设置 ntask 循环次数的上限。
 *
 * @param task 任务对象。
 * @param count_max 循环次数上限。
 * @return xf_err_t
 *      - XF_ERR_INVALID_ARG 参数错误
 *      - XF_OK 设置成功
 */
xf_err_t xf_ntask_set_count_max(xf_task_t task, uint32_t count_max);

/**
 * @brief 设置 ntask 的上下文位置（无栈协程专属）。
 *
 * @param task 任务对象。
 * @param lc 上下文位置。
 * @return xf_err_t
 *      - XF_ERR_INVALID_ARG 参数错误
 *      - XF_OK 设置成功
 */
xf_err_t xf_ntask_set_lc(xf_task_t task, uint32_t lc);

/**
 * @brief 获取 ntask 的上下文位置。
 *
 * @note 该函数属于无栈协程专属。
 *
 * @param task 任务对象。
 * @return uint32_t 上下文位置
 */
uint32_t xf_ntask_get_lc(xf_task_t task);

/**
 * @brief 设置 ntask 的钩子指针。
 *
 * @note 该函数属于无栈协程专属，用于保存临时变量。
 *
 * @param task 任务对象。
 * @param ptr_hook 钩子指针，用于保存当前的指针，方便无栈协程下次访问。
 * @return xf_err_t
 *      - XF_ERR_INVALID_ARG 参数错误
 *      - XF_OK 设置成功
 */
xf_err_t xf_ntask_set_hook_ptr(xf_task_t task, void *ptr_hook);

/**
 * @brief 获取 ntask 的钩子指针。
 *
 * @note 该函数属于无栈协程专属，用于保存临时变量。
 *
 * @param task 任务对象。
 * @return void* 钩子指针
 */
void *xf_ntask_get_hook_ptr(xf_task_t task);

/* ==================== [Macros] ============================================ */

/**
 * @brief 无栈协程延时。
 *
 * @attention 这里延时只能放在无栈协程内，不可被函数调用，否则无法正常延时。
 *
 * @param delay_ms  延时的时间，单位为毫秒。
 */
#define xf_ntask_delay(delay_ms)                    \
    do                                              \
    {                                               \
        xf_task_set_delay(__xf_now_task, delay_ms); \
        XF_NTASK_YIELD();                           \
    } while (0)

/**
 * @brief 无栈协程的开始，当它出现表示后续内容为无栈协程内容。
 *
 * @note 需要搭配 XF_NTASK_END(ntask) 使用，否则会报错。
 *
 * @param ntask 当前的 ntask 任务。
 */
#define XF_NTASK_BEGIN(ntask)               \
    char __xf_task_yield_flag = 1;          \
    UNUSED(__xf_task_yield_flag);           \
    xf_task_t __xf_now_task = ntask;        \
    switch (xf_ntask_get_lc(__xf_now_task)) \
    {                                       \
    case 0:

/**
 * @brief 无栈协程的结束，当它出现表示结束无栈协程。
 *
 * @note 需要搭配 XF_NTASK_BEGIN(ntask) 使用，否则会报错。
 */
#define XF_NTASK_END()                 \
    }                                  \
    __xf_task_yield_flag = 0;          \
    xf_ntask_set_lc(__xf_now_task, 0); \
    xf_task_delete(__xf_now_task);     \
    return;

/**
 * @brief 无栈协程的等待，直到条件满足，才能执行后续内容。
 *
 * @param condition 等待条件，不满足则一直等待。
 */
#define XF_NTASK_WAIT_UNTIL(condition)              \
    do                                              \
    {                                               \
        xf_ntask_set_lc(__xf_now_task, __LINE__);   \
    /* FALLTHRU */                                  \
    case __LINE__:                                  \
        if (!(condition))                           \
        {                                           \
            xf_ntask_count_add_once(__xf_now_task); \
            return;                                 \
        }                                           \
    } while (0)

/**
 * @brief 无栈协程的等待，直到条件不满足，才能执行后续内容。
 *
 * @param cond 等待条件，满足则一直等待。
 */
#define XF_NTASK_WHILE(cond) XF_NTASK_UNTIL(!(cond))

/**
 * @brief 当前无栈协程重启。
 *
 */
#define XF_NTASK_RESTART()                      \
    do                                          \
    {                                           \
        xf_ntask_set_lc(__xf_now_task, 0);      \
        xf_ntask_count_add_once(__xf_now_task); \
        return;                                 \
    } while (0)

/**
 * @brief 强行退出并删除当前无栈协程（无视循环次数）。
 *
 */
#define XF_NTASK_EXIT()                    \
    do                                     \
    {                                      \
        xf_ntask_set_lc(__xf_now_task, 0); \
        xf_task_delete(__xf_now_task);     \
        return;                            \
    } while (0)

/**
 * @brief 无栈协程让出 CPU 执行权，进入调度器。
 *
 * @note 下次进入的时候还从这里继续执行。
 */
#define XF_NTASK_YIELD()                            \
    do                                              \
    {                                               \
        __xf_task_yield_flag = 0;                   \
        xf_ntask_set_lc(__xf_now_task, __LINE__);   \
    /* FALLTHRU */                                  \
    case __LINE__:                                  \
        if (__xf_task_yield_flag == 0)              \
        {                                           \
            xf_ntask_count_add_once(__xf_now_task); \
            return;                                 \
        }                                           \
    } while (0)

/**
 * @brief 无栈协程让出 CPU 执行权，进入调度器，直到条件满足。
 *
 * @note 下次进入的时候还从这里继续执行。
 *
 * @param cond 条件判断，不满足就一直让出 CPU 。
 */
#define XF_NTASK_YIELD_UNTIL(cond)                  \
    do                                              \
    {                                               \
        __xf_task_yield_flag = 0;                   \
        xf_ntask_set_lc(__xf_now_task, __LINE__);   \
    /* FALLTHRU */                                  \
    case __LINE__:                                  \
        if ((__xf_task_yield_flag == 0) || !(cond)) \
        {                                           \
            xf_ntask_count_add_once(__xf_now_task); \
            return;                                 \
        }                                           \
    } while (0)

/**
 * @brief 无栈协程计数信号量初始化。
 *
 * @param s 信号量对象 @ref xf_ntask_sem_t.
 * @param c 信号量初始值，信号量的资源总值。
 */
#define xf_ntask_sem_init(s, c) (s)->count = c

/**
 * @brief 无栈协程计数信号量等待，直到大于0才能执行。
 *
 * @note 一般用于请求资源。
 *
 * @param s 信号量对象 @ref xf_ntask_sem_t.
 */
#define xf_ntask_sem_wait(s)                 \
    do                                       \
    {                                        \
        XF_NTASK_WAIT_UNTIL((s)->count > 0); \
        --(s)->count;                        \
    } while (0)

/**
 * @brief 设置无栈协程计数信号量，唤醒等待的线程。
 *
 * @note 一般用于释放资源。
 *
 * @param s 信号量对象 @ref xf_ntask_sem_t.
 */
#define xf_ntask_sem_signal(s) ++(s)->count

#ifdef __cplusplus
} /* extern "C" */
#endif

/**
 * End of group_xf_task_user_ntask
 * @}
 */

#endif // __XF_TASK_NOCONTEXT_H__
