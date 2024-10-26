#include <coroutine>
#include <future>
#include <iostream>
#include <memory>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <errno.h>
#include <asio.hpp>
#include <asio/posix/stream_descriptor.hpp>
#include <thread>
#include <atomic>
#include <mutex>
#include <optional>

#include "dxrfs_msg.h"

#define NETLINK_USER 31
#define MAX_PAYLOAD 1024 /* maximum payload size*/

class NetlinkCommunicator : public std::enable_shared_from_this<NetlinkCommunicator> {
public:
    NetlinkCommunicator(asio::io_context& io_context);
    ~NetlinkCommunicator();

    void send_message(const char* message, int message_len);
    asio::awaitable<std::string> receive_message();

private:
    void start_receive();

    asio::io_context& io_context_;
    asio::posix::stream_descriptor socket_;
    std::shared_ptr<int> sock_fd;
    struct sockaddr_nl src_addr, dest_addr;
    std::atomic<bool> stop_thread;
    std::mutex sock_mutex;
    std::mutex promise_mutex;
    std::optional<std::promise<std::string>> message_promise;
};
