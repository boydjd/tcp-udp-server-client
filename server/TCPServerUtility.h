#ifndef TCPSERVERUTILITY_H_INCLUDED
#define TCPSERVERUTILITY_H_INCLUDED

#define RCVBUFSIZE 10000 
#define MAXSTRINGLENGTH 100

int SetupTCPServerSocket(const char *service);
int AcceptTCPConnection(int servSock); 
void HandleTCPClient(int clntSocket, FILE * fp);

#endif
