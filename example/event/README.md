# event 事件例程

本例程主要展示事件等待

本例程中，总共有三个任务。
ttask负责规定时间发送时间
task1 演示了ctask等待事件1和事件2同时发生的时候才触发
task2 演示了ntask等待事件1或事件2任意一个发生是触发

# 如何使用该例程

1. 安装 [xmake](https://xmake.io/)

2. 使用 xmake 编译本例程（在有 xmake.lua 文件夹运行）

```shell
xmake b hunger
```
3. 使用 xmake 运行本例程（在有 xmake.lua 文件夹运行）

```shell
xmake r hunger
```

# 运行结果

```shell
task1 waiting for event
task2 waiting for event
event send 1
task1 get event
task1 waiting for event
event send 2
task1 get event
task1 waiting for event
event send 3
task2 get event
task2 waiting for event
task1 get event
task1 waiting for event
```

