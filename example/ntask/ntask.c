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

xf_async_t test(xf_task_t task)
{
    XF_NTASK_BEGIN(task, int, a);
    a = -1;
    xf_ntask_delay(1000);
    printf("task:%d\n", a);

    XF_NTASK_END();
}

static void task1(xf_task_t task)
{
    XF_NTASK_BEGIN(task, int, a);
    a = 0;
    while (1) {
        a++;
        printf("a:%d\n", a);
        xf_await(test(task));
        printf("a:%d\n", a);
    }

    XF_NTASK_END();
}
