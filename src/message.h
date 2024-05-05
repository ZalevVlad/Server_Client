#ifndef SERVER_CLIENT_MESSAGE_H
#define SERVER_CLIENT_MESSAGE_H

#include <vector>
#include <map>
#include <cstring> // std::strlen
#include <sstream>

constexpr int TEXT_LIMIT = 256; // CHANGING THIS IS UB

class Message {
    std::vector<char> _msg;
    char _msgType;

    std::size_t _maxSize;
    std::size_t _msgBytes;

    Message(std::size_t max_size) : _maxSize(max_size) {
        _msg.reserve(max_size);
    }

    std::size_t unused() const {
        return _msg.capacity() - _msg.size();
    }

    const char *bytesToTableMsg(const char *bytes);

    const char *bytesToConnectionsMsg(const char *bytes);

public:
    enum msg_type {
        ERR,
        TEXT,
        SYM_TABLE,
        CONNECTIONS
    };

    Message(std::size_t max_size, const std::string &text, bool parse = false) : Message(max_size) {
        _msgType = TEXT;
        _msg.insert(_msg.end(), _msgType);

        std::size_t max_symbols = std::min(text.length(), unused() - 1);
        _msg.insert(_msg.end(), text.begin(), text.begin() + max_symbols);
        _msg.insert(_msg.end(), '\0');
    }

    Message(std::size_t max_size, const std::string &text, const std::vector<std::pair<char, u_char>> table) : Message(
            max_size) {
        _msgType = SYM_TABLE;
        _msg.insert(_msg.end(), _msgType);

        std::size_t max_rows = std::min(table.size(), (unused() - 1) / 2);
        for (std::size_t i = 0; i < max_rows; i++) {
            _msg.insert(_msg.end(), table[i].first);
            _msg.insert(_msg.end(), table[i].second);
        }
        _msg.insert(_msg.end(), '\0');
    }

    Message(std::size_t max_size, std::size_t connections) : Message(max_size) {
        _msgType = CONNECTIONS;
        _msg.insert(_msg.end(), _msgType);

        if (unused() > sizeof(std::size_t)) {
            u_char *bytes = reinterpret_cast<u_char *>(&connections);
            _msg.insert(_msg.end(), bytes, bytes + sizeof(std::size_t));
        } else {
            _msgType = ERR;
        }
    }

    static const char *say(const char *bytes) {
        std::vector<char> msg(bytes, bytes + std::strlen(bytes));
        char msgType = msg[0];
        switch (msgType) {
            case Message::TEXT:
                return ++bytes;
            case Message::SYM_TABLE:
                return "";
            case Message::CONNECTIONS:
                return "";
            default:
                return "Server: Error";
        }
    }

    const char *c_str() {
        return _msg.data();
    }

    const char type() const {
        return _msgType;
    }
};


#endif //SERVER_CLIENT_MESSAGE_H
