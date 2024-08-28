/**
 * @file port.c
 * @author cangyu (sky.kirto@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-03-05
 *
 * @copyright Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

/* ==================== [Includes] ========================================== */
#include "port.h"
#include <time.h>
#include <unistd.h>

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

#if !XF_TASK_CONTEXT_DISABLE && !USE_GNU_UC

typedef void *fcontext_t;
typedef struct {
    fcontext_t  fctx;
    void *data;
} transfer_t;

typedef struct {
    xf_task_manager_t manager;
    fcontext_t *from;
    fcontext_t *to;
} task_jump_t;

extern transfer_t jump_fcontext(fcontext_t const to, void *vp);
extern fcontext_t make_fcontext(void *sp, size_t size, void (* fn)(transfer_t));

/* ==================== [Static Prototypes] ================================= */

static void fcontext(transfer_t transfer);

/* ==================== [Static Variables] ================================== */

static xf_context_func_t s_context_func = NULL;

#endif

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

#if !XF_TASK_CONTEXT_DISABLE && USE_GNU_UC
void create_context(xf_task_manager_t manager, xf_context_func_t context_entry, void *context, void *stack,
                    size_t stack_size)
{
    xf_task_context_t *uc = (xf_task_context_t *)context;

    // 初始化上下文
    getcontext(uc);
    uc->uc_stack.ss_sp = stack;
    uc->uc_stack.ss_size = stack_size;
    uc->uc_link = NULL;

    // 设置上下文的执行函数
    makecontext(uc, (void (*)(void))context_entry, 1, manager);
}

void swap_context(xf_task_manager_t manager, void *old_context, void *new_context)
{
    xf_task_context_t *now_context = old_context;
    xf_task_context_t *jump_context = new_context;
    swapcontext(now_context, jump_context);
}

#elif !XF_TASK_CONTEXT_DISABLE && !USE_GNU_UC

void create_context(xf_task_manager_t manager, xf_context_func_t context_entry, void *context, void *stack,
                    size_t stack_size)
{
    fcontext_t *fc = (fcontext_t *)context;
    stack = (void *)((char *)stack + stack_size);
    *fc = make_fcontext(stack, stack_size, fcontext);
    s_context_func = context_entry;
}

void swap_context(xf_task_manager_t manager, void *old_context, void *new_context)
{
    static task_jump_t jump;
    jump.manager = manager;
    jump.from = old_context;
    jump.to = new_context;
    transfer_t t = jump_fcontext(*jump.to, (void *)&jump);
    task_jump_t *ret = (task_jump_t *)t.data;
    *ret->from = t.fctx;
}
#endif


xf_task_time_t task_get_tick(void)
{
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    return (long)(tp.tv_sec * 1000 + tp.tv_nsec / 1000000);
}

void task_on_idle(unsigned long int max_idle_ms)
{
    usleep(max_idle_ms);
}

/* ==================== [Static Functions] ================================== */
#if !XF_TASK_CONTEXT_DISABLE && !USE_GNU_UC
static void fcontext(transfer_t arg)
{
    task_jump_t *jump = (task_jump_t *)arg.data;
    *jump->from = arg.fctx;
    s_context_func(jump->manager);
}

#endif // !XF_TASK_CONTEXT_DISABLE
