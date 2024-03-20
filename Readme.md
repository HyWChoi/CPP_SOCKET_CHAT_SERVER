# TCP/IP 소켓 프로그래밍 프로젝트
### 프로젝트 git link && 블로그 회고록
https://github.com/HyWChoi/CPP_SOCKET_CHAT_SERVER
https://bloblogle.tistory.com/category/Project/socket_chat_server
## 개요

이 프로젝트는 Windows 환경에서 Visual Studio를 사용하여 TCP/IP 소켓 프로그래밍의 기본을 다룹니다. 프로젝트는 두 단계로 구성되어 있으며, 기본적인 서버/클라이언트 연결의 설정부터 시작하여, 간단한 채팅 애플리케이션의 구현 및 데이터베이스를 통한 채팅 기록 저장까지 포함됩니다.

## 0단계: 개발 환경 설정

### OS: Windows
프로젝트는 Windows 환경에서 개발됩니다. Windows 환경을 선택한 주된 이유는 개발자가 평소에 Mac을 주로 사용하며, Windows 코딩 및 Winsocket 사용 경험이 부족하기 때문입니다.

### 에디터: Visual Studio
개발에는 Microsoft의 Visual Studio를 사용합니다. Visual Studio는 다양한 프로그래밍 언어를 지원하며, Windows 기반의 애플리케이션 개발에 최적화된 강력한 IDE입니다.

## 1단계: 기본적인 TCP/IP 소켓 서버 / 클라이언트 만들기

### TCP/IP 소켓 이해하기
- TCP/IP 프로토콜이 네트워크 통신을 어떻게 가능하게 하는지에 대해 연구합니다.
- 소켓이 서버와 클라이언트 간의 연결을 어떻게 설정하는지 학습합니다.

### 기본 구현
- 서버와 클라이언트 간의 기본적인 연결을 설정하고, 이 연결이 성공적으로 이루어졌는지를 확인하는 프로그램을 작성합니다.

# 구현
## TCP 서버 예제 구현

네트워크 프로그래밍의 첫 단계는 통신을 위한 소켓 생성입니다. 소켓은 네트워크 상에서 데이터를 주고받기 위한 엔드포인트 역할을 합니다.

### 소켓 생성

- **AF_INET**: IPv4 인터넷 프로토콜 주소 체계 사용
- **SOCK_STREAM**: TCP 통신을 위한 소켓 타입
- **0**: 기본 프로토콜 지정

### 소켓 옵션 설정

소켓을 생성한 후, 소켓을 특정 포트에 바인딩하기 전에 소켓 옵션을 설정해야 합니다.
```cpp
int opt = 1;
if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
    perror("Setsockopt failed");
    return -1;
}
```
- **SOL_SOCKET**: 소켓 레벨의 옵션 설정
- **SO_REUSEADDR | SO_REUSEPORT**: 포트 재사용 가능 옵션 활성화

### 주소 바인딩

주소 바인딩 과정에서는:
```cpp
struct sockaddr_in address;
address.sin_family = AF_INET;
address.sin_addr.s_addr = INADDR_ANY;
address.sin_port = htons(PORT);
```
- **sin_family**: IPv4 주소 체계 지정
- **sin_addr.s_addr**: 모든 인터페이스를 통해 들어오는 연결을 수락
- **sin_port**: 네트워크 바이트 순서로 포트 번호 변환

### 연결 수신 대기
```cpp
if (listen(server_fd, 3) < 0) {
    cerr << "Listen error" << endl;
    return -1;
}
```
`listen` 함수는 서버가 클라이언트의 연결 요청을 기다리게 합니다.

### 클라이언트 연결 수락 및 메시지 교환
```cpp
int new_socket, valread;
char buffer[1024] = {0};
const char *hello = "Hello from server";

if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
    cerr << "Accept error" << endl;
    return -1;
}

// 클라이언트로부터 메시지 수신
valread = read(new_socket, buffer, 1024);
cout << buffer << endl;

// 클라이언트에게 메시지 송신
send(new_socket, hello, strlen(hello), 0);
cout << "Hello message sent" << endl;
```
클라이언트 연결 수락 후:

- 클라이언트로부터 메시지 수신 후 출력
- 클라이언트에게 "Hello from server" 메시지 송신 및 전송 확인 메시지 출력

이 코드는 TCP를 활용해서 서버와 클라이언트 간의 간단한 메시지 주고 받기를 구현합니다. TCP의 중요한 특징을 활용하여 정보를 정확하게, 순서대로, 또 빠짐없이 전달할 수 있습니다. 이 방식은 채팅 애플리케이션, 이메일 시스템, 파일 공유 서비스 등 다양한 네트워크 애플리케이션 구축에 사용될 수 있습니다.

## TCP 클라이언트 예제 구현

# TCP 클라이언트 구현 분석

클라이언트와 서버 간의 TCP 통신 구현은 네트워크 프로그래밍의 기본입니다. 이 과정에서, 클라이언트는 서버에 연결하여 데이터를 주고받습니다. 아래 내용은 TCP 클라이언트 구현에 대한 분석을 제공합니다. 상세한 설명과 예제 코드는 [이 블로그](https://a-researcher.tistory.com/122)에서 확인할 수 있습니다.

### 클라이언트 소켓 생성

클라이언트도 서버와 마찬가지로 소켓을 생성하여 통신을 시작합니다. 

- `socket(AF_INET, SOCK_STREAM, 0)`: IPv4 주소 체계를 사용하며, TCP 통신을 위한 스트림 소켓을 생성합니다. 여기서 `0`은 기본 프로토콜을 의미합니다.

### 서버 주소 설정

서버의 IP 주소와 포트 번호를 설정하고, 이를 소켓에 연결합니다.

- `inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)`: "127.0.0.1"로 주어진 서버 IP 주소를 네트워크 바이트 순서의 이진 형태로 변환하고, 이를 서버 주소 구조체에 설정합니다.

### 서버 연결 시도

- `connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr))`: 설정된 서버 주소로 소켓을 연결합니다. 성공적인 연결 후, 클라이언트와 서버 간의 데이터 교환 준비가 완료됩니다.

### 데이터 교환

- 서버로 메시지 전송: `send(sock, hello, strlen(hello), 0)`를 통해 "Hello from client"라는 메시지를 서버로 전송합니다.
- 서버로부터의 응답 수신: `read(sock, buffer, 1024)`를 통해 서버로부터 응답을 수신하고 출력합니다.

### 실행 결과

성공적인 구현을 통해 클라이언트와 서버가 통신을 주고받는 과정을 확인할 수 있습니다. 이 예제를 통해 학습한 TCP/IP 서버-클라이언트 통신의 기본 원리는 다양한 네트워크 기반 애플리케이션 개발의 기반으로 활용될 수 있습니다.


## 2단계: TCP/IP 소켓 채팅 서버 / 클라이언트 + DB 연결

### 기능
- 간단한 채팅 기능을 구현합니다.
- 채팅 기록을 데이터베이스에 저장합니다.
- 로그인 기능은 별도로 구현하지 않습니다.

### 데이터베이스 스키마 디자인
채팅 기록 저장을 위해 다음과 같은 테이블을 사용합니다:

```markdown
- Messages Table
  - MessageID (Primary Key, Auto Increment)
  - UserID (Text)
  - Message (Text)
  - Timestamp (DateTime)
```

## 개발 환경 설정 및 실행 방법
- Visual Studio에서 새 프로젝트를 생성하고, 필요한 설정을 진행합니다.
- 프로젝트에 Winsock2 라이브러리(`ws2_32.lib`)를 추가합니다.
- 적절한 데이터베이스 드라이버 또는 ORM을 프로젝트에 포함시켜 데이터베이스 연동을 구현합니다.

# 구현
##  CHAT 서버 개요
Winsock API를 사용하여 Windows 환경에서 개발된 TCP/IP 기반 채팅 서버인 `ChatServer` 클래스의 구현을 다룹니다. `ChatServer` 클래스는 네트워크 프로그래밍의 핵심 요소를 캡슐화하며, 동시에 여러 클라이언트를 처리하기 위한 멀티 스레딩, 데이터베이스 연결 설정 및 메시지 지속성을 위한 SQL 쿼리 실행 등을 포함합니다.

## 주요 구성 요소와 기능

### 초기화 및 설정
- **Winsock 초기화 (`initializeWinsock`)**: 네트워크 통신을 시작하기 전 필수적인 Winsock DLL 초기화를 진행합니다.
    ```cpp
    void ChatServer::initializeWinsock() {
        WSADATA wsaData;
        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
        // 에러 처리 및 성공 확인
    }
    ```
- **Winsock (Windows Sockets API)**: TCP/IP 네트워크 통신을 위한 표준 API 제공.

### 서버 소켓 설정
- **소켓 생성 및 바인딩 (`setupServerSocket`)**: TCP 소켓 생성과 지정된 포트에 바인딩을 진행, 클라이언트 연결 수락 준비.
    ```cpp
    void ChatServer::setupServerSocket() {
        // 소켓 생성, 바인딩 및 리스닝 설정
    }
    ```

### 클라이언트 연결 처리
- **클라이언트 수신 및 수락 (`listenForClients`)**: 들어오는 클라이언트 연결을 지속적으로 수락하고 각 클라이언트를 별도의 스레드로 처리.
    ```cpp
    void ChatServer::listenForClients() {
        // 지속적으로 새 클라이언트 연결을 수락하고 별도의 스레드에서 처리
    }
    ```
- **클라이언트 통신 (`handleClient`)**: 연결된 클라이언트와의 통신 관리, 메시지 수신 및 데이터베이스 저장, 다른 클라이언트에게 메시지 방송 포함.
    ```cpp
    void ChatServer::handleClient(SOCKET clientSocket) {
        // 메시지 수신, 데이터베이스 저장, 방송 처리
    }
    ```

### 정리
- **자원 관리 (`cleanup`)**: 활성 소켓 종료 및 사용된 자원 정리 보장.
    ```cpp
    void ChatServer::cleanup() {
        // 모든 클라이언트 소켓과 리스닝 소켓을 닫고 Winsock 정리
    }
    ```

### 데이터베이스 상호작용
- **환경 구성 (`loadEnv`)**: `.env` 파일 파싱을 통한 데이터베이스 연결 정보 동적 로드.
    ```cpp
    std::map<std::string, std::string> ChatServer::loadEnv(const std::string& envFilePath);
    ```
- **메시지 지속성 (`saveMessageToDatabase`)**: 채팅 메시지 데이터베이스 저장 구현.
    ```cpp
    void ChatServer::saveMessageToDatabase(const std::string& message);
    ```

## 구현 하이라이트
- 멀티 스레딩을 활용해 여러 클라이언트 연결을 동시에 효율적으로 관리합니다.
- 자원 관리와 오류 처리에 중점을 두어 안정성과 보안을 유지합니다.
- 준비된 명령문 사용을 통해 SQL 주입 방지와 같은 안전한 데이터베이스 상호작용 패턴을 시연합니다.

## CLIENT 개요
TCP/IP 기반 채팅 애플리케이션의 핵심 구성 요소인 `ChatClient` 클래스의 구현을 자세히 다룹니다. 이 클래스는 클라이언트와 서버 간의 통신을 촉진하며, 실시간으로 메시지를 송수신할 수 있게 합니다. Winsock API를 사용한 네트워크 프로그래밍의 핵심 개념, 멀티 스레딩을 통한 비동기 메시지 처리, 그리고 Windows 환경에서 네트워크 연결을 설정하고 관리하는 실제적인 측면을 통해 이해할 수 있습니다.

## 주요 기능 및 메서드

### 초기화 및 연결
- **Winsock 초기화 (`initializeWinsock`)**: 네트워크 통신을 위해 Winsock 라이브러리를 준비합니다. 소켓 작업을 시작하기 전에 필요한 단계입니다.
    ```cpp
    void ChatClient::initializeWinsock();
    ```
- **서버 연결 (`connectToServer`)**: 제공된 IP 주소와 포트 번호를 사용하여 채팅 서버에 TCP 연결을 설정합니다. 소켓 생성 및 서버 연결을 처리하며, 성공적인 연결을 보장합니다.
    ```cpp
    void ChatClient::connectToServer();
    ```

### 메시징
- **메시지 송신 (`sendMessage`)**: 사용자가 생성한 메시지를 채팅 서버로 전송합니다. 메시지를 포맷하고 설정된 소켓 연결을 통해 전송합니다.
    ```cpp
    void ChatClient::sendMessage(const std::string& message);
    ```
- **메시지 수신 (`receiveMessage`)**: 채팅 서버로부터 오는 메시지를 수신하고 처리합니다. 메시지는 실시간으로 사용자에게 표시되어 원활한 채팅 경험을 제공합니다.
    ```cpp
    void ChatClient::receiveMessage();
    ```

### 비동기 메시지 처리
- **수신 스레드 시작 (`startReceiveThread`)**: 서버로부터 메시지를 수신하기 위해 별도의 스레드를 시작하는 메서드입니다. 이를 통해 메인 스레드를 차단하지 않고 비동기 통신을 가능하게 합니다.
    ```cpp
    void ChatClient::startReceiveThread();
    ```

### 정리
- **자원 관리 (`cleanup`)**: 클라이언트 종료 시 네트워크 자원과 Winsock 라이브러리를 적절히 해제합니다. 자원 누수를 방지하고 깨끗한 종료를 보장하는 이 메서드는 매우 중요합니다.
    ```cpp
    void ChatClient::cleanup();
    ```

## 구현 하이라이트
- **Winsock API 사용**: 네트워크 통신을 위한 소켓 작업, 연결 관리, 데이터 전송 등을 시연합니다.
- **멀티 스레딩 활용**: 비동기 메시지 수신을 위해 멀티 스레딩을 사용하여 사용자 경험을 향상시키며, 연속적인 입력 및 상호작용을 가능하게 합니다.
- **오류 처리 및 자원 관리 전략 구현**: 네트워크 작업의 견고함과 신뢰성을 보장합니다.

### 실행 화면
![실행화면](./실행%20화면.PNG)