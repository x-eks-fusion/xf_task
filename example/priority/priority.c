#include "xf_task.h"
#include "port.h"
#include <stdio.h>
#include <unistd.h>

static void task1(xf_task_t task)
{
    uintptr_t num = (uintptr_t) xf_task_get_arg(task);
    usleep(500 * 1000); // 模拟函数运行消耗 500ms 时间
    printf("task:%ld, timeout:%d\n", num, xf_task_get_timeout(task));
}

static void task2(xf_task_t task)
{
    uintptr_t num = (uintptr_t) xf_task_get_arg(task);
    usleep(500 * 1000); // 模拟函数运行消耗 500ms 时间
    printf("task:%ld, timeout:%d\n", num, xf_task_get_timeout(task));
}

static void task3(xf_task_t task)
{
    uintptr_t num = (uintptr_t) xf_task_get_arg(task);
    printf("task:%ld, timeout:%d\n", num, xf_task_get_timeout(task));
}

int main()
{
    // 对接时间戳
    xf_task_tick_init(task_get_tick);
    // 初始化默认任务管理器
    xf_task_manager_default_init(task_on_idle);

    // 创建任务
    xf_ntask_create_loop(task1, (void *)0, 1, 500);
    xf_ntask_create_loop(task2, (void *)1, 1, 500);
    xf_ntask_create_loop(task3, (void *)2, 2, 100);

    // 启动任务管理器
    while (1)
    {
        xf_task_manager_run_default();
    }

    return 0;
}
