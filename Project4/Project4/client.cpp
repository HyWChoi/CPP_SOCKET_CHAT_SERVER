#include "client.hpp"

void ChatClient::initializeWinsock() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "Winsock initialization failed: " << result << std::endl;
        exit(1);
    }
}

void ChatClient::connectToServer() {
    serverSocket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket_ == INVALID_SOCKET) {
        std::cerr << "Failed to create socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        exit(1);
    }

    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    InetPtonA(AF_INET, ip_.c_str(), &serverAddr.sin_addr);
    serverAddr.sin_port = htons(static_cast<u_short>(port_));

    if (connect(serverSocket_, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Failed to connect to server: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket_);
        WSACleanup();
        exit(1);
    }
}
void ChatClient::startReceiveThread() {
    while (running) {
        receiveMessage();
    }
}

void ChatClient::start() {
    std::thread receiveThread(&ChatClient::startReceiveThread, this);

    startChat();

    if (receiveThread.joinable()) {
        receiveThread.join();
    }
}


void ChatClient::startChat() {
    std::string input;
    while (running) {
        std::getline(std::cin, input);
        if (input == "/x") {
            running = false;
            break;
        }
        sendMessage("[" + nickname_ + "] " + input);
    }
}

void ChatClient::sendMessage(const std::string& message) {
    send(serverSocket_, message.c_str(), static_cast<int>(message.length()), 0);
}

void ChatClient::receiveMessage() {
    char buffer[1024];

    while (running) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(serverSocket_, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            std::cout << "Received: " << std::string(buffer, 0, bytesReceived) << std::endl;
        }
        else if (bytesReceived == 0) {
            std::cout << "Server closed the connection." << std::endl;
            break;
        }
        else {
            std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
            break;
        }
    }
}
void ChatClient::cleanup() {
    if (serverSocket_ != INVALID_SOCKET) {
        closesocket(serverSocket_);
    }
    WSACleanup();
}

