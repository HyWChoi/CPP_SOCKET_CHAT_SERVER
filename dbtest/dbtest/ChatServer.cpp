#include "ChatServer.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/prepared_statement.h>
#include <stdexcept>

ChatServer::ChatServer(unsigned short port) : port_(port), listeningSocket_(INVALID_SOCKET) {
    initializeWinsock();
    setupServerSocket();
}

ChatServer::~ChatServer() {
    cleanup();
}

void ChatServer::start() {
    listenForClients();
}

void ChatServer::initializeWinsock() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed with error: " << result << std::endl;
        std::cerr << "Detailed error: " << WSAGetLastError() << std::endl;
        std::cerr << "WSAStartup failed: " << result << std::endl;
        exit(1);
    }
    std::cout << "WSAStartup successful" << std::endl;
}

std::map<std::string, std::string> ChatServer::loadEnv(const std::string& envFilePath) {
    std::map<std::string, std::string> env;
    std::ifstream file(envFilePath);
    std::string line;

    while (std::getline(file, line)) {
        std::istringstream is_line(line);
        std::string key;
        if (std::getline(is_line, key, '=')) {
            std::string value;
            if (key[0] == '#') continue; // 주석 처리된 줄은 무시
            if (std::getline(is_line, value)) {
                env[key] = value;
            }
        }
    }
    return env;
}

void ChatServer::saveMessageToDatabase(const std::string& message) {
    try {
        auto env = loadEnv("\.env");
        std::string host = env["DB_HOST"];
        std::string port = env["DB_PORT"];
        std::string user = env["DB_USER"];
        std::string password = env["DB_PASSWORD"];
        std::string connectionStr = "tcp://" + host + ":" + port;

        // MySQL 드라이버 인스턴스 호출을 보호하기 위해 뮤텍스 사용
        std::lock_guard<std::mutex> guard(driverMutex);
        sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
        if (!driver) {
            std::cerr << "MySQL driver instance not found" << std::endl;
            return;
        }
        std::cout << "Driver instance created" << std::endl;
        std::unique_ptr<sql::Connection> con(driver->connect(connectionStr, user, password));
        std::cout << "Connected to database" << std::endl;

        con->setSchema("chat_db");
        std::cout << "Schema set to chat_db" << std::endl;

        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("INSERT INTO messages(message) VALUES(?)"));
        std::cout << "Prepared statement created" << std::endl;

        pstmt->setString(1, message);
        std::cout << "Message set to prepared statement" << std::endl;

        pstmt->executeUpdate();
        std::cout << "Message saved to database" << std::endl;
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQLException in saveMessageToDatabase: " << e.what() << std::endl;
    }
}

void ChatServer::setupServerSocket() {
    listeningSocket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (listeningSocket_ == INVALID_SOCKET) {
        std::cerr << "Failed to create socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        exit(1);
    }
    std::cout << "Socket created" << std::endl;

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port_);
    hint.sin_addr.S_un.S_addr = INADDR_ANY;

    if (bind(listeningSocket_, (sockaddr*)&hint, sizeof(hint)) == SOCKET_ERROR) {
        std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
        closesocket(listeningSocket_);
        WSACleanup();
        exit(1);
    }

    std::cout << "Server is set up and bound to port " << port_ << std::endl;

    if (listen(listeningSocket_, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed: " << WSAGetLastError() << std::endl;
        closesocket(listeningSocket_);
        WSACleanup();
        exit(1);
    }

    std::cout << "Server is now listening on port " << port_ << std::endl;
}

void ChatServer::listenForClients() {
    std::cout << "Waiting for clients..." << std::endl;
    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);

    while (true) {
        SOCKET clientSocket = accept(listeningSocket_, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrSize);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
            continue;
        }

        char clientIp[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientIp, INET_ADDRSTRLEN);
        std::cout << "Client connected from " << clientIp << ":" << ntohs(clientAddr.sin_port) << std::endl;

        clientSockets_.push_back(clientSocket);
        std::thread(&ChatServer::handleClient, this, clientSocket).detach();
    }
}

void ChatServer::handleClient(SOCKET clientSocket) {
    char buf[4096];
    while (true) {
        ZeroMemory(buf, 4096);
        int bytesReceived = recv(clientSocket, buf, 4096, 0);
        if (bytesReceived <= 0) {
            closesocket(clientSocket);
            break;
        }

        std::string message(buf, bytesReceived);
        saveMessageToDatabase(message);

        for (SOCKET outSock : clientSockets_) {
            if (outSock != clientSocket) {
                send(outSock, buf, bytesReceived, 0);
            }
        }
    }
}

void ChatServer::cleanup() {
    closesocket(listeningSocket_);
    for (SOCKET sock : clientSockets_) {
        closesocket(sock);
    }
    WSACleanup();
}