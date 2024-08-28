#include "xf_task.h"
#include "port.h"
#include <stdio.h>

xf_task_pool_t npool = NULL;
xf_task_pool_t cpool = NULL;
#define MAX_WORKERS 3

static void ntask(xf_task_t task)
{
    static int num = 0;
    num++;
    // 不断的创建任务（由于任务池的存在，所以不会申请内存，反而会回收已经使用完的任务）
    printf("ntask:%d\n", num);
    if (num < 5)
    {
        xf_task_init_from_pool(npool, ntask, (void *)1, 1);
    }
    else
    {
        xf_task_pool_delete(npool);
        printf("delete npool\n");
    }
    
}

static void ctask(xf_task_t task)
{
    static int num = 0;
    num++;
    // 不断的创建任务（由于任务池的存在，所以不会申请内存，反而会回收已经使用完的任务）
    printf("ctask:%d\n", num);
    if (num < 5)
    {
        xf_ctask_delay(1000);
        xf_task_init_from_pool(cpool, ctask, (void *)1, 1);
    }
    else
    {
        xf_task_pool_delete(cpool);
        printf("delete cpool\n");
    }
    
}

int main()
{
    // 对接时间戳
    xf_task_tick_init(1000, task_get_tick);
    // 对接上下文
    xf_task_context_init(create_context, swap_context);
    // 初始化默认任务管理器
    xf_task_manager_default_init(task_on_idle);

    // 创建 ntask 任务池
    npool = xf_ntask_pool_create(MAX_WORKERS, 1000, 1);

    // 创建 ctask 任务池
    cpool = xf_ctask_pool_create(MAX_WORKERS, 1024*8);

    // 通过任务池创建 ntask 任务
    xf_task_init_from_pool(npool, ntask, (void *)1, 1);
    // 通过任务池创建 ctask 任务
    xf_task_init_from_pool(cpool, ctask, (void *)1, 1);

    // 启动任务管理器
    xf_task_manager_run_default();

    return 0;
}


