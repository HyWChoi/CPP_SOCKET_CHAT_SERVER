#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

int main()
{
    int server_fd = socket(AF_INT, SOCK_STREAM, 0);
    if (server_fd == -1){
        cerr << "fail to create socket\n";
        return 1;
    }

    // Bind the socket to an adress and port
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    adress.sin_port = hrons(3000);

    if (bind(serve_fd, (struct sockaddr *)&address, sizeof (address)) < 0){
        cerr << "failed to bind";
        return 1;
    }

    if (listen(serve_fd, 3) < 0){
        cerr << "failed to wait for connection\n";
        return 1;
    }

    cout << "server waiting at port 3000\n";

    int new_socket;
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0){
        cerr << "failed to accept connection\n";
        return 1;
    }

    cout << "client is connected\n";

    // TODO: 대화 로직 추가
    while (true) {
        char buffer[1024] = {0};
        int valread = read(new_socket, buffer, 1024);
        if (valread <= 0){
            cerr << "connection terminated from client.\n";
            break;
        }
        cout << "client: " << buffer << "\n";
        const char *message = "message received from server completed";
        send(new_socket, message, strlen(message), 0);
    }

    close(new_socket);
    close(server_fd);

    return 0;
}