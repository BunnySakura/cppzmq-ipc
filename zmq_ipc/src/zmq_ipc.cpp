#include "zmq.hpp"

#include "zmq_ipc.h"
#include "make_unique.h"

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <thread>
#include <mutex>
#include <chrono>

ZmqIpc::ZmqIpc()
    : context_(details::make_unique<zmq::context_t>(1)),
      sub_socket_(details::make_unique<zmq::socket_t>(*context_, zmq::socket_type::sub)),
      pub_socket_(details::make_unique<zmq::socket_t>(*context_, zmq::socket_type::pub)),
      exit_flag_(false) {}

ZmqIpc::~ZmqIpc()
{
    // 通知线程停止
    exit_flag_.store(true);
    // 等待线程退出
    if (receiver_thread_.joinable())
    {
        receiver_thread_.join();
    }
}

void ZmqIpc::Init(CallBackFunc callback)
{
    pub_socket_->connect("tcp://localhost:5555");
    sub_socket_->connect("tcp://localhost:5556");
    callback_ = callback;

    // 创建接收消息的线程
    receiver_thread_ = std::thread([&]()
                                   {
        zmq::pollitem_t items[] = {{*(sub_socket_.get()), 0, ZMQ_POLLIN, 0}};
        while (!exit_flag_)
        {
            {
                std::unique_lock<std::mutex> lock(sub_socket_mutex_);
                zmq::poll(items, 1, std::chrono::milliseconds(50)); // 轮询器等待可读取事件,毫秒超时
            }
            if (items[0].revents & ZMQ_POLLIN)
            {
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
                callback_(topic_str, message_vec);
            }
        } });
}

void ZmqIpc::Subscribe(const std::string &topic)
{
    std::unique_lock<std::mutex> lock(sub_socket_mutex_);
    sub_socket_->set(zmq::sockopt::subscribe, topic);
}

void ZmqIpc::Unsubscribe(const std::string &topic)
{
    std::unique_lock<std::mutex> lock(sub_socket_mutex_);
    sub_socket_->set(zmq::sockopt::unsubscribe, topic);
}

void ZmqIpc::Publish(const std::string &topic, const std::vector<uint8_t> &message)
{
    zmq::message_t topic_msg{topic.data(), topic.size()};
    zmq::message_t message_msg{message.data(), message.size()};

    pub_socket_->send(topic_msg, zmq::send_flags::sndmore);
    pub_socket_->send(message_msg, zmq::send_flags::none);
}
