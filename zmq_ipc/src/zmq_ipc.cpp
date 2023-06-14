#include "zmq.hpp"

#include "zmq_ipc.h"
#include "make_unique.h"

#include <string>
#include <utility>
#include <vector>
#include <functional>
#include <thread>
#include <mutex>
#include <chrono>

ZmqIpc::ZmqIpc()
    : context_(details::make_unique<zmq::context_t>(1)),
      sub_socket_(details::make_unique<zmq::socket_t>(*context_, zmq::socket_type::sub)),
      pub_socket_(details::make_unique<zmq::socket_t>(*context_, zmq::socket_type::pub)),
      exit_flag_(false) {}

ZmqIpc::~ZmqIpc() {
  // 通知线程停止
  exit_flag_.store(true);
  // 等待线程退出
  if (receiver_thread_.joinable()) {
    receiver_thread_.join();
  }
}

void ZmqIpc::Init(ZmqCallBackFunc callback, const std::string &subscriber, const std::string &publisher) {
  sub_socket_->connect(publisher);
  pub_socket_->connect(subscriber);
  callback_ = std::move(callback);

  // 创建接收消息的线程
  receiver_thread_ = std::thread([&]() {
    zmq::pollitem_t items[] = {{*sub_socket_, 0, ZMQ_POLLIN, 0}};
    while (!exit_flag_) {
      {
        std::unique_lock<std::mutex> lock(sub_socket_mutex_);
        zmq::poll(items, 1, std::chrono::milliseconds(50)); // 轮询器等待可读取事件,毫秒超时
      }
      if (items[0].revents & ZMQ_POLLIN) {
        zmq::message_t topic;
        zmq::message_t message;
        { // 注意避免长时间上锁影响其他线程
          std::unique_lock<std::mutex> lock(sub_socket_mutex_);
          sub_socket_->recv(topic);
          sub_socket_->recv(message);
        }

        // 转换为std::string和std::vector<uint8_t>类型
        std::string topic_str(static_cast<const char *>(topic.data()), topic.size());
        std::vector<uint8_t> message_vec(
            static_cast<uint8_t *>(message.data()),
            static_cast<uint8_t *>(message.data()) + message.size());

        // 调用回调函数处理消息
        try {
          callback_(topic_str, message_vec);
        }
        catch (const std::exception &e) {
          printf("std err: %s", e.what());
        }
        catch (...) {
          printf("Unknown err!");
        }
      }
    }
  });
}

void ZmqIpc::Subscribe(const std::string &topic) {
  std::unique_lock<std::mutex> lock(sub_socket_mutex_);
  sub_socket_->set(zmq::sockopt::subscribe, topic);
}

void ZmqIpc::Unsubscribe(const std::string &topic) {
  std::unique_lock<std::mutex> lock(sub_socket_mutex_);
  sub_socket_->set(zmq::sockopt::unsubscribe, topic);
}

void ZmqIpc::Publish(const std::string &topic, const std::vector<uint8_t> &message) {
  zmq::message_t topic_msg{topic.data(), topic.size()};
  zmq::message_t message_msg{message.data(), message.size()};

  pub_socket_->send(topic_msg, zmq::send_flags::sndmore);
  pub_socket_->send(message_msg, zmq::send_flags::none);
}

ZmqIpc *CZmqIpcNew() {
  return new ZmqIpc();
}

void CZmqIpcDel(ZmqIpc *zmq_ipc) {
  delete zmq_ipc;
}

void CZmqIpcInit(ZmqIpc *zmq_ipc,
                 CZmqIpcCallbackFunc callback,
                 CZmqIpcString subscriber,
                 CZmqIpcString publisher) {
  std::string sub(subscriber.string, subscriber.size);
  std::string pub(publisher.string, publisher.size);
  auto cb = [callback](const std::string &topic_str, const std::vector<uint8_t> &message_vec) {
    callback({topic_str.size(), topic_str.data()},
             {message_vec.size(), message_vec.data()});
  };
  try {
    zmq_ipc->Init(cb, sub, pub);
  }
  catch (const errno_t &e) {
    printf("zmq err: %s", zmq_strerror(e));
  }
  catch (const std::exception &e) {
    printf("std err: %s", e.what());
  }
  catch (...) {
    printf("Unknown err!");
  }
}

void CZmqIpcSubscribe(ZmqIpc *zmq_ipc, CZmqIpcString topic) {
  std::string top(topic.string, topic.size);
  zmq_ipc->Subscribe(top);
}

void CZmqIpcUnsubscribe(ZmqIpc *zmq_ipc, CZmqIpcString topic) {
  std::string top(topic.string, topic.size);
  zmq_ipc->Unsubscribe(top);
}

void CZmqIpcPublish(ZmqIpc *zmq_ipc, CZmqIpcString topic, CZmqIpcByteArr message) {
  std::string top(topic.string, topic.size);
  std::vector<uint8_t> msg(message.byte_arr, message.byte_arr + message.size);
  zmq_ipc->Publish(top, msg);
}