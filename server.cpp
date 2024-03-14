#define MAX_SIZE 1024
#define MAX_CLIENT 5

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <istream>
#include <unistd.h>

#define PORT 3000

typedef unsigned int *SOCKET;

using namespace std;

struct SOCK_INFO {
	SOCKET sock = 0;
	string user = "";
} SOCK_INFO;

vector<SOCK_INFO> sock_list;
SOCKET_INFO sock_info;
int client_count = 0;
