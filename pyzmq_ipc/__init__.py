import ctypes
import sys

# 加载共享库
if sys.platform.startswith('win32'):
    zmq_ipc_lib = ctypes.cdll.LoadLibrary('./zmq_ipc.dll')
else:
    zmq_ipc_lib = ctypes.cdll.LoadLibrary('./zmq_ipc.so')


# 定义结构体类型
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


CZmqIpcCallbackFunc = ctypes.CFUNCTYPE(None, CZmqIpcString, CZmqIpcByteArr)

# 指定函数参数和返回类型
zmq_ipc_lib.CZmqIpcNew.restype = ctypes.c_void_p
zmq_ipc_lib.CZmqIpcNew.argtypes = []

zmq_ipc_lib.CZmqIpcDel.restype = None
zmq_ipc_lib.CZmqIpcDel.argtypes = [ctypes.c_void_p]

zmq_ipc_lib.CZmqIpcInit.restype = None
zmq_ipc_lib.CZmqIpcInit.argtypes = [ctypes.c_void_p, CZmqIpcCallbackFunc, CZmqIpcString, CZmqIpcString]

zmq_ipc_lib.CZmqIpcSubscribe.restype = None
zmq_ipc_lib.CZmqIpcSubscribe.argtypes = [ctypes.c_void_p, CZmqIpcString]

zmq_ipc_lib.CZmqIpcUnsubscribe.restype = None
zmq_ipc_lib.CZmqIpcUnsubscribe.argtypes = [ctypes.c_void_p, CZmqIpcString]

zmq_ipc_lib.CZmqIpcPublish.restype = None
zmq_ipc_lib.CZmqIpcPublish.argtypes = [ctypes.c_void_p, CZmqIpcString, CZmqIpcByteArr]


# 定义回调函数
def callback(topic, message):
    # 转换 CZmqIpcString 到字符串
    topic_str = ctypes.cast(topic.string, ctypes.POINTER(ctypes.c_char_p * topic.size)).contents

    # 转换 CZmqIpcByteArr 到字节数组
    message_bytes = ctypes.cast(message.byte_arr, ctypes.POINTER(ctypes.c_uint8 * message.size)).contents

    print("Subscriber:", topic_str)
    print("Message:", message_bytes)


if __name__ == "__main__":
    # 创建对象
    zmq_ipc = zmq_ipc_lib.CZmqIpcNew()

    # 将回调函数转换为函数指针
    callback_func = CZmqIpcCallbackFunc(callback)

    # 初始化对象
    # 创建字符串
    subscriber_str = "tcp://localhost:5555"
    publisher_str = "tcp://localhost:5556"

    # 将字符串转换为字节串
    subscriber_bytes = subscriber_str.encode()
    publisher_bytes = publisher_str.encode()

    # 创建结构体变量
    subscriber = CZmqIpcString(len(subscriber_bytes), subscriber_bytes)
    publisher = CZmqIpcString(len(publisher_bytes), publisher_bytes)
    zmq_ipc_lib.CZmqIpcInit(zmq_ipc, callback_func, subscriber, publisher)

    # 订阅主题
    topic = CZmqIpcString(len("hello"), "hello".encode())
    zmq_ipc_lib.CZmqIpcSubscribe(zmq_ipc, topic)

    # 发布消息
    topic = CZmqIpcString(len("world"), "world".encode())
    message_data = b'\x01\x02\x03\x04'
    message = CZmqIpcByteArr(len(message_data), ctypes.cast(message_data, ctypes.POINTER(ctypes.c_uint8)))
    zmq_ipc_lib.CZmqIpcPublish(zmq_ipc, topic, message)

    # 取消订阅
    zmq_ipc_lib.CZmqIpcUnsubscribe(zmq_ipc, topic)

    # 销毁对象
    zmq_ipc_lib.CZmqIpcDel(zmq_ipc)
