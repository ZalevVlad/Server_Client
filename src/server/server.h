#ifndef SERVER_CLIENT_SERVER_H
#define SERVER_CLIENT_SERVER_H

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <csignal>

#include "../message.h"

namespace server{
    volatile bool _serverInterrupt = false;
    void signalHandler(int sig){
        _serverInterrupt = true;
    };
}

class Server{
    int _socket;
    sockaddr_in _address;
    char _buff[1024];

    std::vector<int> _clients;
    std::mutex _clientsMutex;

//TODO
// - reply functions
// - 3 threads: accepting connections, reading

    void signal_handler(int signal) {
        std::cout << "\nServer is stopping...\n";

        // Закрываем все сокеты
        for (int sock : _clients) {
            close(sock);
        }

        // Закрываем серверный сокет
        close(_socket);
    }

    void acceptClients(int serverSocket) {
        while (!server::_serverInterrupt) {
            int clientSocket = accept(serverSocket, NULL, NULL);
            std::cout << "Client connected." << std::endl;

            _clientsMutex.lock();
            _clients.push_back(clientSocket);
            _clientsMutex.unlock();
        }
    }

    void handleRequests() {
        while (!server::_serverInterrupt) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            std::vector<int> clientsCopy;
            _clientsMutex.lock();
            clientsCopy = _clients;
            _clientsMutex.unlock();

            for (int sock : clientsCopy) {
                char buffer[1024];
                ssize_t bytesReceived = recv(sock, buffer, 1024, MSG_DONTWAIT);

                if (bytesReceived > 0) {
                    buffer[bytesReceived] = '\0';

                    std::string message = std::to_string(sock) + ": " + std::string(buffer);
                    std::cout << message << std::endl;

//                    send(sock, message.c_str(), message.length()+1,0);

                    for (int sock_mailing: clientsCopy){
                        if(sock_mailing!=sock){
                            send(sock_mailing, message.c_str(), message.length() + 1, 0);
                        }
                    }
                }
            }
        }
    }


public:
    Server(in_addr_t s_addr, u_int16_t sin_port) : _socket(socket(AF_INET, SOCK_STREAM, 0)){
        _address.sin_family = AF_INET;
        _address.sin_addr.s_addr = s_addr;
        _address.sin_port = sin_port;

        bind(_socket, (sockaddr*)&_address, sizeof(_address));
    }

    void run(){
        listen(_socket, SOMAXCONN);

        signal(SIGINT, server::signalHandler);
//        std::signal(SIGINT, std::bind(&Server::signal_handler, this, std::placeholders::_1));

        std::thread acceptThread(&Server::acceptClients,this, _socket);
        std::thread handleThread(&Server::handleRequests,this);

        acceptThread.join();
        handleThread.join();
    }
};

#endif //SERVER_CLIENT_SERVER_H
