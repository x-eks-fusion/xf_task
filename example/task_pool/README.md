# 任务池例程

任务池是用于创建一定数量的内存空间。
方便用户后续频繁使用和删除任务。
其删除的任务会被汇总后返回任务池中，可以循环使用。
直到任务池被删除，任务才会彻底的释放内存。

该例程主要创建了两个不同的任务池，ctask 任务池和 ttask 任务池。

虽然总数为3，但是因为任务池里面可以回收。所以利用这些任务池内的任务。
我们每种任务使用了五次。

# 如何使用该例程

1. 安装 [xmake](https://xmake.io/)

2. 使用 xmake 编译本例程（在有 xmake.lua 文件夹运行）

```shell
xmake b task_pool
```

3. 使用 xmake 运行本例程（在有 xmake.lua 文件夹运行）

```shell
xmake r task_pool
```

# 运行结果

```shell
ctask:1
ttask:1
D-task_base[:116(xf_task_base_set_state)]: task will be delete
ctask:2
ttask:2
D-task_base[:116(xf_task_base_set_state)]: task will be delete
ctask:3
ttask:3
D-task_base[:116(xf_task_base_set_state)]: task will be delete
ctask:4
ttask:4
D-task_base[:116(xf_task_base_set_state)]: task will be delete
ctask:5
delete cpool
D-task_base[:116(xf_task_base_set_state)]: task will be delete
D-task_base[:138(xf_task_destructor)]: task was delete
D-task_base[:138(xf_task_destructor)]: task was delete
D-task_base[:138(xf_task_destructor)]: task was delete
ttask:5
delete npool
D-task_base[:116(xf_task_base_set_state)]: task will be delete
D-task_base[:138(xf_task_destructor)]: task was delete
D-task_base[:138(xf_task_destructor)]: task was delete
D-task_base[:138(xf_task_destructor)]: task was delete
```
