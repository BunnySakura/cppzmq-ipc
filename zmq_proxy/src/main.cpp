#include "zmq.hpp"

int main()
{
    // 创建 ZeroMQ 上下文
    zmq::context_t context(1);

    // 创建 SUB 和 PUB 套接字
    zmq::socket_t sub_socket(context, zmq::socket_type::sub);
    zmq::socket_t pub_socket(context, zmq::socket_type::pub);

    // 绑定 SUB 套接字到 "tcp://localhost:5555"
    sub_socket.bind("tcp://*:5555");
    sub_socket.set(zmq::sockopt::subscribe, "");

    // 绑定 PUB 套接字到 "tcp://localhost:5556"
    pub_socket.bind("tcp://*:5556");

    // 将消息从 SUB 套接字转发到 PUB 套接字
    zmq::proxy(sub_socket, pub_socket);

    return 0;
}
