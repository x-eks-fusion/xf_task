/**
 * @file xf_ntask.h
 * @author cangyu (sky.kirto@qq.com)
 * @brief 无栈协程。
 * @version 0.1
 * @date 2024-12-16
 *
 * @copyright Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

#ifndef __XF_NTASK_H__
#define __XF_NTASK_H__

/* ==================== [Includes] ========================================== */

#include "../kernel/xf_task_kernel.h"

/**
 * @cond XFAPI_USER
 * @ingroup group_xf_task
 * @defgroup group_xf_task_ntask ntask
 * @brief 无栈协程。
 * @endcond
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/**
 * @brief ntask 类型值。
 * `XF_TASK_TYPE_ntask` 通过 `xf_task_reg.inc` 拼接而来。
 */
#define XF_TASK_TYPE_NTASK XF_TASK_TYPE_ntask

/* ==================== [Typedefs] ========================================== */

/**
 * @brief ntask 信号量结构体。
 */
typedef struct _xf_ntask_sem_t {
    uint32_t count;
} xf_ntask_sem_t;

typedef int(*xf_ntask_compare_func_t)(xf_task_t task);

typedef enum _xf_ntask_status_t {
    XF_NTASK_NONE = -1,
    XF_NTASK_WAITING,
    XF_NTASK_YIELDED,
    XF_NTASK_EXITED,
    XF_NTASK_ENDED,
    XF_NTASK_FINSHED,
} xf_ntask_status_t;

/* ==================== [Global Prototypes] ================================= */

/**
 * @brief 创建无栈协程
 *
 * @param manager 任务调度器
 * @param func 任务执行函数
 * @param func_arg 任务参数
 * @param priority 任务优先级
 * @return xf_task_t 任务对象，NULL 表示创建失败
 */
static inline
xf_task_t xf_ntask_create_with_manager(xf_task_manager_t manager, xf_task_func_t func, void *func_arg,
                                       uint16_t priority)
{
    return xf_task_create_with_manager(manager, XF_TASK_TYPE_NTASK, func, func_arg, priority, NULL);
}

/**
 * @brief 设置比较函数，回调函数返回 0，则任务继续进行，单次有效
 *
 * @param task 任务对象
 * @param compare 比较回调函数
 */
void xf_ntask_set_compare(xf_task_t task, xf_ntask_compare_func_t compare);

/**
 * @brief 获取保存的整数变量
 *
 * @param task 任务对象
 * @param name 整数变量名称
 * @return int 获取到的整数变量
 */
int xf_ntask_args_get_int(xf_task_t *task, const char *name);

/**
 * @brief 获取保存的浮点变量
 *
 * @param task 任务对象
 * @param name 浮点变量名称
 * @return float 获取到的浮点变量
 */
float xf_ntask_args_get_float(xf_task_t *task, const char *name);

/**
 * @brief 获取保存的数组
 *
 * @param task 任务对象
 * @param name 数组名称
 * @return void* 数组地址
 */
void *xf_ntask_args_get_array(xf_task_t *task, const char *name);

/**
 * @brief 保存整数变量
 *
 * @param task 任务对象
 * @param name 变量名称
 * @param value 整数变量值
 * @return xf_err_t 返回错误码
 */
xf_err_t xf_ntask_args_set_int(xf_task_t *task, const char *name, int value);

/**
 * @brief 保存浮点变量
 *
 * @param task 任务对象
 * @param name 变量名称
 * @param value 浮点变量值
 * @return xf_err_t 返回错误码
 */
xf_err_t xf_ntask_args_set_float(xf_task_t *task, const char *name, float value);

/**
 * @brief 保存数组
 *
 * @param task 任务对象
 * @param name 数组变量名称
 * @param value 数组变量值
 * @param size 数组元素大小
 * @param len 数组元素个数
 * @return xf_err_t 返回错误码
 */
xf_err_t xf_ntask_args_set_array(xf_task_t *task, const char *name, void *value, unsigned int size, unsigned int len);

/**
 * @brief 获取上下文位置 （给宏调用）
 *
 * @param task 任务对象
 * @param name 函数上下文名称
 * @return uint32_t 上下文位置
 */
uint32_t xf_ntask_get_lc(xf_task_t *task, const char *name);

/**
 * @brief 设置上下文位置 （给宏调用）
 *
 * @param task 任务对象
 * @param name 函数上下文名称
 * @param lc 上下文位置
 * @return xf_err_t 上下文位置
 */
xf_err_t xf_ntask_set_lc(xf_task_t *task, const char *name, uint32_t lc);

/**
 * @brief 获取退出状态 （给宏调用）
 *
 * @param task 任务对象
 * @return xf_ntask_status_t 任务退出状态
 */
xf_ntask_status_t xf_ntask_get_exit_status(xf_task_t *task);

/**
 * @brief 设置退出状态 （给宏调用）
 *
 * @param task 任务对象
 * @param status 任务退出状态
 * @return xf_err_t 是否设置成功
 */
xf_err_t xf_ntask_set_exit_status(xf_task_t *task, xf_ntask_status_t status);

/**
 * @brief 获取是否是最外层函数
 *
 * @param task 任务对象
 * @param name 任务上下文名称
 * @return true 是最外层函数
 * @return false 不是最外层函数
 */
bool xf_ntask_lc_is_first(xf_task_t *task, const char *name);

/* ==================== [Macros] ============================================ */

#define XF_NTASK_BEGIN(ntask)       \
    xf_task_t *_ntask = ntask;      \
    switch (xf_ntask_get_lc(ntask, __func__)) \
    {                               \
    case 0:

#define XF_NTASK_END()    \
    }                          \
    xf_ntask_set_lc(_ntask, __func__, 0); \
    if(xf_ntask_lc_is_first(_ntask, __func__)) { \
        xf_ntask_set_exit_status(_ntask, XF_NTASK_FINSHED); \
    } else {                    \
        xf_ntask_set_exit_status(_ntask, XF_NTASK_ENDED); \
    }\
    return

#define xf_ntask_yield()               \
    do                                      \
    {                                       \
        xf_task_trigger(_ntask);             \
        xf_ntask_set_lc(_ntask, __func__, __LINE__);   \
        xf_ntask_set_exit_status(_ntask, XF_NTASK_YIELDED); \
        return;                             \
    case __LINE__:;                         \
    } while (0)

#define xf_ntask_until(compare_cb)             \
    do                                      \
    {                                       \
        xf_ntask_set_compare(_ntask, compare_cb);      \
        xf_ntask_set_lc(_ntask, __func__, __LINE__);   \
        xf_ntask_set_exit_status(_ntask, XF_NTASK_WAITING); \
        return;                             \
    case __LINE__:;                         \
    } while (0)

#define xf_ntask_delay(delay_ms)     \
    do                                      \
    {                                       \
        xf_task_set_delay(_ntask, delay_ms); \
        xf_ntask_set_lc(_ntask, __func__, __LINE__);   \
        xf_ntask_set_exit_status(_ntask, XF_NTASK_WAITING); \
        return;                             \
    case __LINE__:;                         \
    } while (0)

#define xf_ntask_until_timeout(compare_cb, timeout_ms) \
    do                                      \
    {                                       \
        xf_task_set_delay(_ntask, timeout_ms); \
        xf_ntask_set_compare(_ntask, compare_cb);      \
        xf_ntask_set_lc(_ntask, __func__, __LINE__);   \
        xf_ntask_set_exit_status(_ntask, XF_NTASK_WAITING); \
        return;                             \
    case __LINE__:;                         \
    } while (0)

#define xf_ntask_exit()                \
    do                                      \
    {                                       \
        xf_ntask_set_lc(_ntask, __func__, 0);                \
        if(xf_ntask_lc_is_first(_ntask, __func__)) {         \
            xf_ntask_set_exit_status(_ntask, XF_NTASK_FINSHED);   \
        } else {                        \
            xf_ntask_set_exit_status(_ntask, XF_NTASK_ENDED);     \
        }                               \
        return;                         \
    } while (0)

typedef void xf_async_t;

#define xf_await(func)                      \
    case __LINE__:                          \
    func;\
    int _async_status = xf_ntask_get_exit_status(_ntask);\
    if (_async_status == XF_NTASK_YIELDED || _async_status == XF_NTASK_WAITING) \
    {\
        xf_ntask_set_lc(_ntask, __func__, __LINE__);   \
        return;                             \
    }\
    else\
    {\
        xf_ntask_set_exit_status(_ntask, XF_NTASK_NONE);\
    }

#ifdef __cplusplus
} /* extern "C" */
#endif

/**
 * End of group_xf_task_ntask
 * @}
 */

#endif // __XF_NTASK_H__
