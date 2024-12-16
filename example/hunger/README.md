# 任务饥饿度

本例程主要展示如何使用任务的饥饿度。想要使用饥饿度首先 xf_task_config.h 要打开该功能(默认为打开)

```c
#define XF_TASK_HUNGER_ENABLE 1
```

本例程中，总共有三个任务。
任务1 2分别耗时500ms，且500ms一次切换。
理论上占据所有cpu资源。
任务3开启了饥饿度 1000ms。等到饥饿度（1000ms）消耗完，如果还无法执行，则自动向上爬一个优先级，直到优先级足以运行。

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
task:0, timeout:0
task:1, timeout:500
task:0, timeout:0
task:1, timeout:0
task:2, timeout:2000
task:0, timeout:0
task:1, timeout:0
task:0, timeout:1
task:1, timeout:0
task:0, timeout:0
task:2, timeout:2001
task:1, timeout:0
task:0, timeout:0
task:1, timeout:0
task:0, timeout:1
task:1, timeout:0
task:2, timeout:2001
```
