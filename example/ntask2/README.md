# ntask 无栈协程例程

普通的 ntask 任务，必须要写成“开环”。不能像 ctask 那样写一个 while 大循环。对于很多习惯写裸机的人，转向这种纯异步式的使用很多时候不是非常习惯。
于是，在参考了[protothread](https://dunkels.com/adam/pt/)的实现后，我们编写了基于 ntask 的无栈协程。其不但能够使用 while 大循环，也利用其特性实现了 ntask_delay ，方便大家后续的使用。

本例程主要展示，如何在 ntask 的任务上使用我们的无栈协程的方式。以及信号量的用法

本例程创建了两个 ntask ，其中一个 ntask 每隔 1s 计数一次，第五次的时候发送信号。另一个 ntask 一直等待这个信号。等到了就结束运行。

# 如何使用该例程

1. 安装 [xmake](https://xmake.io/)

2. 使用 xmake 编译本例程（在有 xmake.lua 文件夹运行）

```shell
xmake b ntask2
```

3. 使用 xmake 运行本例程（在有 xmake.lua 文件夹运行）

```shell
xmake r ntask2
```

# 运行结果

```shell
ntask 1 init
num:1
ntask 2 wait sem
num:2
num:3
num:4
num:5
ntask 2 get sem
D-task_base[:116(xf_task_base_set_state)]: task will be delete
D-task_base[:138(xf_task_destructor)]: task was delete
num:6
num:7
num:8
num:9
num:10
num:11
num:12
```
