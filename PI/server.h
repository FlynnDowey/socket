#ifndef SERVER_H_
#define SERVER_H_

/// @brief Initializes socket for connection at portNumber and ipAddr
/// @param portNumber 
/// @param ipAddr 
void Server_init(int portNumber, char* ipAddr);

/// @brief closes the socket
void Server_cleanup(void);


/// @brief recieves a message from the socket; user must free the message afterwards
/// @return dynamically allocated array
char* Server_getMessage(void);

/// @brief Sends a message to the socket; must specify the size.
/// @param messageTx 
/// @param size 
void Server_sendMessage(char* messageTx, int size);

/// @brief Binds to socket, accepts and waits for signal from client.
void Server_listen(void);

/// @brief Sends an integer across the socket. Used for image size.
/// @param size 
void Server_sendMessageSize(long size);

#endif
