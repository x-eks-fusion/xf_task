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

typedef int(*xf_ntask_compare_func_t)(xf_task_t task);

typedef enum _xf_ntask_status_t {
    XF_NTASK_NONE = -1,
    XF_NTASK_WAITING,
    XF_NTASK_YIELDED,
    XF_NTASK_EXITED,
    XF_NTASK_ENDED,
    XF_NTASK_FINSHED,
} xf_ntask_status_t;

typedef struct _xf_ntask_stack_t
{
    void* addr;
    uint32_t size;
}xf_ntask_stack_t;

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

void *xf_ntask_args_create(xf_task_t task, const char *name, unsigned int size);
void *xf_ntask_args_find(xf_task_t task, const char *name);
xf_err_t xf_ntask_stack_load(xf_task_t task, const char *name, xf_ntask_stack_t *stack, uint32_t len);
xf_err_t xf_ntask_stack_save(xf_task_t task, const char *name, xf_ntask_stack_t *stack, uint32_t len);

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

#define EVAL4(...) EVAL3(EVAL3(EVAL3(EVAL3(__VA_ARGS__))))
#define EVAL3(...) EVAL2(EVAL2(EVAL2(EVAL2(__VA_ARGS__))))
#define EVAL2(...) EVAL1(EVAL1(EVAL1(EVAL1(__VA_ARGS__))))
#define EVAL1(...) __VA_ARGS__

#define XF_NTASK_GAP
#define XF_NTASK_DROP(...)

//
#define XF_NTASK_NARG(...) EVAL4(XF_NTASK_NARG_ONCE0(__VA_ARGS__, (), (), 0))
#define XF_NTASK_NARG_ONCE0(argTask, argType, argName, ...) XF_NTASK_NARG_NEXT00(argTask, argType, argName, XF_NTASK_NARG_LAST0 argName, XF_NTASK_NARG_BACK0, 0)(argTask, __VA_ARGS__)
#define XF_NTASK_NARG_ONCE1(argTask, argType, argName, ...) XF_NTASK_NARG_NEXT10(argTask, argType, argName, XF_NTASK_NARG_LAST1 argName, XF_NTASK_NARG_BACK1, 0)(argTask, __VA_ARGS__)
#define XF_NTASK_NARG_LAST0(...) 0, XF_NTASK_NARG_LAST_NONE
#define XF_NTASK_NARG_LAST1(...) 0, XF_NTASK_NARG_LAST_MANY
#define XF_NTASK_NARG_NEXT00(...) XF_NTASK_NARG_NEXT01(__VA_ARGS__)
#define XF_NTASK_NARG_NEXT10(...) XF_NTASK_NARG_NEXT11(__VA_ARGS__)
#define XF_NTASK_NARG_NEXT01(argTask, argType, argName, drop, func, ...) func XF_NTASK_GAP(argTask, argType, argName)
#define XF_NTASK_NARG_NEXT11(argTask, argType, argName, drop, func, ...) func XF_NTASK_GAP(argTask, argType, argName)

#define XF_NTASK_NARG_LAST_NONE(argTask, argType, argName) XF_NTASK_DROP
#define XF_NTASK_NARG_BACK0(argTask, argType, argName) +1 XF_NTASK_NARG_ONCE1
#define XF_NTASK_NARG_BACK1(argTask, argType, argName) +1 XF_NTASK_NARG_ONCE1
#define XF_NTASK_NARG_LAST_MANY(argTask, argType, argName) XF_NTASK_DROP

//
#define XF_NTASK_SIZEOF(...) EVAL4(XF_NTASK_SIZEOF_ONCE0(__VA_ARGS__, (), (), 0))
#define XF_NTASK_SIZEOF_ONCE0(argTask, argType, argName, ...) XF_NTASK_SIZEOF_NEXT00(argTask, argType, argName, XF_NTASK_SIZEOF_LAST0 argName, XF_NTASK_SIZEOF_BACK0, 0)(argTask, __VA_ARGS__)
#define XF_NTASK_SIZEOF_ONCE1(argTask, argType, argName, ...) XF_NTASK_SIZEOF_NEXT10(argTask, argType, argName, XF_NTASK_SIZEOF_LAST1 argName, XF_NTASK_SIZEOF_BACK1, 0)(argTask, __VA_ARGS__)
#define XF_NTASK_SIZEOF_LAST0(...) 0, XF_NTASK_SIZEOF_LAST_NONE
#define XF_NTASK_SIZEOF_LAST1(...) 0, XF_NTASK_SIZEOF_LAST_MANY
#define XF_NTASK_SIZEOF_NEXT00(...) XF_NTASK_SIZEOF_NEXT01(__VA_ARGS__)
#define XF_NTASK_SIZEOF_NEXT10(...) XF_NTASK_SIZEOF_NEXT11(__VA_ARGS__)
#define XF_NTASK_SIZEOF_NEXT01(argTask, argType, argName, drop, func, ...) func XF_NTASK_GAP(argTask, argType, argName)
#define XF_NTASK_SIZEOF_NEXT11(argTask, argType, argName, drop, func, ...) func XF_NTASK_GAP(argTask, argType, argName)

#define XF_NTASK_SIZEOF_LAST_NONE(argTask, argType, argName) XF_NTASK_DROP
#define XF_NTASK_SIZEOF_BACK0(argTask, argType, argName) +sizeof(argType) XF_NTASK_SIZEOF_ONCE1
#define XF_NTASK_SIZEOF_BACK1(argTask, argType, argName) +sizeof(argType) XF_NTASK_SIZEOF_ONCE1
#define XF_NTASK_SIZEOF_LAST_MANY(argTask, argType, argName) XF_NTASK_DROP

//
#define XF_NTASK_STK_DECL(...) EVAL4(XF_NTASK_STK_DECL_ONCE0(__VA_ARGS__, (), 0))
#define XF_NTASK_STK_DECL_ONCE0(argTask, argType, argName, ...) XF_NTASK_STK_DECL_NEXT0(argTask, argType, argName, XF_NTASK_STK_DECL_LAST0 argName, XF_NTASK_STK_DECL_LAST_MANY, __VA_ARGS__)
#define XF_NTASK_STK_DECL_LAST0(...) 0, XF_NTASK_STK_DECL_LAST_NONE
#define XF_NTASK_STK_DECL_NEXT0(...) XF_NTASK_STK_DECL_NEXT1(__VA_ARGS__)
#define XF_NTASK_STK_DECL_NEXT1(argTask, argType, argName, drop, func, ...) func XF_NTASK_GAP(argTask, argType, argName, __VA_ARGS__)

#define XF_NTASK_STK_DECL_LAST_NONE(argTask, argType, argName, d, ...) xf_ntask_stack_t *_stack = NULL
#define XF_NTASK_STK_DECL_LAST_MANY(argTask, argType, argName, ...) xf_ntask_stack_t _stack[0 XF_NTASK_NARG_ONCE0(argTask, argType, argName, __VA_ARGS__)] = {XF_NTASK_STK_ITEM_ONCE0(argTask, argType, argName, __VA_ARGS__)}

//
#define XF_NTASK_STK_ITEM(...) EVAL4(XF_NTASK_STK_ITEM_ONCE0(__VA_ARGS__, (), (), 0))
#define XF_NTASK_STK_ITEM_ONCE0(argTask, argType, argName, ...) XF_NTASK_STK_ITEM_NEXT00(argTask, argType, argName, XF_NTASK_STK_ITEM_LAST0 argName, XF_NTASK_STK_ITEM_BACK0, 0)(argTask, __VA_ARGS__)
#define XF_NTASK_STK_ITEM_ONCE1(argTask, argType, argName, ...) XF_NTASK_STK_ITEM_NEXT10(argTask, argType, argName, XF_NTASK_STK_ITEM_LAST1 argName, XF_NTASK_STK_ITEM_BACK1, 0)(argTask, __VA_ARGS__)
#define XF_NTASK_STK_ITEM_LAST0(...) 0, XF_NTASK_STK_ITEM_LAST_NONE
#define XF_NTASK_STK_ITEM_LAST1(...) 0, XF_NTASK_STK_ITEM_LAST_MANY
#define XF_NTASK_STK_ITEM_NEXT00(...) XF_NTASK_STK_ITEM_NEXT01(__VA_ARGS__)
#define XF_NTASK_STK_ITEM_NEXT10(...) XF_NTASK_STK_ITEM_NEXT11(__VA_ARGS__)
#define XF_NTASK_STK_ITEM_NEXT01(argTask, argType, argName, drop, func, ...) func XF_NTASK_GAP(argTask, argType, argName)
#define XF_NTASK_STK_ITEM_NEXT11(argTask, argType, argName, drop, func, ...) func XF_NTASK_GAP(argTask, argType, argName)

#define XF_NTASK_STK_ITEM_LAST_NONE(argTask, argType, argName) XF_NTASK_DROP
#define XF_NTASK_STK_ITEM_BACK0(argTask, argType, argName) {.addr = &argName, .size = sizeof(argType)} XF_NTASK_STK_ITEM_ONCE1
#define XF_NTASK_STK_ITEM_BACK1(argTask, argType, argName) , {.addr = &argName, .size = sizeof(argType)} XF_NTASK_STK_ITEM_ONCE1
#define XF_NTASK_STK_ITEM_LAST_MANY(argTask, argType, argName) XF_NTASK_DROP

//
#define XF_NTASK_VAR_DECL(...) EVAL4(XF_NTASK_VAR_DECL_ONCE0(__VA_ARGS__, (), (), 0))
#define XF_NTASK_VAR_DECL_ONCE0(argTask, argType, argName, ...) XF_NTASK_VAR_DECL_NEXT00(argTask, argType, argName, XF_NTASK_VAR_DECL_LAST0 argName, XF_NTASK_VAR_DECL_BACK0, 0)(argTask, __VA_ARGS__)
#define XF_NTASK_VAR_DECL_ONCE1(argTask, argType, argName, ...) XF_NTASK_VAR_DECL_NEXT10(argTask, argType, argName, XF_NTASK_VAR_DECL_LAST1 argName, XF_NTASK_VAR_DECL_BACK1, 0)(argTask, __VA_ARGS__)
#define XF_NTASK_VAR_DECL_LAST0(...) 0, XF_NTASK_VAR_DECL_LAST_NONE
#define XF_NTASK_VAR_DECL_LAST1(...) 0, XF_NTASK_VAR_DECL_LAST_MANY
#define XF_NTASK_VAR_DECL_NEXT00(...) XF_NTASK_VAR_DECL_NEXT01(__VA_ARGS__)
#define XF_NTASK_VAR_DECL_NEXT10(...) XF_NTASK_VAR_DECL_NEXT11(__VA_ARGS__)
#define XF_NTASK_VAR_DECL_NEXT01(argTask, argType, argName, drop, func, ...) func XF_NTASK_GAP(argTask, argType, argName)
#define XF_NTASK_VAR_DECL_NEXT11(argTask, argType, argName, drop, func, ...) func XF_NTASK_GAP(argTask, argType, argName)

#define XF_NTASK_VAR_DECL_LAST_NONE(argTask, argType, argName) XF_NTASK_DROP
#define XF_NTASK_VAR_DECL_BACK0(argTask, argType, argName) \
    argType argName;                                       \
    XF_NTASK_VAR_DECL_ONCE1
#define XF_NTASK_VAR_DECL_BACK1(argTask, argType, argName) \
    argType argName;                                       \
    XF_NTASK_VAR_DECL_ONCE1
#define XF_NTASK_VAR_DECL_LAST_MANY(argTask, argType, argName) XF_NTASK_DROP

//
#define XF_NTASK_ARG_CREATE(...) EVAL4(XF_NTASK_ARG_CREATE_ONCE0(__VA_ARGS__, (), 0))
#define XF_NTASK_ARG_CREATE_ONCE0(argTask, argType, argName, ...) XF_NTASK_ARG_CREATE_NEXT0(argTask, argType, argName, XF_NTASK_ARG_CREATE_LAST0 argName, XF_NTASK_ARG_CREATE_LAST_MANY, __VA_ARGS__)
#define XF_NTASK_ARG_CREATE_LAST0(...) 0, XF_NTASK_ARG_CREATE_LAST_NONE
#define XF_NTASK_ARG_CREATE_NEXT0(...) XF_NTASK_ARG_CREATE_NEXT1(__VA_ARGS__)
#define XF_NTASK_ARG_CREATE_NEXT1(argTask, argType, argName, drop, func, ...) func XF_NTASK_GAP(argTask, argType, argName, __VA_ARGS__)

#define XF_NTASK_ARG_CREATE_LAST_NONE(argTask, argType, argName, d, ...)
#define XF_NTASK_ARG_CREATE_LAST_MANY(argTask, argType, argName, ...) xf_ntask_args_create(_ntask, __func__, 0 XF_NTASK_SIZEOF_ONCE0(argTask, argType, argName, __VA_ARGS__));

//
#define XF_NTASK_BEGIN_IMPL(ntask)            \
    xf_task_t *_ntask = ntask;                \
    switch (xf_ntask_get_lc(ntask, __func__)) \
    {                                         \
    case 0:

#define XF_NTASK_BEGIN_A(ntask, ...)                            \
    XF_NTASK_VAR_DECL(ntask, __VA_ARGS__)                       \
    uint32_t _stack_size = 0 XF_NTASK_NARG(ntask, __VA_ARGS__); \
    XF_NTASK_STK_DECL(ntask, __VA_ARGS__);                      \
    XF_NTASK_BEGIN_IMPL(ntask)                                  \
    XF_NTASK_ARG_CREATE(ntask, __VA_ARGS__)

#define XF_NTASK_BEGIN(...) XF_NTASK_BEGIN_A(__VA_ARGS__, (), ())
   
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
        xf_ntask_stack_save(_ntask, __func__, &_stack, _stack_size);\
        return;                             \
    case __LINE__:                          \
        xf_ntask_stack_load(_ntask, __func__, &_stack, _stack_size);\
    } while (0)

#define xf_ntask_until(compare_cb)             \
    do                                      \
    {                                       \
        xf_ntask_set_compare(_ntask, compare_cb);      \
        xf_ntask_set_lc(_ntask, __func__, __LINE__);   \
        xf_ntask_set_exit_status(_ntask, XF_NTASK_WAITING); \
        xf_ntask_stack_save(_ntask, __func__, _stack, _stack_size);\
        return;                             \
    case __LINE__:                          \
        xf_ntask_stack_load(_ntask, __func__, _stack, _stack_size);\
    } while (0)

#define xf_ntask_delay(delay_ms)     \
    do                                      \
    {                                       \
        xf_task_set_delay(_ntask, delay_ms); \
        xf_ntask_set_lc(_ntask, __func__, __LINE__);   \
        xf_ntask_set_exit_status(_ntask, XF_NTASK_WAITING); \
        xf_ntask_stack_save(_ntask, __func__, _stack, _stack_size);\
        return;                             \
    case __LINE__:                          \
        xf_ntask_stack_load(_ntask, __func__, _stack, _stack_size);\
    } while (0)

#define xf_ntask_until_timeout(compare_cb, timeout_ms) \
    do                                      \
    {                                       \
        xf_task_set_delay(_ntask, timeout_ms); \
        xf_ntask_set_compare(_ntask, compare_cb);      \
        xf_ntask_set_lc(_ntask, __func__, __LINE__);   \
        xf_ntask_set_exit_status(_ntask, XF_NTASK_WAITING); \
        xf_ntask_stack_save(_ntask, __func__, _stack, _stack_size);\
        return;                             \
    case __LINE__:                          \
        xf_ntask_stack_load(_ntask, __func__, _stack, _stack_size);\
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
    xf_ntask_stack_save(_ntask, __func__, _stack, _stack_size);\
    case __LINE__:                          \
    xf_ntask_stack_load(_ntask, __func__, _stack, _stack_size);\
    func;\
    int _async_status = xf_ntask_get_exit_status(_ntask);\
    if (_async_status == XF_NTASK_YIELDED || _async_status == XF_NTASK_WAITING) \
    {\
        xf_ntask_set_lc(_ntask, __func__, __LINE__);   \
        xf_ntask_stack_save(_ntask, __func__, _stack, _stack_size);\
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
