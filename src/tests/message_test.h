#ifndef SERVER_CLIENT_MESSAGE_TEST_H
#define SERVER_CLIENT_MESSAGE_TEST_H

#include <limits>

#include "../message.h"

namespace message_test{

    TEST(message_string_ctor, 1){
        Message msg(10, std::string ("Haiai"));
        const char* cStr = msg.bytes();
        const char res[] = {Message::TEXT,'H','a', 'i','a','i','\0'};
        ASSERT_EQ(msg.type(), Message::TEXT);
        ASSERT_STREQ(cStr, res);
    }

    TEST(message_table_ctor, 1){
        Message msg(100,"aaabbb", {{'a',3},{'b',3}});
        const char* cStr = msg.bytes();
        const char res[] = {Message::SYM_TABLE,
                            6, 'a','a','a','b','b','b',
                            2,'a',3,'b',3,'\0'};
        ASSERT_EQ(msg.type(), Message::SYM_TABLE);
        ASSERT_STREQ(cStr, res);
    }

    TEST(message_table_ctor, 2){
        Message msg(100, "", std::vector<std::pair<char,u_char>>{});
        const char* cStr = msg.bytes();
        const char res[] = {Message::SYM_TABLE,'\0'};
        ASSERT_EQ(msg.type(), Message::SYM_TABLE);
        ASSERT_STREQ(cStr, res);
    }

    TEST(message_connections_ctor, 1){
        Message msg(10, 100);
        const char* cStr = msg.bytes();
        const char res[] = {Message::CONNECTIONS,100,'\0'};
        ASSERT_EQ(msg.type(), Message::CONNECTIONS);
        ASSERT_STREQ(cStr, res);
    }

    TEST(message_connections_ctor, 2){
        Message msg(10, 255);
        const char* cStr = msg.bytes();
        const char res[] = {Message::CONNECTIONS,(char)0xff,'\0'};
        ASSERT_EQ(msg.type(), Message::CONNECTIONS);
        ASSERT_STREQ(cStr, res);
    }

    TEST(say_text,1){
        Message msg(10,"haiai");
        ASSERT_EQ(Message::say(msg.bytes()),
                  std::string("haiai"));
    }

    TEST(say_table,1){
        Message msg(10,"haiai");
        ASSERT_EQ(Message::say(msg.bytes()),
                  std::string("haiai"));
    }

    TEST(say_connections,1){
        Message msg(10,0);
        ASSERT_EQ(Message::say(msg.bytes()),
                  std::string("Num of connections is 0"));
    }

    TEST(say_connections,2){
        Message msg(10,18446744073709551615);
        ASSERT_EQ(Message::say(msg.bytes()),
                  std::string("Num of connections is 18446744073709551615"));
    }

} // namespace message_test

#endif //SERVER_CLIENT_MESSAGE_TEST_H
