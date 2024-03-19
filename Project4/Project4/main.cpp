#include "client.hpp"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <Server IP> <Port> <Nickname>\n";
        return 1;
    }

    std::string serverIp = argv[1];
    int port = std::stoi(argv[2]);
    std::string nickname = argv[3];

    ChatClient client(serverIp, port, nickname);
    client.start();

    return 0;
}