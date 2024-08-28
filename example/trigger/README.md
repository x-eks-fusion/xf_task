# 任务的触发执行（事件执行）

任务不仅能够通过时间进行触发（延时或者定时）
还可以通过触发执行。

该例程主要展示 ctask 和 ntask 被事件触发执行的效果。

该例程创建了三个任务 ctask 和 ntask 任务。还有一个 trigger 任务用于发出触发。
当使用 xf_ntask_create_loop() 创建任务，且创建任务的时间设置为 0 的时候。说明该任务不能被时间触发，只能通过 trigger 触发。

# 如何使用该例程

1. 安装 [xmake](https://xmake.io/)

2. 使用 xmake 编译本例程（在有 xmake.lua 文件夹运行）

```shell
xmake b trigger
```

3. 使用 xmake 运行本例程（在有 xmake.lua 文件夹运行）

```shell
xmake r trigger
```

# 运行结果

```shell
task:3
task:1
task:2
D-task_base[:116(xf_task_base_set_state)]: task will be delete
D-task_base[:138(xf_task_destructor)]: task was delete
task:3
task:1
task:3
task:1
...
```
