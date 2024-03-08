cmake_minimum_required(VERSION 3.27)
project(cpp_socket_server)

set(CMAKE_CXX_STANDARD 17)

add_executable(cpp_socket_server main.cpp
				client.cpp)
