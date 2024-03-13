#include "zmq.hpp"

#include "zmq_ipc.h"
#include "czmq_ipc.h"

#include <vector>
#include <queue>
#include <memory>

static struct {
  std::vector<std::unique_ptr<ZmqIpc>> zipc_list_;  // IPC对象数组
  std::queue<int32_t> idle_nums_;                   // 空元素下标
} zipc_manager;

int32_t CZmqIpcNew() {
  // 如果数组存在空元素，则在该位置初始化一个IPC对象，否则在尾部初始化一个对象。返回下标。
  if (!zipc_manager.idle_nums_.empty()) {
    int32_t zid = zipc_manager.idle_nums_.front();
    zipc_manager.idle_nums_.pop();
    zipc_manager.zipc_list_.at(zid) = std::make_unique<ZmqIpc>();
    return zid;
  } else {
    size_t zid = zipc_manager.zipc_list_.size();
    zipc_manager.zipc_list_.push_back(std::make_unique<ZmqIpc>());
    return static_cast<int32_t>(zid);
  }
}

void CZmqIpcDel(int32_t zid) {
  zipc_manager.zipc_list_.at(zid) = nullptr;
  zipc_manager.idle_nums_.push(zid);
}

void CZmqIpcInit(int32_t zid,
                 CZmqIpcCallbackFunc callback,
                 size_t threads,
                 CZmqIpcString subscriber,
                 CZmqIpcString publisher) {
  std::string sub(subscriber.string, subscriber.size);
  std::string pub(publisher.string, publisher.size);
  auto cb = [callback](const std::string &topic_str, const std::vector<uint8_t> &message_vec) {
    callback({topic_str.size(), topic_str.data()},
             {message_vec.size(), message_vec.data()});
  };
  try {
    zipc_manager.zipc_list_.at(zid)->Init(cb, threads, sub, pub);
  }
  catch (const int32_t &e) {
    printf("zmq err: %s", zmq_strerror(e));
  }
  catch (const std::exception &e) {
    printf("std err: %s", e.what());
  }
  catch (...) {
    printf("Unknown err!");
  }
}

void CZmqIpcSubscribe(int32_t zid, CZmqIpcString topic) {
  std::string top(topic.string, topic.size);
  zipc_manager.zipc_list_.at(zid)->Subscribe(top);
}

void CZmqIpcUnsubscribe(int32_t zid, CZmqIpcString topic) {
  std::string top(topic.string, topic.size);
  zipc_manager.zipc_list_.at(zid)->Unsubscribe(top);
}

void CZmqIpcPublish(int32_t zid, CZmqIpcString topic, CZmqIpcByteArr message) {
  std::string top(topic.string, topic.size);
  std::vector<uint8_t> msg(message.byte_arr, message.byte_arr + message.size);
  zipc_manager.zipc_list_.at(zid)->Publish(top, msg);
}
