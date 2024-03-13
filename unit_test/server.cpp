#include "zmq_ipc.h"

#include <cstring>
#include <csignal>

static bool exit_flag = false;

// 信号处理函数
void SignalHandler(int signal) {
  if (signal == SIGINT) {
    printf("Ctrl+C pressed. Exiting...\n");
    // 在这里执行退出操作
    exit_flag = true;
  }
}

int main() {
  // 注册信号处理函数
  signal(SIGINT, SignalHandler);

  ZmqIpc zmq_ipc;

  zmq_ipc.Init();
  zmq_ipc.Publish("hello", {'w', 'o', 'r', 'l', 'd'});

  while (true) {
    if (exit_flag) {
      break;
    }

    std::vector<uint8_t> data(1024 * 1024 * 100);
    // 获取当前时间
    auto time_point = std::chrono::system_clock::now();
    // 将时间转换为ms
    auto send_time_in_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        time_point.time_since_epoch()
    ).count();
    // 将时间存储为字节数组
    std::memcpy(data.data(), &send_time_in_ms, sizeof(uint64_t));

    zmq_ipc.Publish("hello", data);
    std::this_thread::sleep_for(std::chrono::seconds(2));
  }
}
