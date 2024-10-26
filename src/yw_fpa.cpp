#include "yw_fpa.h"

NetlinkCommunicator::NetlinkCommunicator() : stop_thread(false) {
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

    // Custom deleter for shared_ptr to close the epoll file descriptor
    auto epoll_deleter = [](int* epoll_fd) {
        if (epoll_fd && *epoll_fd >= 0) {
            close(*epoll_fd);
            std::cout << "Epoll fd closed" << std::endl;
        }
        delete epoll_fd;
    };

    // Create a shared_ptr with custom deleter for epoll file descriptor
    epoll_fd = std::shared_ptr<int>(new int(epoll_create1(0)), epoll_deleter);
    if (*epoll_fd < 0) {
        throw std::runtime_error("epoll_create1 failed: " + std::string(strerror(errno)));
    }

    // Add Netlink socket to epoll
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = *sock_fd;
    if (epoll_ctl(*epoll_fd, EPOLL_CTL_ADD, *sock_fd, &ev) < 0) {
        throw std::runtime_error("epoll_ctl failed: " + std::string(strerror(errno)));
    }

    // Start the thread
    epoll_thread = std::thread(&NetlinkCommunicator::epoll_loop, this);
}

NetlinkCommunicator::~NetlinkCommunicator() {
    stop_thread = true;
    if (epoll_thread.joinable()) {
        epoll_thread.join();
    }
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

Awaitable<std::string> NetlinkCommunicator::receive_message() {
    std::promise<std::string> promise;
    auto future = promise.get_future();
    {
        std::lock_guard<std::mutex> lock(promise_mutex);
        message_promise = std::move(promise);
    }
    return Awaitable<std::string>{std::move(future)};
}

void NetlinkCommunicator::epoll_loop() {
    while (!stop_thread) {
        struct epoll_event events[MAX_EVENTS];
        int nfds = epoll_wait(*epoll_fd, events, MAX_EVENTS, 1000); // 1 second timeout
        if (nfds < 0) {
            if (errno == EINTR) continue; // Interrupted by signal, retry
            throw std::runtime_error("epoll_wait failed: " + std::string(strerror(errno)));
        }

        for (int n = 0; n < nfds; ++n) {
            if (events[n].data.fd == *sock_fd) {
                //std::lock_guard<std::mutex> sock_lock(sock_mutex); // Not needed as we are not sending messages in this thread

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
            }
        }
    }
}
