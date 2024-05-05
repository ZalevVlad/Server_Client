#include "client.h"

int main(int argc, char* argv[]) {
    if(argc != 3){
        std::cout << "Wrong parameters";
        exit(0);
    }
    try{
        Client client(inet_addr(argv[1]),htons(atoi(argv[2])));
        std::cout << "Connected to server" << std::endl;
        client.run();
        std::cout << "Client stopped" << std::endl;
    }
    catch (const std::exception& e){
        std::cout << "Connection error: " << e.what();
    }
    return 0;
}
