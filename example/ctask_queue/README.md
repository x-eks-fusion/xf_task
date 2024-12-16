# ctask queue 例程

本例程主要展示有上下文的时候，ctask 之间可以通过队列进行任务同步和任务传递信息。

本实例创建了两个任务，一个任务发送一个任务接收。
运行过程中，task1连续发三个数据后被阻塞（队列满）。
task2进行接收，连续接受三次后task2被阻塞（队列空）。依次循环

# 如何使用该例程

1. 安装 [xmake](https://xmake.io/)

2. 使用 xmake 编译本例程（在有 xmake.lua 文件夹运行）

```shell
xmake b ctask_queue
```
3. 使用 xmake 运行本例程（在有 xmake.lua 文件夹运行）

```shell
xmake r ctask_queue
```

# 运行结果

```shell
send success
send success
send success
receive:1
receive:1
receive:1
send success
send success
send success
receive:1
receive:1
receive:1
...
```
