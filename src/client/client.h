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
#include <atomic>
#include <csignal>

#include "../message.h"

class Client {
    int _socket;
    sockaddr_in _serverAddress;
    char _buff[MESSAGE_SIZE];

    std::string _inputBuffer;

    std::atomic<bool> _connected = false;
    std::atomic<bool> _continue = true;

    socklen_t _saddrSize;

    void readMessages() {
        while (_continue.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            ssize_t bytesReceived = recv(_socket, _buff, MESSAGE_SIZE, MSG_DONTWAIT);
            if (bytesReceived > 0) {
                _buff[bytesReceived] = '\0';
                std::cout << Message::say(_buff) << std::endl;
            }
        }
    }

    void reconnect() {
        close(_socket);
        _socket = socket(AF_INET, SOCK_STREAM, 0);
        if(_socket == -1){
            throw std::runtime_error("Reconnect error: bad socket");
        }

        while (_continue.load()) {
            std::cout << "Trying to reconnect..." << std::endl;
            int res = connect(_socket, (sockaddr *) &_serverAddress, _saddrSize);
            if (res == 0) {
                std::cout << "Reconnected" << std::endl;
                _connected.store(true);
                break;
            }
            std::cout << "Error code: " << errno << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    }

    void waitForQuit() {
        while (_connected == false) {
            std::getline(std::cin, _inputBuffer);
            if (_inputBuffer == "q") {
                _continue.store(false);
                break;
            }
        }
    }

public:
    Client(in_addr_t s_addr, u_int16_t sin_port) : _socket(socket(AF_INET, SOCK_STREAM, 0)) {
        _serverAddress.sin_family = AF_INET;
        _serverAddress.sin_addr.s_addr = s_addr;
        _serverAddress.sin_port = sin_port;

        _saddrSize = sizeof(_serverAddress);

        if (connect(_socket, (sockaddr *) &_serverAddress, _saddrSize) == -1) {
            throw std::runtime_error("Connection to server failed");
        }
        _connected.store(true);
    }

    void run() {
        std::thread readMessagesThread(&Client::readMessages, this);

        while (_continue) {
            std::getline(std::cin, _inputBuffer);
            if (_inputBuffer == "q") {
                _continue = false;
                std::cout << "Stopping..." << std::endl;
            } else {
                if (getpeername(_socket, (sockaddr *) &_serverAddress, &_saddrSize) < 0) {
                    _connected.store(false);
                }

                if (_connected) {
                    std::size_t msg_size = std::strlen(_inputBuffer.c_str());
                    std::size_t bytesSent = send(_socket, _inputBuffer.c_str(), msg_size, MSG_NOSIGNAL);
                    if (bytesSent != msg_size && (errno == EPIPE || errno == ECONNRESET)) {
                        _connected.store(false);
                    }
                }

                if (_connected.load() == false){
                    std::cout << "Lost connection to the server" << std::endl;
                    std::thread waitForQuitThread(&Client::waitForQuit, this);
                    waitForQuitThread.detach();

                    reconnect();
                }
            }
        }
        readMessagesThread.join();
        close(_socket);
    }

};

#endif //SERVER_CLIENT_CLIENT_H
