#include <asio.hpp>
#include "yw_fpa.h"

asio::awaitable<void> run_netlink_communicator(std::shared_ptr<NetlinkCommunicator> netlink_comm) {
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

            netlink_comm->send_message(message, message_len);
            std::string received_message = co_await netlink_comm->receive_message();
            std::cout << "Received message payload: " << received_message << std::endl;
        }
    }
}

int main() {
    try {
        asio::io_context io_context;
        auto netlink_comm = std::make_shared<NetlinkCommunicator>(io_context);

        asio::co_spawn(io_context, run_netlink_communicator(netlink_comm), asio::detached);

        io_context.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    return 0;
}