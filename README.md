# cppzmq-ipc

一个基于[ZeroMQ]实现的订阅发布机制IPC通信库。

## 介绍

### 1. zmq_ipc

封装发布和订阅方法，提供一个接口类，通过回调函数实现自定义收到订阅消息时的操作。

### 2. zmq_proxy

通信代理，后台运行将所有发布的消息广播给订阅方。

## 构建

- 使用vcpkg安装对应平台的`cppzmq`即可正常构建。
- 使用cmake进行构建

[ZeroMQ]: https://zeromq.org/ "ZeroMQ"
