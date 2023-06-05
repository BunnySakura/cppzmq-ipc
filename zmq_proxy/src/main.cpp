#include "zmq.hpp"
#include "cxxopts.hpp"

#include "iostream"
#include "string"

const std::string kProjectName = "zmq_proxy";
const std::string kProjectVersion = "0.1.0";

int main(int argc, char **argv) {
  cxxopts::Options options("zmq_proxy", "A relay agent for subscriptions and publications");
  options.allow_unrecognised_options(); // 跳过无法识别的参数
  options.add_options()
      ("h,help", "Print usage")
      ("d,debug", "Enable debugging")
      ("v,version", "Print version information and then exit")
      ("t,tcp", "Use TCP as the channel")
      ("i,ipc", "Use IPC as the channel")
      ("s,subscriber",
       R"(Subscriber, such as "ipc:///tmp/zmq_proxy_pub" or "tcp://*:5555")",
       cxxopts::value<std::string>())
      ("p,publisher",
       R"(Publisher, such as "ipc:///tmp/zmq_proxy_sub" or "tcp://*:5556")",
       cxxopts::value<std::string>());
  auto result = options.parse(argc, argv);

  if (result.count("help") || result.arguments().empty()) {
    std::cout << options.help() << std::endl;
    exit(0);
  }

  if (result.count("version")) {
    std::cout << kProjectName << " version: " << kProjectVersion << std::endl
              << "Build date(UTC0): " << __DATE__ << " " << __TIME__ << std::endl;
    exit(0);
  }


  // 创建 ZeroMQ 上下文
  zmq::context_t context(1);

  // 创建 SUB 和 PUB 套接字
  zmq::socket_t sub_socket(context, zmq::socket_type::sub);
  zmq::socket_t pub_socket(context, zmq::socket_type::pub);

  // 绑定 SUB 套接字，绑定 PUB 套接字
  std::string subscriber;
  std::string publisher;
  if (result.count("tcp")) {
    subscriber = result.count("subscriber") ?
                 result["subscriber"].as<std::string>() :
                 "tcp://*:5555";
    publisher = result.count("publisher") ?
                result["publisher"].as<std::string>() :
                "tcp://*:5556";
  } else if (result.count("ipc")) {
#if WIN32
    subscriber = result.count("subscriber") ?
                 result["subscriber"].as<std::string>() :
                 "ipc://C:/Windows/Temp/zmq_proxy_pub";
    publisher = result.count("publisher") ?
                result["publisher"].as<std::string>() :
                "ipc://C:/Windows/Temp/zmq_proxy_sub";
#else
    subscriber = result.count("subscriber") ?
                 result["subscriber"].as<std::string>() :
                 "ipc:///tmp/zmq_proxy_pub";
    publisher = result.count("publisher") ?
                result["publisher"].as<std::string>() :
                "ipc:///tmp/zmq_proxy_sub";
#endif  // WIN32
  } else {
    std::cout << "Must have tcp or ipc parameter" << std::endl;
    exit(0);
  }

  std::cout << "Subscriber: \t" << subscriber << std::endl
            << "Publisher: \t" << publisher << std::endl;
  sub_socket.bind(subscriber);
  pub_socket.bind(publisher);

  // 订阅所有消息
  sub_socket.set(zmq::sockopt::subscribe, "");

  // 将消息从 SUB 套接字转发到 PUB 套接字
  zmq::proxy(sub_socket, pub_socket);

  return 0;
}
