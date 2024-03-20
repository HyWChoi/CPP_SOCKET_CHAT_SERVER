#ifndef CHATSERVER_HPP
#define CHATSERVER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/prepared_statement.h>
#include <stdexcept>
#include <vector>
#include <string>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <mutex>
#include <map>

namespace sql {
    namespace mysql {
        class MySQL_Driver;
    }
}

class ChatServer {
public:
    ChatServer(unsigned short port);
    ~ChatServer();
    void start();

private:
    unsigned short port_;
    SOCKET listeningSocket_;
    std::vector<SOCKET> clientSockets_;
    std::mutex driverMutex;

    void initializeWinsock();
    std::map<std::string, std::string> loadEnv(const std::string& envFilePath);
    void saveMessageToDatabase(const std::string& message);
    void setupServerSocket();
    void listenForClients();
    void handleClient(SOCKET clientSocket);
    void cleanup();
};

#endif