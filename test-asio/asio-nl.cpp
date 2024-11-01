// FILE: asio-nl.cpp

#include <iostream>
#include <memory>
#include <functional>
#include <asio.hpp>
#include <linux/netlink.h>
#include <netlink/socket.h>
#include <netlink/msg.h>

#define NETLINK_USER 31 

int create_netlink_socket() {
    struct sockaddr_nl local;
    int sock = socket(PF_NETLINK, SOCK_RAW | SOCK_NONBLOCK, NETLINK_USER);

    if (sock < 0) {
        perror("socket");
        return -1;
    }

    memset(&local, 0, sizeof(local));
    local.nl_family = AF_NETLINK;
    local.nl_pid = getpid();

    if (bind(sock, (struct sockaddr*)&local, sizeof(local)) < 0) {
        perror("bind");
        close(sock);
        return -1;
    }

    return sock;
}

void async_read_netlink(asio::io_context& io_context, 
    asio::posix::stream_descriptor& stream,
    std::shared_ptr<std::vector<char>> buffer
    )
{
    std::cout << "async_read_netlink ... " << std::endl;
    stream.async_wait(asio::posix::stream_descriptor::wait_read,
        [buffer, &stream, &io_context](const std::error_code& ec)
        {
            if (!ec)
            {
                std::cout << "Read Arrived " << std::endl;
                async_read_netlink(io_context, stream, buffer);
            }
            else if (ec == asio::error::operation_aborted)
            {
                std::cerr << "Error: Operation aborted" << std::endl;
            }
            else
            {
                std::cerr << "Error: " << ec.message() << std::endl;
            }
        });
}

void ReadMsg(int sock) {
    struct sockaddr_nl nladdr;
    struct msghdr msg;
    struct iovec iov;
    char buffer[4096];

    memset(&nladdr, 0, sizeof(nladdr));
    nladdr.nl_family = AF_NETLINK;

    iov.iov_base = buffer;
    iov.iov_len = sizeof(buffer);
    msg.msg_name = &nladdr;
    msg.msg_namelen = sizeof(nladdr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    int ret = recvmsg(sock, &msg, 0);
    if (ret < 0) {
        perror("recvmsg");
    } else {
        std::cout << "Received message: " << buffer << std::endl;
    }
}
void TimerHandler(asio::steady_timer& timer, int sock) {
    ReadMsg(sock);
    timer.expires_after(asio::chrono::seconds(10));
    timer.async_wait([&](const asio::error_code& ec) {
        if (!ec) {
            TimerHandler(timer, sock);
        }
    });
}

void handle_ep_read(const asio::error_code& ec, std::size_t bytes_transferred) {
    if (!ec) {
        std::cout << "Read " << bytes_transferred << " bytes" << std::endl;
    } else {
        std::cerr << "Error on read: " << ec.message() << std::endl;
    }
}

int main() {
    try {
        int sock = create_netlink_socket();
        if (sock < 0) {
            return 1;
        }

        asio::io_context io;
        auto buffer = std::make_shared<std::vector<char>>(1024);
        asio::posix::stream_descriptor stream(io, sock);
        async_read_netlink(io, stream, buffer);

        asio::steady_timer timer(io, asio::chrono::seconds(10));
        timer.async_wait([&](const asio::error_code& ec) {
            if (!ec) {
                TimerHandler(timer, sock);
            }
        });

        io.run();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
