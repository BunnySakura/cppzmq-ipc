#include "zmq_ipc.h"
#include "thread"

// 获取代码块开始时间
auto start_time = std::chrono::steady_clock::now();
// 获取代码块结束时间
auto end_time = std::chrono::steady_clock::now();

static void callback_func(CZmqIpcString zmq_string, CZmqIpcByteArr zmq_bytes) {
  printf("%s\t", zmq_string.string);
  for (int i = 0; i < zmq_bytes.size; i++) {
    printf("%02x ", *(zmq_bytes.byte_arr + i));
  }
  printf("\n");
}

static void callback_stress(CZmqIpcString zmq_string, CZmqIpcByteArr zmq_bytes) {
  end_time = std::chrono::steady_clock::now();
  // 计算运行耗时
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
  printf("Elapse: %lld ms\n", duration.count());
}

int main() {
  ZmqIpc *zmq_ipc = CZmqIpcNew();
#if 0
  CZmqIpcInit(zmq_ipc, callback_func, 1,
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
#else
  CZmqIpcInit(zmq_ipc, callback_stress, 1,
              {sizeof("tcp://localhost:5555"), "tcp://localhost:5555"},
              {sizeof("tcp://localhost:5556"), "tcp://localhost:5556"});
  CZmqIpcSubscribe(zmq_ipc, {sizeof("hello"), "hello"});
  uint8_t idx = 0;
  auto *data = new uint8_t[1024 * 1024 * 1024];
  while (idx != 0x10) {
    data[0] = idx++;
    CZmqIpcPublish(zmq_ipc,
                   {sizeof("hello"), "hello"},
                   {1024 * 1024 * 1024, data});
    start_time = std::chrono::steady_clock::now();
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  delete[]data;
#endif

  CZmqIpcDel(zmq_ipc);
}
