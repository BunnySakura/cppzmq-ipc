from pyzmq_ipc import ZmqIpc

import time
import sys


def callback_print(topic: bytes, msg: bytes):
    print(topic, msg)


def callback_stress(topic: bytes, msg: bytes):
    print("end: \t", time.time_ns())


if __name__ == "__main__":
    # zmq_ipc = ZmqIpc(callback_print, "tcp://localhost:5555", "tcp://localhost:5556")
    # zmq_ipc.subscribe(b"hello\0")
    # idx = 0
    # while idx < 255:
    #     zmq_ipc.publish([b"world\0", bytes([idx] * 1000 * 1000)])
    #     idx += 1
    #     time.sleep(0.01)

    zmq_ipc = ZmqIpc(callback_stress, "tcp://localhost:5555", "tcp://localhost:5556")
    zmq_ipc.subscribe(b"hello")
    idx = 0
    while idx < 255:
        data = bytes([idx] * 1000 * 1000 * 1000)
        print("Size of data: ", sys.getsizeof(data))
        zmq_ipc.publish([b"hello", data])
        print("start: \t", time.time_ns())
        idx += 1
        time.sleep(1)

        zmq_ipc.stop()
