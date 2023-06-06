import ctypes
import sys

# 加载共享库
if sys.platform.startswith('win32'):
    zmq_ipc_lib = ctypes.cdll.LoadLibrary('zmq_ipc.dll')
else:
    zmq_ipc_lib = ctypes.cdll.LoadLibrary('zmq_ipc.so')


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

# 创建对象
zmq_ipc_new = zmq_ipc_lib.CZmqIpcNew
zmq_ipc_new.restype = ctypes.c_void_p
zmq_ipc = zmq_ipc_new()


# 定义回调函数
def callback(topic, message):
    # 这里是回调函数的具体实现
    print("Received Topic:", topic.decode())
    print("Received Message:", list(message))


# 将回调函数转换为函数指针
callback_func = CZmqIpcCallbackFunc(callback)

# 初始化对象
subscriber = CZmqIpcString(len("subscriber"), "subscriber".encode())
publisher = CZmqIpcString(len("publisher"), "publisher".encode())
zmq_ipc_lib.CZmqIpcInit(zmq_ipc, callback_func, subscriber, publisher)

# 订阅主题
topic = CZmqIpcString(len("topic"), "topic".encode())
zmq_ipc_lib.CZmqIpcSubscribe(zmq_ipc, topic)

# 发布消息
message_data = b'\x01\x02\x03\x04'
message = CZmqIpcByteArr(len(message_data), ctypes.cast(message_data, ctypes.POINTER(ctypes.c_uint8)))
zmq_ipc_lib.CZmqIpcPublish(zmq_ipc, topic, message)

# 取消订阅
zmq_ipc_lib.CZmqIpcUnsubscribe(zmq_ipc, topic)

# 销毁对象
zmq_ipc_del = zmq_ipc_lib.CZmqIpcDel
zmq_ipc_del.argtypes = [ctypes.c_void_p]
zmq_ipc_del(zmq_ipc)
