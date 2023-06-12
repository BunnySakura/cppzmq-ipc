import ctypes
import sys

# 加载共享库
if sys.platform.startswith('win32'):
    zmq_ipc_lib = ctypes.cdll.LoadLibrary('./zmq_ipc.dll')
else:
    zmq_ipc_lib = ctypes.cdll.LoadLibrary('./zmq_ipc.so')


class CZmqIpcByteArr(ctypes.Structure):
    _fields_ = [
        ('size', ctypes.c_size_t),
        ('byte_arr', ctypes.POINTER(ctypes.c_uint8))
    ]


class CZmqIpcString(ctypes.Structure):
    _fields_ = [
        ('size', ctypes.c_size_t),
        ('string', ctypes.c_char_p)
    ]


class ZmqIpc:
    def __init__(self):
        self.zmq_ipc = zmq_ipc_lib.CZmqIpcNew()

        # 指定函数参数和返回类型
        zmq_ipc_lib.CZmqIpcNew.restype = ctypes.c_void_p
        zmq_ipc_lib.CZmqIpcNew.argtypes = []

        zmq_ipc_lib.CZmqIpcDel.restype = None
        zmq_ipc_lib.CZmqIpcDel.argtypes = [ctypes.c_void_p]

        zmq_ipc_lib.CZmqIpcInit.restype = None
        zmq_ipc_lib.CZmqIpcInit.argtypes = [ctypes.c_void_p, ctypes.CFUNCTYPE(None, CZmqIpcString, CZmqIpcByteArr),
                                            CZmqIpcString, CZmqIpcString]

        zmq_ipc_lib.CZmqIpcSubscribe.restype = None
        zmq_ipc_lib.CZmqIpcSubscribe.argtypes = [ctypes.c_void_p, CZmqIpcString]

        zmq_ipc_lib.CZmqIpcUnsubscribe.restype = None
        zmq_ipc_lib.CZmqIpcUnsubscribe.argtypes = [ctypes.c_void_p, CZmqIpcString]

        zmq_ipc_lib.CZmqIpcPublish.restype = None
        zmq_ipc_lib.CZmqIpcPublish.argtypes = [ctypes.c_void_p, CZmqIpcString, CZmqIpcByteArr]

    def __del__(self):
        zmq_ipc_lib.CZmqIpcDel(self.zmq_ipc)

    def init(self, callback, subscriber, publisher):
        # 将回调函数转换为函数指针
        callback_func = ctypes.CFUNCTYPE(None, CZmqIpcString, CZmqIpcByteArr)(callback)

        # 将字符串转换为字节串
        subscriber_bytes = subscriber.encode()
        publisher_bytes = publisher.encode()

        # 创建结构体变量
        subscriber_str = CZmqIpcString(len(subscriber_bytes), subscriber_bytes)
        publisher_str = CZmqIpcString(len(publisher_bytes), publisher_bytes)

        zmq_ipc_lib.CZmqIpcInit(self.zmq_ipc, callback_func, subscriber_str, publisher_str)

    def subscribe(self, topic):
        topic_str = CZmqIpcString(len(topic), topic.encode())
        zmq_ipc_lib.CZmqIpcSubscribe(self.zmq_ipc, topic_str)

    def unsubscribe(self, topic):
        topic_str = CZmqIpcString(len(topic), topic.encode())
        zmq_ipc_lib.CZmqIpcUnsubscribe(self.zmq_ipc, topic_str)

    def publish(self, topic, message):
        topic_str = CZmqIpcString(len(topic), topic.encode())
        message_bytes = CZmqIpcByteArr(len(message), ctypes.cast(message.encode(), ctypes.POINTER(ctypes.c_uint8)))
        zmq_ipc_lib.CZmqIpcPublish(self.zmq_ipc, topic_str, message_bytes)


def callback(topic, message):
    # 转换 CZmqIpcString 到字符串
    topic_str = ctypes.cast(topic.string, ctypes.POINTER(ctypes.c_char_p * topic.size)).contents

    # 转换 CZmqIpcByteArr 到字节数组
    message_bytes = ctypes.cast(message.byte_arr, ctypes.POINTER(ctypes.c_uint8 * message.size)).contents

    print("Subscriber:", topic_str)
    print("Message:", message_bytes)


if __name__ == "__main__":
    zmq_ipc = ZmqIpc()
    zmq_ipc.init(callback, "tcp://localhost:5555", "tcp://localhost:5556")
    zmq_ipc.subscribe("hello")
    zmq_ipc.publish("hello", "Hello, World!")
    zmq_ipc.unsubscribe("hello")
