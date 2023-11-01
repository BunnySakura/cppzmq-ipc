#include "zmq_ipc.h"

#include <string>
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

void CallbackFunc(const std::string &topic, const std::vector<uint8_t> &message) {
  if (message != std::vector<uint8_t>({'w', 'o', 'r', 'l', 'd'})) {
    // 解析字节数组为ms
    uint64_t receive_time_in_ms;
    std::memcpy(&receive_time_in_ms, message.data(), sizeof(uint64_t));
    // 将时间转换为时间点
    auto receive_time = std::chrono::system_clock::time_point(std::chrono::milliseconds(receive_time_in_ms));
    // 计算时间差
    auto elapsed_time = std::chrono::system_clock::now() - receive_time;
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_time).count();

    printf("Elapse: %ld ms\n", elapsed_ms);
  } else {
    std::string str;
    for (auto &c : message) {
      str += static_cast<char>(c);
    }
    printf("Receive message: %s\n", str.c_str());
  }
}

int main() {
  // 注册信号处理函数
  signal(SIGINT, SignalHandler);

  ZmqIpc zmq_ipc;

  zmq_ipc.Init(CallbackFunc, 2);
  zmq_ipc.Subscribe("hello");

  while (true) {
    if (exit_flag) {
      break;
    } else {
      std::this_thread::sleep_for(std::chrono::seconds(2));
    }
  }
}