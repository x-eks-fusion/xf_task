/**
 * @file xf_task_port.c
 * @author cangyu (sky.kirto@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-03-05
 *
 * @copyright Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

/* ==================== [Includes] ========================================== */

#include "xf_task_port.h"

/* ==================== [Defines] =========================================== */

#define TAG "xf_port"

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

static void xf_task_reg_init(void);

/* ==================== [Static Variables] ================================== */

static xf_task_clock_t s_clock = NULL;

#if XF_TASK_CONTEXT_IS_ENABLE
static xf_task_create_context_t s_create_context = NULL;
static xf_task_swap_context_t s_swap_context = NULL;
#endif

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */
xf_err_t xf_task_tick_init(xf_task_clock_t clock)
{
    XF_ASSERT(clock, XF_ERR_INVALID_ARG, TAG, "clock function must not be NULL");

    s_clock = clock;

    xf_task_reg_init();

    return XF_OK;

}

xf_task_time_t xf_task_get_ticks(void)
{
    return s_clock();
}

#if XF_TASK_CONTEXT_IS_ENABLE
xf_err_t xf_task_context_init(xf_task_create_context_t create_context, xf_task_swap_context_t swap_context)
{
    XF_ASSERT(create_context, XF_ERR_INVALID_ARG, TAG, "create_context function must not be NULL");
    XF_ASSERT(swap_context, XF_ERR_INVALID_ARG, TAG, "swap_context function must not be NULL");

    s_create_context = create_context;
    s_swap_context = swap_context;

    return XF_OK;
}

void xf_task_context_create(xf_task_manager_t manager, xf_context_func_t context_entry, void *context, void *stack,
                            size_t stack_size)
{
    s_create_context(manager, context_entry, context, stack, stack_size);
}

void xf_task_context_swap(xf_task_manager_t manager, void *old_context, void *new_context)
{
    s_swap_context(manager, old_context, new_context);
}

#endif // XF_TASK_CONTEXT_IS_ENABLE


int32_t xf_task_msec_to_ticks(int32_t msec)
{
    return (msec * XF_TASK_TICKS_FREQUENCY / 1000);
}

int32_t xf_task_ticks_to_msec(int32_t ticks)
{
    return (ticks * 1000 / XF_TASK_TICKS_FREQUENCY);
}

/* ==================== [Static Functions] ================================== */

#define XF_TASK_REG_EXTERN
#include "../task/xf_task_reg.inc"

static void xf_task_reg_init(void)
{
#define XF_TASK_REG_FUNCTION
#include "../task/xf_task_reg.inc"
}

