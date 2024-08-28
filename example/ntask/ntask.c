#include "xf_task.h"
#include "port.h"
#include <stdio.h>

static void task(xf_task_t task)
{
    uintptr_t num = (uintptr_t) xf_task_get_arg(task);
    printf("task:%ld\n", num);
}

int main()
{
    // 对接时间戳
    xf_task_tick_init(1000, task_get_tick);

    // 初始化默认任务管理器
    xf_task_manager_default_init(task_on_idle);

    // 创建任务
    xf_ntask_create(task, (void *)1, 1, 1000, 5);
    // 创建无限循环任务
    xf_ntask_create_loop(task, (void *)2, 1, 2000);

    // 启动任务管理器
    xf_task_manager_run_default();

    return 0;
}
