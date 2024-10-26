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
#include <thread>
#include <atomic>
#include <mutex>
#include <optional>

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
    NetlinkCommunicator();
    ~NetlinkCommunicator();

    void send_message(const char* message, int message_len);
    Awaitable<std::string> receive_message();

private:
    void epoll_loop();

    std::shared_ptr<int> sock_fd;
    std::shared_ptr<int> epoll_fd;
    struct sockaddr_nl src_addr, dest_addr;
    std::thread epoll_thread;
    std::atomic<bool> stop_thread;
    std::mutex sock_mutex;
    std::mutex promise_mutex;
    std::optional<std::promise<std::string>> message_promise;
};
