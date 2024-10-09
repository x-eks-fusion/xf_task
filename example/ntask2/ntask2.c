#include "xf_task.h"
#include "port.h"
#include <stdio.h>

xf_ntask_sem_t sem;

static void task1(xf_task_t task)
{
    // 启动无栈协程
    XF_NTASK_BEGIN(task);
    printf("ntask 1 init\n");
    // 无栈协程退出后无法保存上下文
    // 需要使用静态或全局变量
    // 也可以通过申请内存，然后挂在任务对象的钩子指针上
    int *num = (int *)xf_malloc(sizeof(int));
    // ntask 专属的钩子指针，用于获取自定义数据
    xf_ntask_set_hook_ptr(task, num);
    while (1) {
        num = xf_ntask_get_hook_ptr(task);
        *num += 1;
        if (*num == 5) {
            xf_ntask_sem_signal(&sem);
        }

        printf("num:%d\n", *num);
        // 每次执行到这里会跳出函数（上下文不保存）
        // 达到指定时间，则会回到这一行继续执行
        // 这个延时无法被函数调用，只能在任务内使用它
        xf_ntask_delay(1000);
    }
    // 无栈协程结束
    XF_NTASK_END();
}

static void task2(xf_task_t task)
{
    XF_NTASK_BEGIN(task);
    printf("ntask 2 wait sem\n");
    xf_ntask_sem_wait(&sem);
    printf("ntask 2 get sem\n");
    XF_NTASK_END();
}

int main()
{
    // 对接时间戳
    xf_task_tick_init(task_get_tick);
    // 初始化默认任务管理器
    xf_task_manager_default_init(task_on_idle);

    // 初始化无栈协程的信号量
    xf_ntask_sem_init(&sem, 0);

    // 创建任务
    xf_ntask_create_loop(task1, (void *)1, 1, 10);
    // 无栈协程中，循环次数会失效（虽然这里设置了五次，但实际执行到END时，任务会删除）
    xf_ntask_create(task2, (void *)2, 1, 10, 5);

    // 启动任务管理器
    while (1)
    {
        xf_task_manager_run_default();
    }

    return 0;
}
