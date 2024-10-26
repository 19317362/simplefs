#include "yw_fpa.h"

NetlinkCommunicator::NetlinkCommunicator(asio::io_context& io_context)
    : io_context_(io_context), socket_(io_context), stop_thread(false) {
    // Custom deleter for shared_ptr to close the socket
    auto socket_deleter = [](int* sock_fd) {
        if (sock_fd && *sock_fd >= 0) {
            close(*sock_fd);
            std::cout << "Socket closed" << std::endl;
        }
        delete sock_fd;
    };

    // Create a shared_ptr with custom deleter
    sock_fd = std::shared_ptr<int>(new int(socket(PF_NETLINK, SOCK_RAW | SOCK_NONBLOCK, NETLINK_USER)), socket_deleter);
    if (*sock_fd < 0) {
        throw std::runtime_error("socket creation failed: " + std::string(strerror(errno)));
    }

    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid(); /* self pid */

    int rc = bind(*sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr));
    if (rc < 0) {
        throw std::runtime_error("bind failed: " + std::string(strerror(errno)));
    }

    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0; /* For Linux Kernel */
    dest_addr.nl_groups = 0; /* unicast */

    socket_.assign(*sock_fd);

    // Start receiving messages
    start_receive();
}

NetlinkCommunicator::~NetlinkCommunicator() {
    stop_thread = true;
    socket_.close();
}

void NetlinkCommunicator::send_message(const char* message, int message_len) {
    std::lock_guard<std::mutex> lock(sock_mutex);

    struct iovec iov;
    struct msghdr msg;
    int rc;

    // Custom deleter for shared_ptr to free the allocated memory
    auto nlh_deleter = [](struct nlmsghdr* nlh) {
        if (nlh) {
            free(nlh);
            std::cout << "Memory freed" << std::endl;
        }
    };

    // Create a shared_ptr with custom deleter for nlmsghdr
    std::shared_ptr<struct nlmsghdr> nlh((struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD)), nlh_deleter);
    if (!nlh) {
        throw std::runtime_error("malloc failed");
    }

    memset(nlh.get(), 0, NLMSG_SPACE(MAX_PAYLOAD));
    nlh->nlmsg_len = NLMSG_SPACE(message_len);
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 0;

    memcpy(NLMSG_DATA(nlh.get()), message, message_len);

    iov.iov_base = (void *)nlh.get();
    iov.iov_len = nlh->nlmsg_len;
    memset(&msg, 0, sizeof(msg));
    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    rc = sendmsg(*sock_fd, &msg, 0);
    if (rc < 0) {
        throw std::runtime_error("sendmsg failed: " + std::string(strerror(errno)));
    }
}

asio::awaitable<std::string> NetlinkCommunicator::receive_message() {
    std::promise<std::string> promise;
    auto future = promise.get_future();
    {
        std::lock_guard<std::mutex> lock(promise_mutex);
        message_promise = std::move(promise);
    }
    co_return co_await asio::co_spawn(io_context_, [this, future = std::move(future)]() mutable -> asio::awaitable<std::string> {
        co_return future.get();
    }, asio::use_awaitable);
}

void NetlinkCommunicator::start_receive() {
    auto self(shared_from_this());
    socket_.async_wait(asio::posix::stream_descriptor::wait_read, 
        [this, self](const asio::error_code& ec) {
            if (!ec) {
                struct iovec iov;
                struct msghdr msg;
                int rc;

                // Custom deleter for shared_ptr to free the allocated memory
                auto nlh_deleter = [](struct nlmsghdr* nlh) {
                    if (nlh) {
                        free(nlh);
                        std::cout << "Memory freed" << std::endl;
                    }
                };

                // Create a shared_ptr with custom deleter for nlmsghdr
                std::shared_ptr<struct nlmsghdr> nlh((struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD)), nlh_deleter);
                if (!nlh) {
                    throw std::runtime_error("malloc failed");
                }

                memset(nlh.get(), 0, NLMSG_SPACE(MAX_PAYLOAD));
                iov.iov_base = (void *)nlh.get();
                iov.iov_len = NLMSG_SPACE(MAX_PAYLOAD);
                memset(&msg, 0, sizeof(msg));
                msg.msg_name = (void *)&dest_addr;
                msg.msg_namelen = sizeof(dest_addr);
                msg.msg_iov = &iov;
                msg.msg_iovlen = 1;

                rc = recvmsg(*sock_fd, &msg, 0);
                if (rc < 0) {
                    throw std::runtime_error("recvmsg failed: " + std::string(strerror(errno)));
                }

                std::string received_message((char *)NLMSG_DATA(nlh.get()));
                std::lock_guard<std::mutex> lock(promise_mutex);
                if (message_promise) {
                    message_promise->set_value(received_message);
                    message_promise.reset();
                }

                // Continue receiving messages
                start_receive();
            }
        }
    );
}
