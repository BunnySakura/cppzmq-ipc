import time

from pyzmq_ipc import ZmqIpc


def callback(topic: bytes, msg: bytes):
    print(topic, msg)


if __name__ == "__main__":
    zmq_ipc = ZmqIpc(callback, "tcp://localhost:5555", "tcp://localhost:5556")
    zmq_ipc.subscribe(b"hello\0")
    idx = 0
    while idx < 255:
        zmq_ipc.publish([b"world\0", bytes([idx] * 10)])
        idx += 1
        time.sleep(1)
    zmq_ipc.stop()
