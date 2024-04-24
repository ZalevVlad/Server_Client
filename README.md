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
