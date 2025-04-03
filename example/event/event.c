#include "xf_task.h"
#include "port.h"
#include <stdio.h>

xf_task_manager_t *manager = NULL;
xf_task_event_t event = xf_event_init(event);

#define TASK1_EVENT 1 << 0 | 1 << 2
#define TASK2_EVENT 1 << 1

static void task1(xf_task_t task)
{
    // 获取任务参数
    while (1) {
        printf("task1 waiting for event\n");
        xf_ctask_event_wait_or(event, task, TASK1_EVENT, 0xfffffff);
        printf("task1 get event\n");
    }
}

static void task2(xf_task_t task)
{
    XF_NTASK_BEGIN(task);
    while (1) {
        printf("task2 waiting for event\n");
        xf_ntask_event_wait_or(event, task, TASK1_EVENT, 0xfffffff);
        printf("task2 get event\n");
    }

    XF_NTASK_END();
}

static void task(xf_task_t task)
{
    static int num = 1;
    xf_task_event_sent(&event, num);
    num++;
}


int main()
{
    // 对接上下文
    xf_task_context_init(create_context, swap_context);
    // 对接时间戳
    xf_task_tick_init(task_get_tick);
    // 初始化默认任务管理器
    xf_task_manager_default_init(task_on_idle);

    // 创建任务
    xf_ctask_create(task1, (void *)1, 1, 1024 * 8);
    xf_ntask_create(task2, (void *)1, 1);
    xf_ttask_create_loop(task, (void *)2, 1, 1000);

    // 启动任务管理器
    while (1) {
        xf_task_manager_run_default();
    }

    return 0;
}
