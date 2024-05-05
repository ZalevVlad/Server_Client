#ifndef SERVER_CLIENT_CLIENT_H
#define SERVER_CLIENT_CLIENT_H

#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <mutex>
#include <thread>
#include <ncurses.h>

class Client {
    int _socket;
    sockaddr_in _serverAddress;
    char _buff[1024];
    std::mutex _consoleMutex;

     std::mutex _coutMutex;
     std::mutex _cinMutex;

    std::string _inputBuffer;

    void readMessages() {
        char buffer[1024];
        while (true) {
            ssize_t bytesReceived = recv(_socket, buffer, 1024, MSG_DONTWAIT);
            if (bytesReceived > 0) {
                buffer[bytesReceived] = '\0';

                _coutMutex.lock();
                std::cout << buffer << std::endl;
                _coutMutex.unlock();
            }
        }
    }

public:
    Client(in_addr_t s_addr, u_int16_t sin_port) : _socket(socket(AF_INET, SOCK_STREAM, 0)) {
        _serverAddress.sin_family = AF_INET;
        _serverAddress.sin_addr.s_addr = s_addr;
        _serverAddress.sin_port = sin_port;

        if (connect(_socket, (sockaddr*)&_serverAddress, sizeof(_serverAddress)) == -1) {
            throw std::runtime_error("Connection to server failed");
        }
    }

    void run(){
        std::thread readMessagesThread(&Client::readMessages, this);

        std::string message =  "";
        while (message!="q") {
//            _coutMutex.lock();
//            std::cout << "> ";
//            _coutMutex.unlock();

            _cinMutex.lock();
            std::getline(std::cin, message);
            _cinMutex.unlock();

//            _inputBuffer += message + "\n"; // new
//            _consoleMutex.lock();
//            std::cout << "> " << _inputBuffer;
//            _consoleMutex.unlock();

            send(_socket, message.c_str(), message.length() + 1, 0);
//            _inputBuffer.clear(); // new
        }

        readMessagesThread.join();
        close(_socket);
    }
};

#endif //SERVER_CLIENT_CLIENT_H
