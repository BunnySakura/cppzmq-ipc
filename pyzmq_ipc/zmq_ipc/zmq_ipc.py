import zmq
import threading

from typing import Callable
from typing import Union
from typing import Sequence


class ZmqIpc:
    def __init__(self, callback: Callable[[bytes, bytes], None], subscriber: str, publisher: str):
        """
        构造函数

        Args:
            callback: 收到消息时的回调函数
            subscriber: 消息订阅者，消息去向
            publisher: 消息发布者，消息来源
        """
        # 初始化上下文和socket对象
        self.__context = zmq.Context()
        self.__sub_socket = self.__context.socket(zmq.SUB)
        self.__pub_socket = self.__context.socket(zmq.PUB)

        # 创建连接
        self.__sub_socket.connect(publisher)
        self.__pub_socket.connect(subscriber)

        # 配置轮询
        self.__poller = zmq.Poller()  # 轮询器
        self.__poller.register(self.__sub_socket, zmq.POLLIN)  # 注册套接字以进行轮询

        # 启动新线程
        self.__callback = callback  # 回调函数
        self.__thread_running = True  # 退出标志
        self.__thread = threading.Thread(target=self._receive_loop)
        self.__thread.setDaemon(True)
        self.__thread.start()

    def __del__(self):
        self.stop()

    def subscribe(self, topic: Union[int, bytes, str]):
        self.__sub_socket.setsockopt(zmq.SUBSCRIBE, topic)

    def unsubscribe(self, topic: Union[int, bytes, str]):
        self.__sub_socket.setsockopt(zmq.UNSUBSCRIBE, topic)

    def publish(self, msg: Sequence):
        self.__pub_socket.send_multipart(msg)

    def stop(self):
        """
        实例销毁前必须停止线程

        Returns:

        """
        self.__thread_running = False
        self.__thread.join()

    def _receive_loop(self):
        while self.__thread_running:
            socks = dict(self.__poller.poll(timeout=50))  # 毫秒超时
            if socks.get(self.__sub_socket, 0) == zmq.POLLIN:
                try:
                    # 非阻塞接收订阅数据
                    sub_msg = self.__sub_socket.recv_multipart(flags=zmq.NOBLOCK)  # 订阅的消息
                    if self.__callback is not None:
                        self.__callback(sub_msg[0], sub_msg[1])
                except zmq.error.Again:
                    # 如果没有消息则跳过
                    pass
