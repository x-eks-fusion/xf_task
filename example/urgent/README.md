# 紧急任务例程

当一个任务需要紧急执行的时候，我们可以设置其为紧急任务。

本例程主要展示如何设置任务为紧急任务。

本例程创建三个任务，其中task1 和 task2 都采用等级为1的优先级,他们本身延时各为500ms,本身也消耗500ms。
task3 优先级为2。
刚开始运行的时候，原本因为 task3 定时时间短，所以优先运行，但是因为 task1 被设置为了紧急任务，所以 task1 会优先运行。
由于 task1 和 task2 的消耗时间和间隔时间一样，且优先级比 task3 高。就会引起 task1 运行完刚好 task2 好了，然后 task2 运行完 task1 运行好了。
最后导致 task3 永远无法运行。

# 如何使用该例程

1. 安装 [xmake](https://xmake.io/)

2. 使用 xmake 编译本例程（在有 xmake.lua 文件夹运行）

```shell
xmake b urgent
```

3. 使用 xmake 运行本例程（在有 xmake.lua 文件夹运行）

```shell
xmake r urgent
```

# 运行结果

```shell
task:0, timeout:-500
task:1, timeout:1
task:0, timeout:0
task:1, timeout:0
task:0, timeout:0
task:1, timeout:1
task:0, timeout:0
task:1, timeout:0
task:0, timeout:0
task:1, timeout:0
...
```
