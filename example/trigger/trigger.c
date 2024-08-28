#include "xf_task.h"
#include "port.h"
#include <stdio.h>

xf_task_t *task1;
xf_task_t *task2;

static void trigger(xf_task_t task)
{
    uintptr_t num = (uintptr_t) xf_task_get_arg(task);
    xf_task_trigger(task1);
    xf_task_trigger(task2);
    printf("task:%ld\n", num);
}

static void ctask(xf_task_t task)
{
    uintptr_t num = (uintptr_t) xf_task_get_arg(task);
    xf_ctask_delay(10000);  // 这里由于触发所以并不会延时10s
    printf("task:%ld\n", num);
}

static void ntask(xf_task_t task)
{
    uintptr_t num = (uintptr_t) xf_task_get_arg(task);
    printf("task:%ld\n", num);
}

int main()
{
    // 对接时间戳
    xf_task_tick_init(1000, task_get_tick);
    // 对接上下文
    xf_task_context_init(create_context, swap_context);
    // 初始化默认任务管理器
    xf_task_manager_default_init(task_on_idle);

    // 创建任务
    task1 = xf_ntask_create_loop(ntask, (void *)1, 1, 0);  // 循环时间设置为0，不会通过时间触发
    task2 = xf_ctask_create(ctask, (void *)2, 1, 1024 * 8);
    // 创建循环触发任务
    xf_ntask_create_loop(trigger, (void *)3, 1, 2000);

    // 启动任务管理器
    xf_task_manager_run_default();

    return 0;
}
