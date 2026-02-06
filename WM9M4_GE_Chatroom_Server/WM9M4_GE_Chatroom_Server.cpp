#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <thread>
#include <string>
#include <vector>
#include <mutex>
#pragma comment(lib, "ws2_32.lib")

#define DEFAULT_BUFFER_SIZE 512

class Client {
public:
    SOCKET client_socket;
    char client_ip[INET_ADDRSTRLEN];
    std::string id = "UnknownUser";
    bool isFirstMessage = true;
    Client(SOCKET socket, const char* ip) : client_socket(socket) {
        strncpy_s(client_ip, ip, INET_ADDRSTRLEN);
    }
};
std::mutex client_mutex;
void clientConnect(std::vector<std::shared_ptr<Client>>& clients, SOCKET server_socket) {
    while (true) {

        sockaddr_in client_address = {};
        int client_address_len = sizeof(client_address);
        SOCKET client_socket = accept(server_socket, (sockaddr*)&client_address, &client_address_len);

        if (client_socket != INVALID_SOCKET) {
            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &client_address.sin_addr, client_ip, INET_ADDRSTRLEN);
            auto client = std::make_shared<Client>(client_socket, client_ip);

            char buffer[DEFAULT_BUFFER_SIZE] = { 0 };

            int bytes_received = recv(client_socket, buffer, DEFAULT_BUFFER_SIZE - 1, 0);
            if (bytes_received > 0) {
                buffer[bytes_received] = '\0'; // Null-terminate the received data
                std::cout << "Client connected --- " << "name: " << buffer << " connected" << std::endl;
                client->id = std::string(buffer);
                std::lock_guard<std::mutex> lock(client_mutex);//protect shared resource
                clients.push_back(client);
                std::string message = "Welcome to the server!";
                if (send(client->client_socket, message.c_str(), static_cast<int>(message.size()), 0) == SOCKET_ERROR) {
                    std::cerr << "Send failed with error: " << WSAGetLastError() << std::endl;
                }
                std::cout << "number of clients: " << clients.size() << std::endl;
            }
            else if (bytes_received == 0) {
                closesocket(client_socket);
                std::cout << "Connection closed" << std::endl;
            }
            else {
                std::cerr << "Receive failed with error: " << WSAGetLastError() << std::endl;
                closesocket(client_socket);
            }


        }

    }
}
std::string TargetSelect(std::string& buf) {
    std::string result = "Invalid Message";
    int pos1 = buf.find('#');
    if (pos1 == std::string::npos) {
        buf = result;
        return "Invalid Message";
    }
    int pos2 = buf.find('#', pos1 + 1);
    if (pos2 == std::string::npos) {
        buf = result;
        return "Invalid Message";
    }
    result = buf.substr(pos1 + 1, pos2 - pos1 - 1);
    buf = buf.substr(pos2 + 1, buf.size() - 1);
    return result;
}

void BroadcastMsg(std::vector<std::shared_ptr<Client>> clients, std::string& sendmsg) {
    std::vector<std::shared_ptr<Client>> broadcast_list;
    {
        std::lock_guard<std::mutex> lock(client_mutex);
        broadcast_list = clients;
    }
    //sendmsg = "#all#" + sendmsg;
    for (auto& other : broadcast_list) {
        send(other->client_socket, sendmsg.c_str(), sendmsg.size(), 0);
    }
    std::cout << "BroadcastMsg : " << sendmsg << std::endl;
}
void SendToClient(std::vector<std::shared_ptr<Client>> clients, std::string& sendmsg, std::string target) {
    std::vector<std::shared_ptr<Client>> list;
    {
        std::lock_guard<std::mutex> lock(client_mutex);
        list = clients;
    }
    //sendmsg = "#" + target + "#" + sendmsg;
    for (auto& client : list) {
        if(target == client->id){
            std::cout << "send to "<< client->id << " : " << sendmsg << std::endl;
            send(client->client_socket, sendmsg.c_str(), sendmsg.size(), 0);
            return;
        }
    }
    std::cout << "user does not exist" << std::endl;
}
std::string allUsersNameStr(std::vector<std::string> users) {
    std::string allUsersNames;
    //allUsersNames += std::to_string(users.size());
    for (int i = 0; i < users.size() - 1; i++) {
        allUsersNames += '#';
        allUsersNames += users[i];
    }
    return allUsersNames;
}
int main() {
    std::cout << "Hello This is Server!\n";
    const int port = 8080;
    const char* host = "127.0.0.1";
    std::vector<std::string> users;


    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed with error: " << WSAGetLastError() << std::endl;
        return -1;
    }
    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed with error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return -1;
    }

    sockaddr_in server_address = {};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;
    inet_pton(AF_INET, host, &server_address.sin_addr);
    if (bind(server_socket, (sockaddr*)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
        std::cerr << "Bind failed with error: " << WSAGetLastError() << std::endl;
    }

    if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed with error: " << WSAGetLastError() << std::endl;
    }


    //connecting clients
    std::vector<std::shared_ptr<Client>> clients;

    fd_set readfds;
    while (true) {

        FD_ZERO(&readfds);
        FD_SET(server_socket, &readfds);

        SOCKET maxfd = server_socket;

        {
            std::lock_guard<std::mutex> lock(client_mutex);
            for (auto& c : clients) {
                FD_SET(c->client_socket, &readfds);
                if (c->client_socket > maxfd)
                    maxfd = c->client_socket;
            }
        }

        int activity = select(
            (int)maxfd + 1,
            &readfds,
            NULL,
            NULL,
            NULL
        );

        if (activity == SOCKET_ERROR) {
            std::cerr << "select error: " << WSAGetLastError() << std::endl;
            continue;
        }

        if (FD_ISSET(server_socket, &readfds)) {
            sockaddr_in client_addr{};
            int len = sizeof(client_addr);
            SOCKET new_socket = accept(server_socket, (sockaddr*)&client_addr, &len);

            char ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &client_addr.sin_addr, ip, INET_ADDRSTRLEN);

            auto c = std::make_shared<Client>(new_socket, ip);
            {
                std::lock_guard<std::mutex> lock(client_mutex);
                clients.push_back(c);
            }

            std::cout << "New client connected: " << ip << std::endl;
        }

        std::vector<std::shared_ptr<Client>> snapshot;
        {
            std::lock_guard<std::mutex> lock(client_mutex);
            snapshot = clients;
        }

        for (auto& client : snapshot) {
            SOCKET s = client->client_socket;
            if (FD_ISSET(s, &readfds)) {

                char buffer[DEFAULT_BUFFER_SIZE];
                int bytes = recv(s, buffer, DEFAULT_BUFFER_SIZE - 1, 0);

                if (bytes <= 0) {
                    closesocket(s);
                    std::cout << "Client disconnected: " << client->id << std::endl;
                    std::string disconnectID = client->id;
                    {
                        std::lock_guard<std::mutex> lock(client_mutex);
                        clients.erase(
                            std::remove_if(clients.begin(), clients.end(),
                                [&](auto& ptr) { return ptr->client_socket == s; }), clients.end()
                        );
                        snapshot = clients;
                    }
                    std::string disconnectmsg = "#system:Disconnect-" + disconnectID;
                    BroadcastMsg(snapshot, disconnectmsg);
                    //BroadcastMsg(snapshot, disconnectmsg);
                }
                else {
                    buffer[bytes] = '\0';
                    std::string msg(buffer);
                    std::cout << "received : " << msg << std::endl;
                    if (client->isFirstMessage == true) {
                        std::string loginId = msg;
                        bool exists = (std::find(users.begin(), users.end(), loginId) != users.end());//same name
                        if (!exists) {//name unique
                            users.push_back(loginId);
                            client->id = loginId;
                            client->isFirstMessage = false;
                            std::cout << "Client " << client->id << " connected to this server " << std::endl;
                            std::string welcome_msg = "#system:Welcome-" + client->id;
                            //std::cout << "broatcast:" << welcome_msg << std::endl;
                            BroadcastMsg(snapshot, welcome_msg);

                            std::string canLogin = "#system:login-" + loginId;
                            send(client->client_socket, canLogin.c_str(), canLogin.size(), 0);
                            Sleep(50);//make sure we do split messages :(
                            if (users.size() != 1) {
                                std::string usersNameStr = "#system:Users-" + allUsersNameStr(users);
                                send(client->client_socket, usersNameStr.c_str(), usersNameStr.size(), 0);
                            }
                            continue;
                        }
                        else {
                            std::string multiName_msg = "#system:SameName";
                            send(client->client_socket, multiName_msg.c_str(), multiName_msg.size(), 0);
                            continue;
                        }
                        
                    }

                    //buffer[bytes] = '\0';
                    

                    std::string target = TargetSelect(msg);
                    //std::cout << target << std::endl;
                    //std::string sendmsg = client->id + ": " + msg;
                    std::string sendmsg = client->id + ":" + "#" + target + "#" + msg;
                    if(target == "all")
                        BroadcastMsg(snapshot, sendmsg);
                    else {
                        SendToClient(snapshot, sendmsg, target);
                    }
                }
            }
        }
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}
