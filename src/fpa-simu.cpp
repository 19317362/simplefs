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
    struct nlmsghdr *nlh = NULL;
    struct iovec iov;
    struct msghdr msg;

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

    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
    if (!nlh) {
        printf("malloc failed\n");
        return -1;
    }
    memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
    nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 0;

    strcpy((char *)NLMSG_DATA(nlh), "Hello from user");

    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;
    memset(&msg, 0, sizeof(msg));
    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    rc = sendmsg(*sock_fd, &msg, 0);
    if (rc < 0) {
        printf("sendmsg failed: %d %s\n", rc, strerror(errno));
        free(nlh);
        return -1;
    }

    /* Read message from kernel */
    rc = recvmsg(*sock_fd, &msg, 0);
    if (rc < 0) {
        printf("recvmsg failed: %d %s\n", rc, strerror(errno));
        free(nlh);
        return -1;
    }
    printf("Received message payload: %s\n", (char *)NLMSG_DATA(nlh));

    free(nlh);
    return 0;
}