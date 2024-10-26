#include <coroutine>
#include <future>
#include <iostream>
#include <memory>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <errno.h>
#include <sys/epoll.h>

#include "dxrfs_msg.h"

#define NETLINK_USER 31
#define MAX_PAYLOAD 1024 /* maximum payload size*/
#define MAX_EVENTS 10

template<typename T>
struct Awaitable {
    struct promise_type {
        std::promise<T> promise;

        Awaitable get_return_object() {
            return Awaitable{promise.get_future()};
        }

        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }

        void return_value(T value) {
            promise.set_value(value);
        }

        void unhandled_exception() {
            promise.set_exception(std::current_exception());
        }
    };

    std::future<T> future;

    bool await_ready() { return future.wait_for(std::chrono::seconds(0)) == std::future_status::ready; }
    void await_suspend(std::coroutine_handle<>) { }
    T await_resume() { return future.get(); }
};

class NetlinkCommunicator {
public:
    NetlinkCommunicator() {
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
    }

    void send_message(const char* message, int message_len) {
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

    Awaitable<std::string> receive_message() {
        struct epoll_event events[MAX_EVENTS];
        int nfds = epoll_wait(*epoll_fd, events, MAX_EVENTS, -1);
        if (nfds < 0) {
            throw std::runtime_error("epoll_wait failed: " + std::string(strerror(errno)));
        }

        for (int n = 0; n < nfds; ++n) {
            if (events[n].data.fd == *sock_fd) {
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
                co_return std::string((char *)NLMSG_DATA(nlh.get()));
            }
        }
        co_return "";
    }

private:
    std::shared_ptr<int> sock_fd;
    std::shared_ptr<int> epoll_fd;
    struct sockaddr_nl src_addr, dest_addr;
};

int main() {
    try {
        NetlinkCommunicator netlink_comm;

        char input[MAX_PAYLOAD];
        while (true) {
            std::cout << "Enter command (q to quit, s<message> to send): ";
            if (!fgets(input, sizeof(input), stdin)) {
                std::cerr << "Error reading input" << std::endl;
                continue;
            }

            // Remove newline character from input
            input[strcspn(input, "\n")] = 0;

            if (input[0] == 'q') {
                break;
            } else if (input[0] == 's') {
                const char *message = input + 1; // Skip the 's' character
                int message_len = strlen(message);

                netlink_comm.send_message(message, message_len);
                auto future = netlink_comm.receive_message().future;
                future.wait();
                std::cout << "Received message payload: " << future.get() << std::endl;                
            }
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    return 0;
}