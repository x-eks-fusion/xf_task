# context task 例程

本例程主要展示有上下文的时候，ctask 会创建三个任务。
一个任务打印一次延时1s。另一个任务打印一次延时2s。第三个任务打印一次延时3s后退出任务。

本例程展示如何使用 ctask 创建任务。

ctask可以创建类似线程的while大循环，但是必须要有 ctask_delay 或者 ctask_queue。否则无法释放cpu。
ctask也可以创建一个函数，运行到结尾后自动删除自己。

本例程使用 ctask 创建三个任务。
第一个任务打印一次后延时 1s。
第二个任务打印一次后延时 2s。
第三个任务打印一次后延时 3s，然后自动删除任务。

# 如何使用该例程

1. 安装 [xmake](https://xmake.io/)

2. 使用 xmake 编译本例程（在有 xmake.lua 文件夹运行）

```shell
xmake b ctask
```
3. 使用 xmake 运行本例程（在有 xmake.lua 文件夹运行）

```shell
xmake r ctask
```

# 运行结果

```shell
task:1
task:2
task:3
task:1
task:2
task:1
D-task_base[:116(xf_task_base_set_state)]: task will be delete
task:1
D-task_base[:138(xf_task_destructor)]: task was delete
task:2
task:1
task:1
task:2
task:1
task:1
task:2
task:1
task:1
...
```
