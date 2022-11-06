#ifndef SERVER_H_
#define SERVER_H_

void Server_init(int portNumber, char* ipAddr);
void Server_cleanup(void);
char* Server_getMessage(void);
void Server_sendMessage(char* messageTx, int size);
void Server_listen(void);

#endif
