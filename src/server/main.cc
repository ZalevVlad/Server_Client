#include "server.h"

int main() {
    try{
        Server server(INADDR_ANY, htons(3000));
        std::cout << "Server started" << std::endl;
        server.run();
        std::cout << "Server stopped" << std::endl;
    }
    catch(const std::exception& e){
        std::cout << "Server starting error: " << e.what();
    }
   return 0;
}
