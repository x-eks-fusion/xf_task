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
 * @ingroup group_xf_task_user
 * @defgroup group_xf_task_user_ntask ntask
 * @brief 无栈协程。
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

static inline
xf_task_t xf_ntask_create_with_manager(xf_task_manager_t manager, xf_task_func_t func, void *func_arg,
                                       uint16_t priority)
{
    return xf_task_create_with_manager(manager, XF_TASK_TYPE_NTASK, func, func_arg, priority, NULL);
}

void xf_ntask_set_compare(xf_task_t task, xf_ntask_compare_func_t compare);

int xf_ntask_args_get_int(xf_task_t *task, const char *name);

float xf_ntask_args_get_float(xf_task_t *task, const char *name);

const char *xf_ntask_args_get_string(xf_task_t *task, const char *name);

void *xf_ntask_args_get_array(xf_task_t *task, const char *name);

xf_err_t xf_ntask_args_set_int(xf_task_t *task, const char *name, int value);

xf_err_t xf_ntask_args_set_float(xf_task_t *task, const char *name, float value);

xf_err_t xf_ntask_args_set_array(xf_task_t *task, const char *name, void *value, unsigned int size, unsigned int len);

uint32_t xf_ntask_get_lc(xf_task_t *task, const char *name);

xf_err_t xf_ntask_set_lc(xf_task_t *task, const char *name, uint32_t lc);

xf_ntask_status_t xf_ntask_get_status(xf_task_t *task);

xf_err_t xf_ntask_set_status(xf_task_t *task, xf_ntask_status_t status);

bool xf_ntask_lc_is_first(xf_task_t *task, const char *name);

/* ==================== [Macros] ============================================ */

#define XF_NTASK_BEGIN(ntask)       \
    switch (xf_ntask_get_lc(ntask, __func__)) \
    {                               \
    case 0:

#define XF_NTASK_END(ntask)    \
    }                          \
    xf_ntask_set_lc(ntask, __func__, 0); \
    if(xf_ntask_lc_is_first(ntask, __func__)) { \
        xf_ntask_set_status(ntask, XF_NTASK_FINSHED); \
    } else {                    \
        xf_ntask_set_status(ntask, XF_NTASK_ENDED); \
    }\
    return

#define xf_ntask_yield(ntask)               \
    do                                      \
    {                                       \
        xf_task_trigger(ntask);             \
        xf_ntask_set_lc(ntask, __func__, __LINE__);   \
        xf_ntask_set_status(ntask, XF_NTASK_YIELDED); \
        return;                             \
    case __LINE__:                          \
    } while (0)

#define xf_ntask_until(ntask, compare_cb)             \
    do                                      \
    {                                       \
        xf_ntask_set_compare(ntask, compare_cb);      \
        xf_ntask_set_lc(ntask, __func__, __LINE__);   \
        xf_ntask_set_status(ntask, XF_NTASK_WAITING); \
        return;                             \
    case __LINE__:                          \
    } while (0)

#define xf_ntask_delay(ntask, delay_ms)     \
    do                                      \
    {                                       \
        xf_task_set_delay(ntask, delay_ms); \
        xf_ntask_set_lc(ntask, __func__, __LINE__);   \
        xf_ntask_set_status(ntask, XF_NTASK_WAITING); \
        return;                             \
    case __LINE__:                          \
    } while (0)

#define xf_ntask_until_timeout(ntask, compare_cb, timeout_ms) \
    do                                      \
    {                                       \
        xf_task_set_delay(ntask, timeout_ms); \
        xf_ntask_set_compare(ntask, compare_cb);      \
        xf_ntask_set_lc(ntask, __func__, __LINE__);   \
        xf_ntask_set_status(ntask, XF_NTASK_WAITING); \
        return;                             \
    case __LINE__:                          \
    } while (0)

#define xf_ntask_exit(ntask)                \
    do                                      \
    {                                       \
        xf_ntask_set_lc(ntask, __func__, 0);                \
        if(xf_ntask_lc_is_first(ntask, __func__)) {         \
            xf_ntask_set_status(ntask, XF_NTASK_FINSHED);   \
        } else {                        \
            xf_ntask_set_status(ntask, XF_NTASK_ENDED);     \
        }                               \
        return;                         \
    } while (0)


typedef void xf_async_t;

#define xf_await(func) func;\
int _async_status = xf_ntask_get_status(task);\
if (_async_status == XF_NTASK_YIELDED || _async_status == XF_NTASK_WAITING) \
{\
    xf_ntask_set_lc(task, __func__, __LINE__);   \
    return;                             \
    case __LINE__:                          \
}



#ifdef __cplusplus
} /* extern "C" */
#endif

/**
 * End of group_xf_task_user_ttask
 * @}
 */

#endif // __XF_NTASK_H__
