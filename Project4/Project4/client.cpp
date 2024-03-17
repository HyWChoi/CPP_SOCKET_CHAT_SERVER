#include <iostream>
#include <string>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <thread>

class ChatClient {
public:
    ChatClient(const std::string& ip, int port, const std::string& nickname)
        : ip_(ip), port_(port), nickname_(nickname) {
        initializeWinsock();
        connectToServer();
    }

    ~ChatClient() {
        cleanup();
    }

    void startChat();
    void start(); // Add a method to start chatting and receiving messages simultaneously

private:
    std::string ip_;
    bool running = true;
    int port_;
    std::string nickname_;
    SOCKET serverSocket_ = INVALID_SOCKET;
    void startReceiveThread(); // Method to start the thread for receiving messages

    void initializeWinsock();
    void connectToServer();
    void sendMessage(const std::string& message);
    void receiveMessage();
    void cleanup();
};

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
    // 올바르게 InetPton 함수 사용
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
        receiveMessage(); // Call your existing receiveMessage method
    }
}

void ChatClient::start() {
    std::thread receiveThread(&ChatClient::startReceiveThread, this); // Start the receiving thread

    startChat(); // Start chatting (sending messages)

    if (receiveThread.joinable()) {
        receiveThread.join(); // Make sure to join the thread when finished
    }
}


void ChatClient::startChat() {
    std::string input;
    while (running) {
        std::getline(std::cin, input);
        if (input == "/x") {
            running = false; // Ensure to stop the receive thread as well
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

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <Server IP> <Port> <Nickname>\n";
        return 1;
    }

    std::string serverIp = argv[1];
    int port = std::stoi(argv[2]);
    std::string nickname = argv[3];

    ChatClient client(serverIp, port, nickname);
    client.start(); // Use the new start method

    return 0;
}
