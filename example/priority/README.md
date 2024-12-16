# 优先级例程

与抢占式的优先级不同，非抢占优先级需要等待用户任务主动释放 cpu 进度调度器后，才能实现效果。
其效果就是在就绪态任务的选择中如果有多个任务，那么则选取其中优先级最高的。而同等优先级，则选取顺序和进入就绪时间相关，越早进入的越先执行。

本例程主要展示，xf_task 的非抢占优先级的使用。

本例程创建三个任务，其中task1 和 task2 都采用等级为1的优先级,他们本身延时各为500ms,本身也消耗500ms。
task3 优先级为2。
刚开始运行的时候，由于task3定时时间短，所以优先运行。
由于 task1 和 task2 的消耗时间和间隔时间一样，且优先级比 task3 高。就会引起 task1 运行完刚好 task2 好了，然后 task2 运行完 task1 运行好了。
最后导致 task3 永远无法运行。

# 如何使用该例程

1. 安装 [xmake](https://xmake.io/)

2. 使用 xmake 编译本例程（在有 xmake.lua 文件夹运行）

```shell
xmake b priority
```

3. 使用 xmake 运行本例程（在有 xmake.lua 文件夹运行）

```shell
xmake r priority
```

# 运行结果

```shell
task:2, timeout:0
task:2, timeout:0
task:2, timeout:0
task:2, timeout:0
task:0, timeout:0
task:1, timeout:500
task:0, timeout:0
task:1, timeout:0
task:0, timeout:0
task:1, timeout:0
...
```
