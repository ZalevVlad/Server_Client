# Server_Client
Simple implementation of applications: client and server; in C++. 
Applications communicate via the TCP/IP protocol.
## Client
A console application that takes the IP and port of the server from the command line arguments and connects to the server. 
In case of successful connection, the corresponding message is displayed and the command is waiting for commands to be sent to the server. 
In case of an error, the application displays a message and terminates. 
In case of connection loss, the application will try to reconnect. 
## Server
A console application that allows you to connect multiple clients simultaneously and process commands from them.

**Usage:**
- The server is automatically started on local ip and port 3000.
- Use ctrl+c to stop the server.
### Server functionality
- Parsing a message from a client and returning a message containing a table with a number of different letters

  Message|Hello
  ---|---
  H|1
  e|1
  l|2
  o|1
- Return the number of current connections at the client's request
- Ability for clients to communicate through the server

**Usage**
- Pass as command line arguments the IP (0) and port (3000) of the server, to connect.
- You can then send messages by typing text and pressing enter.
- To parse messages use the command "/parse <message>".
- To find out the number of connections to the server use the command "/connections".
- Type "q" to quit the client.

## Installation instructions
Use this commands in project directory to install programms:
```
mkdir build
cd build
cmake ../src
make
```
- To run **server** use: ```./server/server```
- To run **client** use: ```./client/client <ip> <port>```
