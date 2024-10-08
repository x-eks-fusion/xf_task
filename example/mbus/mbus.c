#include "xf_task.h"
#include "port.h"
#include <stdio.h>

// 随便定义一个topic id
#define TOPIC_ID 1

/**
 * @brief 用于测试发布的ntask，定时发布一些数据
 *
 * @param task 任务对象
 */
static void task_pub(xf_task_t task)
{
    int num = 1;
    printf("task:%d\n", num);
    // 异步发送（慢，但是不会阻塞本任务）
    xf_task_mbus_pub_async(TOPIC_ID, &num);
    num++;
    // 同步发送（快，但是会阻塞本任务）
    xf_task_mbus_pub_sync(TOPIC_ID, &num);
}

static void mbus_handle(xf_task_t task)
{
    xf_task_mbus_handle();
}

/**
 * @brief 订阅回调函数，用于处理用户发布的消息
 *
 * @param data 用户发布的消息
 * @param user_data 创建订阅时候的用户自定义参数
 */
void bus_cb(const void *const data, void *user_data)
{
    int num = *(int *)data;
    printf("data:%d\n", num);
}

int main()
{
    // 定时器对接
    xf_task_tick_init(task_get_tick);

    // 任务管理器初始化
    xf_task_manager_default_init(task_on_idle);

    // 创建协作式任务
    xf_ntask_create_loop(task_pub, (void *)1, 1, 2000);
    // 绑定topic到任务管理器上
    xf_task_mbus_reg_topic(TOPIC_ID, sizeof(int));
    // 订阅这个topic，设置处理topic的回调
    xf_task_mbus_sub(TOPIC_ID, bus_cb, NULL);

    xf_ntask_create_loop(mbus_handle, NULL, 0, 10);
    // 任务管理器，启动
    xf_task_manager_run_default();

    return 0;
}
