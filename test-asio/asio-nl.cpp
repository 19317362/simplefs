// FILE: asio-nl.cpp

#include <iostream>
#include <memory>
#include <functional>
#include <asio.hpp>
#include <linux/netlink.h>
#include <netlink/socket.h>
#include <netlink/msg.h>

#define NETLINK_USER 31 
#define MAX_PAYLOAD 1024
struct test_context
{
    int v_nl_socket;
    int v_tty_fd;
    int v_epoll_fd;

};
static test_context g_test_context;

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
        std::cout << "Received message: " << ret << std::endl;
    }
}

// send message to netlink socket
void send_netlink_message(int sock_fd, const char* message, size_t message_len) {
    struct iovec iov;
    struct msghdr msg;
    int rc;

    struct nlmsghdr* nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
    if (!nlh) {
        throw std::runtime_error("malloc failed");
    }

    memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
    nlh->nlmsg_len = NLMSG_SPACE(message_len);
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 0;

    memcpy(NLMSG_DATA(nlh), message, message_len);

    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;
    memset(&msg, 0, sizeof(msg));

    struct sockaddr_nl dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0; /* For Linux Kernel */
    dest_addr.nl_groups = 0; /* unicast */

    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    rc = sendmsg(sock_fd, &msg, 0);
    if (rc < 0) {
        free(nlh);
        throw std::runtime_error("sendmsg failed: " + std::string(strerror(errno)));
    }
    free(nlh);

}

void async_read_epoll(asio::io_context& io_context, 
    asio::posix::stream_descriptor& stream,
    std::shared_ptr<std::vector<char>> buffer
    )
{
    //std::cout << "async_read_netlink ... " << std::endl;
    stream.async_wait(asio::posix::stream_descriptor::wait_read,
        [buffer, &stream, &io_context](const std::error_code& ec)
        {
            if (!ec)
            {
                std::cout << "Read Arrived " << std::endl;
                //使用 epoll_wait 处理读事件
                struct epoll_event events[10];
                int nfds = 0;
                do
                {
                    nfds = epoll_wait(g_test_context.v_epoll_fd , events, 10, 0);
                    if (nfds == -1) {
                        if (errno == EINTR) {
                            continue;        
                        }
                        else{
                            std::cerr << "epoll_wait failed: " << strerror(errno) << std::endl;
                            break;
                        }
                    }
                    else{
                        for (int n = 0; n < nfds; ++n) {
                            if (events[n].data.fd == g_test_context.v_nl_socket) {
                                ReadMsg(g_test_context.v_nl_socket);
                            }
                            else if (events[n].data.fd == g_test_context.v_tty_fd) {
                                char buffer[4096];
                                int ret = read(g_test_context.v_tty_fd, buffer, sizeof(buffer));
                                if (ret < 0) {
                                    perror("read");
                                } else {
                                    std::cout << "Received message: " << ret << std::endl;
                                }
                            }
                            else
                            {
                                std::cerr << "Unknown fd: " << events[n].data.fd << std::endl;
                            }
                            
                        }                

                    }
                } while (nfds>0);
                



                async_read_epoll(io_context, stream, buffer);
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
void async_read_netlink(asio::io_context& io_context, 
    asio::posix::stream_descriptor& stream,
    std::shared_ptr<std::vector<char>> buffer
    )
{
    //std::cout << "async_read_netlink ... " << std::endl;
    stream.async_wait(asio::posix::stream_descriptor::wait_read,
        [buffer, &stream, &io_context](const std::error_code& ec)
        {
            if (!ec)
            {
                std::cout << "Read Arrived " << std::endl;
                ReadMsg(g_test_context.v_nl_socket);


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

void async_wait_tnt(asio::io_context& io_context, 
    asio::posix::stream_descriptor& stream,
    std::shared_ptr<std::vector<char>> buffer
    )
{
    //std::cout << "async_read_netlink ... " << std::endl;
    stream.async_wait(asio::posix::stream_descriptor::wait_read,
        [buffer, &stream, &io_context](const std::error_code& ec)
        {
            if (!ec)
            {
                std::cout << "TNT Arrived " << std::endl;
                
                int ret = read(g_test_context.v_tty_fd, buffer->data(), buffer->size());
                if (ret < 0) {
                    perror("read");
                } else {
                    std::cout << "Received message: " << ret << std::endl;
                }


                async_wait_tnt(io_context, stream, buffer);
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
static std::array<char, 1024> tnt_buffer_;
void async_read_tnt(asio::io_context& io_context, 
    asio::posix::stream_descriptor& stream
    
    )
{
    //调用 async_read 
    stream.async_read_some(
        asio::buffer(tnt_buffer_), // Correctly dereference the buffer
        [&](const asio::error_code& ec, std::size_t bytes_transferred)
        {
            if (!ec)
            {
                std::cout << "TNT RX " << bytes_transferred << std::endl;
                
                async_read_tnt(io_context, stream);
            }
            else if (ec == asio::error::operation_aborted)
            {
                std::cerr << "TNT Error: Operation aborted" << std::endl;
            }
            else
            {
                std::cerr << "TNT Error: " << ec.message() << std::endl;
            }
        });

}

#define TIME_INTERVAL 1
void TimerHandler(asio::steady_timer& timer, int sock) {
    //ReadMsg(sock);
    std::cout << "TimerHandler ... " << std::endl;
    char buf[32];
    static int NB = 0;
    auto len = sprintf(buf, "No. %d", NB++);
    send_netlink_message(sock, buf, len);
    /*
    timer.expires_after(asio::chrono::seconds(TIME_INTERVAL));
    timer.async_wait([&](const asio::error_code& ec) {
        if (!ec) {
            TimerHandler(timer, sock);
        }
    });
    */
}



int main() {
    try {
        int sock = create_netlink_socket();
        if (sock < 0) {
            return 1;
        }
        printf("sock = %d\n", sock);
        g_test_context.v_nl_socket = sock;
        // Open the /dev/tnt0 device
        int tnt_fd = open("/dev/tnt0", O_RDONLY | O_NONBLOCK);
        if (tnt_fd == -1) {
            perror("open");
            return 1;
        }
        g_test_context.v_tty_fd = tnt_fd;

        g_test_context.v_epoll_fd = 0;




        asio::io_context io;
        asio::posix::stream_descriptor ep_stream(io, sock);

        // Example buffer to read into
        std::vector<char> buffer(1024);

        asio::posix::stream_descriptor tnt_stream(io, tnt_fd);
        // Start async wait on epoll
        async_read_netlink(io, ep_stream, std::make_shared<std::vector<char>>(buffer));

        std::vector<char> bufferTnt(1024);
        async_read_tnt(io, tnt_stream);
        //async_wait_tnt(io, tnt_stream, std::make_shared<std::vector<char>>(buffer));
        asio::steady_timer timer(io, asio::chrono::seconds(TIME_INTERVAL));
        timer.async_wait([&](const asio::error_code& ec) {
            if (!ec) {
                TimerHandler(timer, sock);
            }
        });        

        // Run the io_context to process the asynchronous operations
        io.run();

        // Close the file descriptors
        close(tnt_fd);
        close(sock);
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
