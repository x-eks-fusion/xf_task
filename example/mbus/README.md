# mbus 例程

本例程展示如何通过发布订阅机制在任务间通信

本例程在主函数中订阅了指定的 topic 。然后让一个任务向 topic 定时发布消息。此时订阅回调会做出相应的响应。当然，如果需要同步发送功能。则必须创建一个任务用于定期执行异步的处理函数。

# 如何使用该例程

1. 安装 [xmake](https://xmake.io/)

2. 使用 xmake 编译本例程（在有 xmake.lua 文件夹运行）

```shell
xmake b mbus
```
3. 使用 xmake 运行本例程（在有 xmake.lua 文件夹运行）

```shell
xmake r mbus
```

# 运行结果

这里虽然是 data:1 发送先于 data:2。但是因为 data:2 是同步的发送，处理更加及时。而 data:1 是异步发送，不会阻塞任务。所以 data:2 显示先于 data:1
```shell
task:1
data:2
data:1
task:1
data:2
data:1
task:1
data:2
data:1
task:1
data:2
data:1
task:1
data:2
data:1
...
```
