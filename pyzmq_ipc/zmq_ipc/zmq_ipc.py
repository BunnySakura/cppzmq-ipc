import zmq
import threading

from typing import Callable
from typing import Union
from typing import Sequence


class ZmqIpc:
    def __init__(self, callback: Callable[[bytes, bytes], None], subscriber: str, publisher: str):
        # 初始化上下文和socket对象
        self._context = zmq.Context()
        self._sub_socket = self._context.socket(zmq.SUB)
        self._pub_socket = self._context.socket(zmq.PUB)

        # 创建连接
        self._sub_socket.connect(publisher)
        self._pub_socket.connect(subscriber)

        # 配置轮询
        self._poller = zmq.Poller()  # 轮询器
        self._poller.register(self._sub_socket, zmq.POLLIN)  # 注册套接字以进行轮询

        # 启动新线程
        self._callback = callback  # 回调函数
        self._thread_running = True  # 退出标志
        self._thread = threading.Thread(target=self._receive_loop)
        self._thread.setDaemon(True)
        self._thread.start()

    def __del__(self):
        self.stop()

    def subscribe(self, topic: Union[int, bytes, str]):
        self._sub_socket.setsockopt(zmq.SUBSCRIBE, topic)

    def unsubscribe(self, topic: Union[int, bytes, str]):
        self._sub_socket.setsockopt(zmq.UNSUBSCRIBE, topic)

    def publish(self, msg: Sequence):
        self._pub_socket.send_multipart(msg)

    def stop(self):
        """
        实例销毁前必须停止线程

        Returns:

        """
        self._thread_running = False
        self._thread.join()

    def _receive_loop(self):
        while self._thread_running:
            socks = dict(self._poller.poll(timeout=50))  # 毫秒超时
            if socks.get(self._sub_socket, 0) == zmq.POLLIN:
                try:
                    # 非阻塞接收订阅数据
                    sub_msg = self._sub_socket.recv_multipart(flags=zmq.NOBLOCK)  # 订阅的消息
                    if self._callback is not None:
                        self._callback(sub_msg[0], sub_msg[1])
                except zmq.error.Again:
                    # 如果没有消息则跳过
                    pass
