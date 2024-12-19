/**
 * @file ntask.c
 * @author cangyu (sky.kirto@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-12-18
 *
 * @copyright Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

/* ==================== [Includes] ========================================== */

#include "xf_task.h"
#include "port.h"

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

static void task1(xf_task_t task);

/* ==================== [Static Variables] ================================== */

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

int main(void)
{
    // 对接时间戳
    xf_task_tick_init(task_get_tick);
    // 初始化默认任务管理器
    xf_task_manager_default_init(task_on_idle);

    // 创建任务
    xf_ntask_create(task1, (void *)1, 1);

    // 任务管理器，启动
    while (1) {
        xf_task_manager_run_default();
    }

    return 0;
}

/* ==================== [Static Functions] ================================== */

xf_async_t test(xf_task_t task, int a)
{
    XF_NTASK_BEGIN(task);

    printf("task:%d\n", a);
    xf_ntask_delay(task, 1000);
    a = xf_ntask_args_get_int(task, "a");
    printf("task:%d\n", a);

    XF_NTASK_END(task);
}

static void task1(xf_task_t task)
{
    XF_NTASK_BEGIN(task);
    
    int a = 2;
    xf_ntask_args_set_int(task, "a", a);
    while (1) {
        xf_await(test(task, 1));
    }

    XF_NTASK_END(task);
}
