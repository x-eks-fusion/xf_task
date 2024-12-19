# ntask 例程

本例程主要展示无上下文的 ntask 如何使用。

例程共两个任务。一个任务每隔 1s 打印一次，总共打印五次。另一个任务每隔 2s 打印一次，一直循环。

# 如何使用该例程

1. 安装 [xmake](https://xmake.io/)

2. 使用 xmake 编译本例程（在有 xmake.lua 文件夹运行）

```shell
xmake b ntask
```
3. 使用 xmake 运行本例程（在有 xmake.lua 文件夹运行）

```shell
xmake r ntask
```

# 运行结果

```shell
task:1
task:2
task:1
task:1
task:2
task:1
task:1
D-task_base[:138(xf_task_destructor)]: task was delete
task:2
task:2
...
```
