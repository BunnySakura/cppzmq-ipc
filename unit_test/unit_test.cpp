#include "zmq_ipc.h"
#include "thread"

static void callback_func(CZmqIpcString zmq_string, CZmqIpcByteArr zmq_bytes) {
  printf("%s\t", zmq_string.string);
  for (int i = 0; i < zmq_bytes.size; i++) {
    printf("%02x ", *(zmq_bytes.byte_arr + i));
  }
  printf("\n");
}

static void callback_stress(CZmqIpcString zmq_string, CZmqIpcByteArr zmq_bytes) {
  // 解析字节数组为秒数
  uint64_t receive_time_in_seconds;
  std::memcpy(&receive_time_in_seconds, zmq_bytes.byte_arr, sizeof(uint64_t));
  // 将秒数转换为时间点
  auto receive_time = std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(receive_time_in_seconds));
  // 计算时间差，单位为秒
  auto elapsed_time = std::chrono::system_clock::now() - receive_time;
  double elapsed_seconds = std::chrono::duration<double>(elapsed_time).count();

  printf("Elapse: %f s\n", elapsed_seconds);
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
  CZmqIpcInit(zmq_ipc, callback_stress, 8,
              {sizeof("tcp://localhost:5555"), "tcp://localhost:5555"},
              {sizeof("tcp://localhost:5556"), "tcp://localhost:5556"});
  CZmqIpcSubscribe(zmq_ipc, {sizeof("hello"), "hello"});
  auto *data = new uint8_t[1024 * 1024 * 100];
  for (uint8_t idx = 0; idx < 10; idx++) {
    // 获取当前时间
    auto send_time = std::chrono::system_clock::now();
    // 将时间转换为秒数
    auto send_time_in_seconds = std::chrono::duration_cast<std::chrono::seconds>(send_time.time_since_epoch()).count();
    // 将秒数存储为字节数组
    std::memcpy(data, &send_time_in_seconds, sizeof(uint64_t));

    CZmqIpcPublish(zmq_ipc,
                   {sizeof("hello"), "hello"},
                   {sizeof(data), data});
    std::this_thread::sleep_for(std::chrono::seconds(1));
    printf("%03d -> ", idx);
  }
  delete[]data;
#endif

  CZmqIpcDel(zmq_ipc);
}
