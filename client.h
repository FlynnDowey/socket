#ifndef CLIENT_H_
#define CLIENT_H_

void Client_init(int portNumber, char* ipAddr);
void Client_connectToServer(void);
void Client_disconnectFromServer(void);
void Client_sendMessage(char* messageTx, int size);
char* Client_getMessage(void);

#endif
