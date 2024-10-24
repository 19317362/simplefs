#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <memory>
#include <errno.h>

#define NETLINK_USER 31
#define MAX_PAYLOAD 1024 /* maximum payload size*/

int main()
{
    struct sockaddr_nl src_addr, dest_addr;
    struct iovec iov;
    struct msghdr msg;
    char input[MAX_PAYLOAD];

    // Custom deleter for shared_ptr to close the socket
    auto socket_deleter = [](int* sock_fd) {
        if (sock_fd && *sock_fd >= 0) {
            close(*sock_fd);
            printf("Socket closed\n");
        }
        delete sock_fd;
    };

    // Create a shared_ptr with custom deleter
    std::shared_ptr<int> sock_fd(new int(socket(PF_NETLINK, SOCK_RAW, NETLINK_USER)), socket_deleter);
    if (*sock_fd < 0) {
        printf("socket creation failed: %s\n", strerror(errno));
        return -1;
    }

    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid(); /* self pid */

    int rc = bind(*sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr));
    if (rc < 0) {
        printf("bind failed: %d %s\n", rc, strerror(errno));
        return -1;
    }

    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0; /* For Linux Kernel */
    dest_addr.nl_groups = 0; /* unicast */

    // Custom deleter for shared_ptr to free the allocated memory
    auto nlh_deleter = [](struct nlmsghdr* nlh) {
        if (nlh) {
            free(nlh);
            printf("Memory freed\n");
        }
    };

    // Create a shared_ptr with custom deleter for nlmsghdr
    std::shared_ptr<struct nlmsghdr> nlh((struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD)), nlh_deleter);
    if (!nlh) {
        printf("malloc failed\n");
        return -1;
    }

    while (1) {
        printf("Enter command (q to quit, s<message> to send): ");
        if (!fgets(input, sizeof(input), stdin)) {
            printf("Error reading input\n");
            continue;
        }

        // Remove newline character from input
        input[strcspn(input, "\n")] = 0;

        if (input[0] == 'q') {
            break;
        } else if (input[0] == 's') {
            const char *message = input + 1; // Skip the 's' character

            memset(nlh.get(), 0, NLMSG_SPACE(MAX_PAYLOAD));
            nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
            nlh->nlmsg_pid = getpid();
            nlh->nlmsg_flags = 0;

            strncpy((char *)NLMSG_DATA(nlh.get()), message, MAX_PAYLOAD - NLMSG_HDRLEN);

            iov.iov_base = (void *)nlh.get();
            iov.iov_len = nlh->nlmsg_len;
            memset(&msg, 0, sizeof(msg));
            msg.msg_name = (void *)&dest_addr;
            msg.msg_namelen = sizeof(dest_addr);
            msg.msg_iov = &iov;
            msg.msg_iovlen = 1;

            rc = sendmsg(*sock_fd, &msg, 0);
            if (rc < 0) {
                printf("sendmsg failed: %d %s\n", rc, strerror(errno));
                continue;
            }

            /* Read message from kernel */
            rc = recvmsg(*sock_fd, &msg, 0);
            if (rc < 0) {
                printf("recvmsg failed: %d %s\n", rc, strerror(errno));
                continue;
            }
            printf("Received message payload: %s\n", (char *)NLMSG_DATA(nlh.get()));
        }
    }

    return 0;
}