#ifndef CLIENT_HPP
#define CLIENT_HPP


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
    void start();

private:
    std::string ip_;
    bool running = true;
    int port_;
    std::string nickname_;
    SOCKET serverSocket_ = INVALID_SOCKET;
    void startReceiveThread();
    void initializeWinsock();
    void connectToServer();
    void sendMessage(const std::string& message);
    void receiveMessage();
    void cleanup();
};

#endif