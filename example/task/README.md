# 综合 task 例程

本例程主要展示有上下文和无上下文的混合使用.

本例程会创建四个任务。
第一个任务，是有上下文，打印后延时1s，无限循环
第二个任务，是有上下文，打印后延时2s，执行一次后退出
第三个任务，是无上下文，无限循环100ms打印
第四个任务，是无上下文，循环200ms打印，执行五次

# 如何使用该例程

1. 安装 [xmake](https://xmake.io/)

2. 使用 xmake 编译本例程（在有 xmake.lua 文件夹运行）

```shell
xmake b task
```

3. 使用 xmake 运行本例程（在有 xmake.lua 文件夹运行）

```shell
xmake r task
```

# 运行结果

```shell
task:1
task:2
D-task_base[:116(xf_task_base_set_state)]: task will be delete
D-task_base[:138(xf_task_destructor)]: task was delete
task:3
task:4
task:3
task:3
task:4
task:3
task:3
task:4
task:3
task:3
task:4
task:3
task:3
task:1
task:4
task:3
D-task_base[:138(xf_task_destructor)]: task was delete
task:3
task:3
task:3
task:3
task:3
task:3
task:3
task:3
task:3
task:1
...
```
