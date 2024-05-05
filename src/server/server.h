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
#include <atomic>

#include "../message.h"

namespace server {
    volatile std::atomic<bool> _serverInterrupt = false;

    void signalHandler(int sig) {
        _serverInterrupt.store(true);
        std::cout << "Server is stopping..." << std::endl;
    };
}

class Server {
    int _socket;
    sockaddr_in _address;

    std::vector<int> _clients;
    std::mutex _clientsMutex;


    char _buffer[MESSAGE_SIZE];

    void close_sockets() {
        for (int sock: _clients) {
            close(sock);
        }
        close(_socket);
    }

    void acceptClients(int serverSocket) {
        while (server::_serverInterrupt.load() != true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            int clientSocket = accept(serverSocket, NULL, NULL);
            if (clientSocket != -1) {
                std::cout << "Client [" << clientSocket << "] connected" << std::endl;
                _clientsMutex.lock();
                _clients.push_back(clientSocket);
                _clientsMutex.unlock();
            }
        }
        std::cout << "Stopping accepting clients" << std::endl;
    }

    /// \brief Send message all clients but mutedSocket
    /// \param mutedSocket a socket that won't get the message
    /// \param clients list of clients sockets
    /// \param msg message to send
    template<class Container>
    void replyAllExcept(int mutedSocket, Container clients, std::string text) {
        for (int sock_mailing: clients) {
            if (sock_mailing != mutedSocket) {
                Message msg(MESSAGE_SIZE, text);
                send(sock_mailing, msg.bytes(), msg.length() + 1, MSG_NOSIGNAL);
            }
        }
    }

    static void countSymbols(const char *text, std::vector<std::pair<char, u_char>> &counter) {
        counter.resize(0);

        std::map<char, u_char> map;
        while (*text) {
            if (map.count(*text) == 0) {
                map[*text] = 1;
                counter.push_back(std::pair(*text, 0));
            } else {
                map[*text]++;
            }
            text++;
        }

        for (auto& pair: counter) {
            pair.second = map[pair.first];
        }
    }

    void handleClientCommand(int clientSocket, const char *command) {
        if (std::strncmp(command, "/parse ", 7) == 0) {
            std::vector<std::pair<char, u_char>> counter;
            countSymbols(command + 7, counter);
            Message msg(MESSAGE_SIZE, command + 7, counter);
            send(clientSocket, msg.bytes(), msg.length(), 0);
        } else if (std::strncmp(command, "/connections", 12) == 0) {
            Message msg(MESSAGE_SIZE, connections());
            send(clientSocket, msg.bytes(), msg.length(), 0);
        } else {
            Message msg(MESSAGE_SIZE, "[Server]: Wrong command");
            send(clientSocket, msg.bytes(), msg.length(), 0);
        }
    };

    void handleRequests() {
        while (server::_serverInterrupt.load() != true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));

            _clientsMutex.lock();
            for (int sock: _clients) {
                ssize_t bytesReceived = recv(sock, _buffer, MESSAGE_SIZE, MSG_DONTWAIT);
                if (bytesReceived > 0) {
                    _buffer[bytesReceived] = '\0';

                    if (std::strncmp(_buffer, "/", 1) == 0) {
                        handleClientCommand(sock, _buffer);
                    } else {
                        std::string message = std::to_string(sock) + ": " + std::string(_buffer);
                        std::cout << message << std::endl;
                        replyAllExcept(sock, _clients, message);
                    }
                }
            }
            _clientsMutex.unlock();
        }
        std::cout << "Stopping handle requests" << std::endl;
    }

    std::size_t connections() {
        return _clients.size();
    }

public:
    Server(in_addr_t s_addr, u_int16_t sin_port) : _socket(socket(AF_INET, SOCK_STREAM, 0)) {
        _address.sin_family = AF_INET;
        _address.sin_addr.s_addr = s_addr;
        _address.sin_port = sin_port;

        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

        int opt = 1;
        if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
            throw std::runtime_error("Server socket setsockopt error");
        }

        if (bind(_socket, (sockaddr *) &_address, sizeof(_address)) != 0) {
            throw std::runtime_error("Server socket bind error");
        }
    }

    void run() {
        listen(_socket, SOMAXCONN);

        std::thread acceptThread(&Server::acceptClients, this, _socket);
        std::thread handleThread(&Server::handleRequests, this);

        signal(SIGINT, server::signalHandler);

        acceptThread.join();
        handleThread.join();

        close_sockets();
    }
};

#endif //SERVER_CLIENT_SERVER_H
