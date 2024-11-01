#include <asio.hpp>
#include <linux/netlink.h>
#include <netlink/socket.h>
#include <netlink/msg.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <memory>
#include <functional>
#include <climits>




#define NETLINK_USER 31
// 创建一个Netlink套接字
int create_netlink_socket() {
    struct sockaddr_nl local;
    //int sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    int sock = socket(PF_NETLINK, SOCK_RAW | SOCK_NONBLOCK, NETLINK_USER);

    if (sock < 0) {
        perror("socket");
        return -1;
    }

    memset(&local, 0, sizeof(local));
    local.nl_family = AF_NETLINK;
    local.nl_pid = getpid();
    local.nl_groups = RTMGRP_LINK | RTMGRP_IPV4_IFADDR;

    if (bind(sock, (struct sockaddr*)&local, sizeof(local)) < 0) {
        perror("bind");
        close(sock);
        return -1;
    }

    return sock;
}

// 异步读取Netlink消息
void async_read_netlink(asio::io_context& io, int sock) {
    asio::posix::stream_descriptor descriptor(io, sock);
    asio::streambuf buffer;

    descriptor.async_read_some(asio::buffer(buffer.prepare(1024)),
        [&](const asio::error_code& error, std::size_t bytes_transferred) {
            if (!error) {
                buffer.commit(bytes_transferred);

                // 处理接收到的数据
                struct nlmsghdr *nlh = nullptr;
                char *data = asio::buffer_cast<char*>(buffer.data());
                for (char *ptr = data; ptr < data + bytes_transferred; ) {
                    nlh = (struct nlmsghdr *)ptr;
                    if (nlh->nlmsg_type == NLMSG_DONE) {
                        break;
                    }
                    std::cout << "Received message: type=" << nlh->nlmsg_type
                              << ", len=" << nlh->nlmsg_len
                              << ", pid=" << nlh->nl_pid << std::endl;

                    ptr += NLMSG_ALIGN(nlh->nlmsg_len);
                }

                // 继续异步读取
                async_read_netlink(io, sock);
            } else {
                std::cerr << "Error: " << error.message() << std::endl;
            }
        });
}

int main() {
    try {
        int sock = create_netlink_socket();
        if (sock < 0) {
            return 1;
        }

        asio::io_context io;
        async_read_netlink(io, sock);

        io.run();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
