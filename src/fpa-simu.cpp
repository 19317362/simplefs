#include "yw_fpa.h"

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