#include "ChatServer.hpp"

int main() {
    ChatServer server(9999);
    server.start();
    return 0;
}