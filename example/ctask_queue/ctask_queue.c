#include "xf_task.h"
#include "port.h"
#include <stdio.h>

xf_ctask_queue_t *cqueue = NULL;

static void send_task(xf_task_t task)
{
    intptr_t num = (intptr_t) xf_task_get_arg(task);
    while (1) {
        if (xf_ctask_queue_send(cqueue, &num, 1000) == XF_OK) {
            printf("send success\n");
        } else {
            printf("timeout! queue full\n");
        }
    }
}

static void receive_task(xf_task_t task)
{
    intptr_t buffer = 0;
    while (1) {
        if (xf_ctask_queue_receive(cqueue, &buffer, 1000) == XF_OK) {
            printf("receive:%d\n", (int)buffer);
        } else {
            printf("timeout! queue empty\n");
        }
    }
}

int main()
{
    // 对接上下文
    xf_task_context_init(create_context, swap_context);
    // 对接时间戳
    xf_task_tick_init(1000, task_get_tick);

    // 初始化默认任务管理器
    xf_task_manager_default_init(task_on_idle);

    // 创建消息队列
    cqueue = xf_ctask_queue_create(sizeof(intptr_t), 3);

    // 创建任务
    xf_ctask_create(send_task, (void *)1, 1, 1024 * 8);
    xf_ctask_create(receive_task, (void *)2, 1, 1024 * 8);

    // 启动任务管理器
    xf_task_manager_run_default();

    return 0;
}
