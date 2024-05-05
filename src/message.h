#ifndef SERVER_CLIENT_MESSAGE_H
#define SERVER_CLIENT_MESSAGE_H

#include <vector>
#include <map>
#include <cstring> // std::strlen
#include <sstream>

constexpr int TEXT_LIMIT = 256; // CHANGING THIS IS UB
constexpr int MESSAGE_SIZE = 1024;

class Message {
    /*
    Depending on the type of message, the internal structure of
        _msg will be organized as follows (assuming that char occupies
        exactly one byte):
    The first byte always describes the message type: 0 - ERR,
        1 - TEXT, 2 - SYM_TABLE, 3 - CONNECTIONS, then...
    TEXT: null terminated string.
    SYM_TABLE: one byte stores the length of the string - n;
        then comes the string itself with length n; then one byte
        describes the table size; then there are pairs of bytes,
        the first byte of the pair is a character, the second byte
        is an unsigned char number.
    CONNECTIONS: sizeof(std::size_t) bytes describing the number of
        connections.
     */
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

    static std::string bytesToTableMsg(const char *bytes);

    static std::string bytesToConnectionsMsg(const char *bytes);

public:
    enum msg_type {
        ERR,
        TEXT,
        SYM_TABLE,
        CONNECTIONS
    };

    /// \param max_size maximal size of message in bytes (>=1)
    /// \param text text
    Message(std::size_t max_size, const std::string &text) : Message(max_size) {
        _msgType = TEXT;
        _msg.insert(_msg.end(), _msgType);

        std::size_t max_symbols = std::min(text.length(), unused() - 1);
        _msg.insert(_msg.end(), text.begin(), text.begin() + max_symbols);
        _msg.insert(_msg.end(), '\0');
    }

    /// \param max_size maximal size of message in bytes (>=2)
    /// \param table_header table header (length must be <= TEXT_LIMIT)
    /// \param table pairs of symbols and the number of times they appear
    Message(std::size_t max_size,
            const std::string &table_header,
            const std::vector<std::pair<char, u_char>> table) : Message(max_size) {
        if(1+1+table_header.size()+1+table.size()*2>=TEXT_LIMIT){
            _msgType = ERR;
            _msg.insert(_msg.end(), _msgType);
            auto error_msg = "Error of message size";
            _msg.insert(_msg.end(), error_msg, error_msg + std::strlen(error_msg));
            _msg.insert(_msg.end(), '\0');
        }
        else{
            _msgType = SYM_TABLE;
            // stores message type
            _msg.insert(_msg.end(), _msgType);

            // stores header size
            _msg.insert(_msg.end(),static_cast<u_char>(table_header.size()));

            // stores header
            std::size_t max_symbols = std::min(table_header.length(), unused() - 1);
            _msg.insert(_msg.end(), table_header.begin(), table_header.begin() + max_symbols);

            // stores table_size
            std::size_t max_rows = std::min(table.size(), (unused() - 1) / 2);
            _msg.insert(_msg.end(), static_cast<u_char>(max_rows));

            //stores table
            for (std::size_t i = 0; i < max_rows; i++) {
                _msg.insert(_msg.end(), table[i].first);
                _msg.insert(_msg.end(), table[i].second);
            }
            _msg.insert(_msg.end(), '\0');
        }
    }

    /// \param max_size maximal size of message in bytes (>= 1+sizeof(std::size_t))
    /// \param connections number of connections to say
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

    static std::string say(const char *bytes);

    const char *bytes() const {
        return _msg.data();
    }

    std::size_t length() const {
        return _msg.size();
    }

    char type() const {
        return _msgType;
    }
};


#endif //SERVER_CLIENT_MESSAGE_H
