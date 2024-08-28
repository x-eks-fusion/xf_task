#include "xf_task.h"
#include "port.h"
#include <stdio.h>


static void task1(xf_task_t task)
{
    // 获取任务参数
    intptr_t num = (intptr_t) xf_task_get_arg(task);
    while (1) {
        printf("task:%ld\n", num);
        // 使用 ctask 专属延时函数
        xf_ctask_delay(num * 1000);
    }
}

static void task2(xf_task_t task)
{
    intptr_t num = (intptr_t) xf_task_get_arg(task);
    printf("task:%ld\n", num);
    xf_ctask_delay(num * 1000);
}


int main()
{
    // 对接上下文
    xf_task_context_init(create_context, swap_context);
    // 对接时间戳
    xf_task_tick_init(1000, task_get_tick);

    // 初始化默认任务管理器
    xf_task_manager_default_init(task_on_idle);

    // 创建任务
    xf_ctask_create(task1, (void *)1, 1, 1024 * 8);
    xf_ctask_create(task1, (void *)2, 1, 1024 * 8);
    xf_ctask_create(task2, (void *)3, 1, 1024 * 8);

    // 启动任务管理器
    xf_task_manager_run_default();

    return 0;
}
