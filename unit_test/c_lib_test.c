#include "czmq_ipc.h"

#include <stdio.h>
#include <signal.h>
#include "unistd.h"

typedef enum { true = 1, false = 0 } bool;
static bool exit_flag = false;

// 信号处理函数
void SignalHandler(int signal) {
  if (signal == SIGINT) {
    printf("Ctrl+C pressed. Exiting...\n");
    // 在这里执行退出操作
    exit_flag = true;
  }
}

void CallbackFunc(CZmqIpcString topic, CZmqIpcByteArr message) {
  printf("Publisher: [%s]-{%s}\n", topic.string, message.byte_arr);
}

int main() {
  // 注册信号处理函数
  signal(SIGINT, SignalHandler);

  int32_t zmq_ipc = CZmqIpcNew();
  CZmqIpcString subscriber = {sizeof("tcp://localhost:5555"), "tcp://localhost:5555"};
  CZmqIpcString publisher = {sizeof("tcp://localhost:5556"), "tcp://localhost:5556"};
  CZmqIpcString topic = {sizeof("hello") - 1, "hello"};

  CZmqIpcInit(zmq_ipc, CallbackFunc, 2, subscriber, publisher);

  CZmqIpcSubscribe(zmq_ipc, topic);

  while (true) {
    if (exit_flag) {
      break;
    } else {
      sleep(2);
    }
  }

  CZmqIpcDel(zmq_ipc);
}
