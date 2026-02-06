#pragma once
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <string>
#include <Windows.h>//need put it under winsock2.h...  :(
#pragma comment(lib, "ws2_32.lib")
#define DEFAULT_BUFFER_SIZE 512

class Client {
public:
    bool login = false;
    bool sameName = false;
    //bool sendAll = true;
    const int port = 8080;
    const char* host = "127.0.0.1";
    SOCKET client_socket;
    bool inputNew = false;
    bool receiveNew = false;
    std::string sendMessage;
    std::string receiveMessage;
    std::string name = "unknown";
    Client() {}
    ~Client() {
        closesocket(client_socket);
        WSACleanup();
    }
    bool tryConnect() {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "WSAStartup failed with error: " << WSAGetLastError() << std::endl;
            return false;
        }
        client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (client_socket == INVALID_SOCKET) {
            std::cerr << "Socket creation failed with error: " << WSAGetLastError() << std::endl;
            WSACleanup();
            return false;
        }
        sockaddr_in server_address = {};
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(port);
        if (inet_pton(AF_INET, host, &server_address.sin_addr) <= 0) {
            std::cerr << "Invalid address/ Address not supported" << std::endl;
            closesocket(client_socket);
            WSACleanup();
            return false;
        }
        if (connect(client_socket, reinterpret_cast<sockaddr*>(&server_address), sizeof(server_address)) == SOCKET_ERROR) {
            std::cerr << "Connection failed with error: " << WSAGetLastError() << std::endl;
            closesocket(client_socket);
            WSACleanup();
            return false;
        }
        else {
            std::cout << "Connected" << std::endl;
            return true;
        }
    }
    bool sendName(std::string id) {
        //send name first
        if (send(client_socket, id.c_str(), static_cast<int>(id.size()), 0) == SOCKET_ERROR) {
            std::cerr << "Send failed with error: " << WSAGetLastError() << std::endl;
            return false;
        }
        /*int framecount = 10000;
        while (framecount > 0) {
            framecount--;
            if (login) {
                return true;
            }
        }*/
        return false;
        //while (true) {
        //    char buffer[DEFAULT_BUFFER_SIZE] = { 0 };
        //    int bytes_received = recv(client_socket, buffer, DEFAULT_BUFFER_SIZE - 1, 0);
        //    if (bytes_received > 0) {
        //        buffer[bytes_received] = '\0'; // Null-terminate the received data
        //        std::string recv = std::string(buffer);
        //        std::cout << recv << std::endl;
        //        if (recv == "#system:Welcome-" + id) return true;
        //        else return false;
        //    }
        //    else if (bytes_received == 0) {
        //        std::cout << "\nConnection closed by server." << std::endl;
        //        return false;
        //    }
        //    else {
        //        std::cerr << "\nReceive failed with error: " << WSAGetLastError() << std::endl;
        //        return false;
        //    }
        //}
        return true;
    }
    void startReceive() {
        std::thread receiveThread([this] {
            while (true) {
                char buffer[DEFAULT_BUFFER_SIZE] = { 0 };
                int bytes_received = recv(client_socket, buffer, DEFAULT_BUFFER_SIZE - 1, 0);
                if (bytes_received > 0) {
                    buffer[bytes_received] = '\0'; // Null-terminate the received data
                    receiveMessage = std::string(buffer);
                    receiveNew = true;//use bool to notify window.
                    std::cout << "startReceive:" << receiveMessage << std::endl;

                    if (receiveMessage.compare(0, 14, "#system:login-") == 0) {
                        login = true;
                        name = receiveMessage.substr(14, receiveMessage.size() - 1);
                    }

                    if (receiveMessage.compare(0, 16, "#system:SameName") == 0) {
                        sameName = true;
                    }
                }
                else if (bytes_received == 0) {
                    std::cout << "\nConnection closed by server." << std::endl;
                    return;
                }
                else {
                    std::cerr << "\nReceive failed with error: " << WSAGetLastError() << std::endl;
                    return;
                }
            }
        });
        receiveThread.detach();
    }
    void startSend() {

        std::thread sendThread([this] {
            while (true) {
                //std::cout << "thread is working" << std::endl;
                if (inputNew) {
                    std::cout << "Client send message: " << sendMessage << std::endl;
                    inputNew = false;
                    if (send(client_socket, sendMessage.c_str(), static_cast<int>(sendMessage.size()), 0) == SOCKET_ERROR) {
                        std::cerr << "Send failed with error: " << WSAGetLastError() << std::endl;
                        return;
                    }
                }
        }});
        sendThread.detach();
    }
};
