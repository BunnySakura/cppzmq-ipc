#ifndef ZMQ_WRAPPER_H
#define ZMQ_WRAPPER_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>

// 前向声明，消除头文件依赖
namespace zmq {
class context_t;
class socket_t;
}

class ZmqIpc {
 private:
  ////////////////////////////////////////////////////////////////////////////////
  // 使用前向声明的变量必须是指针
  ////////////////////////////////////////////////////////////////////////////////
  std::unique_ptr<zmq::context_t> context_;   // 创建 ZeroMQ 上下文
  std::unique_ptr<zmq::socket_t> sub_socket_; // 订阅socket用来接收发布者发布的消息，绑定到订阅端口
  std::unique_ptr<zmq::socket_t> pub_socket_; // 发布socket用来将消息发送给订阅者，绑定到发布端口
  std::thread receiver_thread_;               // 处理订阅数据的线程对象
  std::atomic<bool> exit_flag_;               // 通知线程退出的原子操作
  std::mutex sub_socket_mutex_;               // 用于保护sub_socket_的访问，zmq::socket_t非线程安全

  // 回调函数类型别名
  using CallBackFunc = std::function<void(const std::string &, const std::vector<uint8_t> &)>;
  CallBackFunc callback_; // 接收到订阅数据时，用于处理数据的回调函数

 public:
  ZmqIpc();
  virtual ~ZmqIpc();

  void Init(CallBackFunc callback,
            const std::string &subscriber = "tcp://*:5555",
            const std::string &publisher = "tcp://*:5556");

  virtual void Subscribe(const std::string &topic);
  virtual void Unsubscribe(const std::string &topic);

  void Publish(const std::string &topic, const std::vector<uint8_t> &message);
};

extern "C" {
#ifdef WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

typedef struct {
  const size_t size;
  const uint8_t *byte_arr;
} CZmqIpcByteArr;

typedef struct {
  const size_t size;
  const char *string;
} CZmqIpcString;

typedef void (*CZmqIpcCallbackFunc)(CZmqIpcString, CZmqIpcByteArr);

// 创建对象
DLL_EXPORT ZmqIpc *CZmqIpcNew() {
  return new ZmqIpc;
}

// 销毁对象
DLL_EXPORT void CZmqIpcDel(ZmqIpc *zmq_ipc) {
  delete zmq_ipc;
}

// 实现方法
DLL_EXPORT void CZmqIpcInit(ZmqIpc *zmq_ipc,
                            CZmqIpcCallbackFunc callback,
                            CZmqIpcString subscriber,
                            CZmqIpcString publisher) {
  std::string sub(subscriber.string, subscriber.size);
  std::string pub(publisher.string, publisher.size);
  auto cb = [&callback](const std::string &topic_str, const std::vector<uint8_t> &message_vec) {
    callback({topic_str.size(), topic_str.data()},
             {message_vec.size(), message_vec.data()});
  };
  zmq_ipc->Init(cb, sub, pub);
}

DLL_EXPORT void CZmqIpcSubscribe(ZmqIpc *zmq_ipc, CZmqIpcString topic) {
  std::string top(topic.string, topic.size);
  zmq_ipc->Subscribe(top);
}

DLL_EXPORT void CZmqIpcUnsubscribe(ZmqIpc *zmq_ipc, CZmqIpcString topic) {
  std::string top(topic.string, topic.size);
  zmq_ipc->Unsubscribe(top);
}

DLL_EXPORT void CZmqIpcPublish(ZmqIpc *zmq_ipc, CZmqIpcString topic, CZmqIpcByteArr message) {
  std::string top(topic.string, topic.size);
  std::vector<uint8_t> msg(message.byte_arr, message.byte_arr + message.size);
  zmq_ipc->Publish(top, msg);
}
}

#endif // ZMQ_WRAPPER_H
