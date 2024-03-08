//
// Created by 최현우 on 3/8/24.
//
#include <thread>
#include <vector>

void handle_client(int client_socket) {
    while (true) {
        char buffer[1024] = {0};
        int valread = read(client_socket, buffer, 1024);
        if (valread <= 0) {
            std::cerr << "connection terminated from client\n";
            break;
        }
        std::cout << "client: " << buffer << "\n";

        const char *message = "message received from server completed";
        send(client_socket, message, strlen(message), 0);
    }

    close(client_socket);
}

int main() {

    std::vector<std::thread> threads;

    while (true) {
        int new_socket;
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            std::cerr << "failed to accept connection\n";
            return 1;
        }

        std::cout << "client is connected\n";

        threads.push_back(std::thread(handle_client, new_socket));
    }

    for (auto& thread : threads) {
        thread.join();
    }

    close(server_fd);

    return 0;
}
