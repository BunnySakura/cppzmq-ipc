#include "zmq_ipc.h"
#include "thread"

static void callback_func(CZmqIpcString zmq_string, CZmqIpcByteArr zmq_bytes) {
  printf("%s\t", zmq_string.string);
  for (int i = 0; i < zmq_bytes.size; i++) {
    printf("%02x ", *(zmq_bytes.byte_arr + i));
  }
  printf("\n");
}

int main() {
  ZmqIpc *zmq_ipc = CZmqIpcNew();
  CZmqIpcInit(zmq_ipc, callback_func,
              {sizeof("tcp://localhost:5555"), "tcp://localhost:5555"},
              {sizeof("tcp://localhost:5556"), "tcp://localhost:5556"});
  CZmqIpcSubscribe(zmq_ipc, {sizeof("world"), "world"});
  uint8_t idx = 0;
  uint8_t msg[2] = {0};
  while (idx != 0xff) {
    msg[0] = idx++;
    CZmqIpcPublish(zmq_ipc,
                   {sizeof("hello"), "hello"},
                   {sizeof(msg), msg});
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  CZmqIpcDel(zmq_ipc);
}
