// https://a-researcher.tistory.com/122 사이트에서 코드 참고
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <istream>
#include <unistd.h>

#define PORT 3000

using namespace std;

int main(){
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    const char *hello = "Hello from server";

    // 소켓 파일 디스크립터 생성
    // func explain -> socket(): 이 함수는 네트워크 통신을 위한 소켓을 생성하고, 소켓의 파일 디스크립터를 반환
    // AF_INET: 인터넷 프로토콜 v4 주소 체계를 사용하는 소켓을 지정
    // SOCK_STREAM: 연결 지향형 TCP 통신을 위한 소켓 타입을 지정
    // 0: 기본 프로토콜 지정
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        cerr << "Socket creation error!" << endl;
        return -1;
    }

    // Attaching socket to the port 3000
    // func explain -> setsockopt(): 소켓의 옵션 설정
    // server_fd -> 옵션을 설정할 sercer_fd
    // SOL_SOCKET -> SOL_SOCKET은 소켓 레벨의 옵션을 설정할 때 사용
    // 다른 프로토콜 레벨의 옵션을 설정하고 싶다면, 예를 들어 TCP 레벨의 옵션을 설정하고 싶다면 IPPROTO_TCP를 사용
    // SO_REUSEADDR -> 소켓이 로컬 주소를 재사용할 수 있음
    // SO_REUSEPORT -> 소켓이 port를 재사용할 수 있음
    // &opt, sizeof(opt) -> 옵션의 새 값을 가리키는 포인터, 옵션에 따라 이 값이 해석되는 방식이 달라짐
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        cerr << "Setsockopt err!" << endl;
        return -1;
    }

    // address.sin_family = AF_INET;
    // sin_family 멤버 변수: 소켓이 사용할 주소 체계(Address Family)를 지정

    // address.sin_addr.s_addr = INADDR_ANY;
    // sin_addr.s_addr 멤버 변수: 소켓이 바인딩될 호스트의 IP 주소를 지정
    // INADDR_ANY: 
    // 이 매크로는 시스템에서 사용 가능한 어떤 IP 주소로도 소켓을 바인딩할 수 있음
    // 즉, 모든 인터페이스를 통해 들어오는 연결을 수락하겠다는 의미
    // 일반적으로 서버가 하나 이상의 네트워크 인터페이스를 가지고 있을 때,
    // 특정 IP 주소를 지정하지 않고 모든 인터페이스를 통해 들어오는 연결을 수락하고자 할 때 사용

    // address.sin_port = htons(PORT);
    // sin_port 멤버 변수: 연결을 수락할 소켓의 포트 번호를 지정
    // htons(PORT): PORT는 서버가 연결을 수락할 포트 번호
    // htons 함수는 호스트 바이트 순서에서 네트워크 바이트 순서로 포트 번호를 변환
    // CP/IP 네트워크 통신 프로토콜은 네트워크 바이트 순서(Big-Endian)을 사용하기 때문에,
    // 다양한 호스트에서 실행되는 애플리케이션이 서로 통신할 때 호환성을 유지할 수 있도록 이러한 변환이 필요
    // Big-Endian 이란? -> 낮은 주소에 높은 바이트부터 저장하는 방식, 메모에 저장된 순서 그대로 읽을 수 있으며, 이해하기 쉬움
    // Big-Endian 참조 문헌) https://tcpschool.com/c/c_refer_endian
    address.sin_family = AF_INET; // 소켓이 사용할 주소 체계 지정, 이 경우엔 IPv4
    address.sin_addr.s_addr = INADDR_ANY; // 소켓이 바인딜 될 호스트의 IP 주소 지정, 이경우엔 모든 인터페이스를 통해 들어오는 연결을 수락
    address.sin_port = htons(PORT); // 네트워크 바이트 순서 (Big-Endian)으로 변환하여 연결을 수락한 소켓의 포트 번호를 변환

    // Bind the socket to the specified port
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        cerr << "Listen error" << endl;
        return -1;
    }

    // Accept incoming connections
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
        cerr << "Accept error" << endl;
        return -1;
    }

    // Receive message from the client
    valread = read(new_socket, buffer, 1024);
    cout << buffer << endl;

    // Send message to the client
    send(new_socket, hello, strlen(hello), 0);
    cout << "Hello message sent" << endl;

    return 0;
}