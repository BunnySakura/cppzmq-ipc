import ctypes
import sys
import time


class ZmqIpc:
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

    def __init__(self):
        self.lib = self._load_library()
        self._configure_functions()
        self.zmq_ipc = self.lib.CZmqIpcNew()

    def __del__(self):
        self.lib.CZmqIpcDel.argtypes = [ctypes.c_void_p]
        self.lib.CZmqIpcDel(self.zmq_ipc)

    def _load_library(self):
        if sys.platform.startswith('win32'):
            return ctypes.cdll.LoadLibrary('./zmq_ipc.dll')
        else:
            return ctypes.cdll.LoadLibrary('./zmq_ipc.so')

    def _configure_functions(self):
        self.CZmqIpcCallbackFunc = ctypes.CFUNCTYPE(None, self.CZmqIpcString, self.CZmqIpcByteArr)
        self.lib.CZmqIpcNew.argtypes = None
        self.lib.CZmqIpcNew.restype = ctypes.c_void_p
        self.lib.CZmqIpcDel.argtypes = [ctypes.c_void_p]
        self.lib.CZmqIpcDel.restype = None
        self.lib.CZmqIpcInit.argtypes = [ctypes.c_void_p, self.CZmqIpcCallbackFunc,
                                         self.CZmqIpcString, self.CZmqIpcString]
        self.lib.CZmqIpcInit.restype = None
        self.lib.CZmqIpcSubscribe.argtypes = [ctypes.c_void_p, self.CZmqIpcString]
        self.lib.CZmqIpcSubscribe.restype = None
        self.lib.CZmqIpcUnsubscribe.argtypes = [ctypes.c_void_p, self.CZmqIpcString]
        self.lib.CZmqIpcUnsubscribe.restype = None
        self.lib.CZmqIpcPublish.argtypes = [ctypes.c_void_p, self.CZmqIpcString, self.CZmqIpcByteArr]
        self.lib.CZmqIpcPublish.restype = None

    def init(self, callback, subscriber: bytes, publisher: bytes):
        subscriber_str = self.CZmqIpcString(len(subscriber), subscriber)
        publisher_str = self.CZmqIpcString(len(publisher), publisher)
        self.lib.CZmqIpcInit(self.zmq_ipc, self.CZmqIpcCallbackFunc(callback), subscriber_str, publisher_str)

    def subscribe(self, topic: bytes):
        topic_str = self.CZmqIpcString(len(topic), topic)
        self.lib.CZmqIpcSubscribe(self.zmq_ipc, topic_str)

    def unsubscribe(self, topic: bytes):
        topic_str = self.CZmqIpcString(len(topic), topic)
        self.lib.CZmqIpcUnsubscribe(self.zmq_ipc, topic_str)

    def publish(self, topic: bytes, message: bytes):
        topic_str = self.CZmqIpcString(len(topic), topic)
        message_bytes = self.CZmqIpcByteArr(len(message), ctypes.cast(message, ctypes.POINTER(ctypes.c_uint8)))
        self.lib.CZmqIpcPublish(self.zmq_ipc, topic_str, message_bytes)


def callback(topic, message):
    topic_str = ctypes.cast(topic.string, ctypes.POINTER(ctypes.c_char_p * topic.size)).contents
    message_bytes = bytes(message.byte_arr[:message.size])
    print("Subscriber:", topic_str.decode())
    print("Message:", message_bytes.decode())


if __name__ == "__main__":
    zmq_ipc = ZmqIpc()
    zmq_ipc.init(callback, b"tcp://localhost:5555", b"tcp://localhost:5556")
    zmq_ipc.subscribe(b"hello\0")
    while True:
        zmq_ipc.publish(b"world\0", b"Hello, World!")
        time.sleep(1)
    # zmq_ipc.unsubscribe(b"hello")
