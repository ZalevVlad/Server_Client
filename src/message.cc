#include "message.h"

std::string Message::say(const char *bytes) {
    std::vector<char> msg(bytes, bytes + (1+std::strlen(bytes+1)));
    char msgType = msg[0];
    switch (msgType) {
        case Message::TEXT:
            return ++bytes;
        case Message::SYM_TABLE:
            return bytesToTableMsg(bytes);
        case Message::CONNECTIONS:
            return bytesToConnectionsMsg(bytes);
        default:
            return ++bytes;
    }
}

std::string Message::bytesToTableMsg(const char *bytes) {
    std::stringstream ss;
    ss << "Message | ";
    const char *ptr = bytes + 1;

    u_char header_size = *ptr;
    ptr++;
    const char *header = ptr;
    ss.write(header, header_size);
    ss << '\n';
    ptr += header_size;

    u_char table_size = *ptr;
    ptr++;
    for (std::size_t i = 0; i < table_size; i++) {
        ss << *(ptr++) << "       | " ;
        ss << (unsigned int)(u_int8_t)(*(ptr++)) << '\n';
    }

    return ss.str();
}

std::string Message::bytesToConnectionsMsg(const char *bytes) {
    std::stringstream ss;
    const char *size_t_ptr = bytes + 1;
    std::size_t value = *reinterpret_cast<const std::size_t *>(size_t_ptr);
    ss << "Num of connections is " << value;

    return ss.str();
}

