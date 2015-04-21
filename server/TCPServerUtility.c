#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "TCPServerUtility.h"

/*
 * Modified heavily from TCPServerUtility.c found in:
 * Donahoo, Michael J., and Kenneth L. Calvert. TCP/IP Sockets in C Practical Guide for Programmers. Amsterdam: Morgan Kaufmann, 2009. Print.
 *
 */

int SetupTCPServerSocket(const char *service)
{
	struct addrinfo addrCriteria;
	memset(&addrCriteria, 0, sizeof(addrCriteria));
	addrCriteria.ai_family = AF_UNSPEC;
	addrCriteria.ai_flags = AI_PASSIVE;
	addrCriteria.ai_socktype = SOCK_STREAM;
	addrCriteria.ai_protocol = IPPROTO_TCP;

	struct addrinfo *servAddr;
	int rtnVal = getaddrinfo(NULL, service, &addrCriteria, &servAddr);
	if (rtnVal != 0)
	{
		puts("getaddrinfo() failed");
		exit(1);
	}

	int servSock = -1;
	struct addrinfo *addr;
	for (addr = servAddr; addr != NULL; addr = addr->ai_next)
	{
		servSock = socket(servAddr->ai_family, servAddr->ai_socktype, servAddr->ai_protocol);
		if (servSock < 0)
			continue;

		if ((bind(servSock, servAddr->ai_addr, servAddr->ai_addrlen) == 0) && (listen(servSock, 5) == 0))
		{
			struct sockaddr_storage localAddr;
			socklen_t addrSize = sizeof(localAddr);
			if (getsockname(servSock, (struct sockaddr *) &localAddr, &addrSize) < 0)
			{
				puts("getsockname() failed");
				exit(1);
			}
			break;
		}

		close(servSock);
		servSock = -1;
	}

	freeaddrinfo(servAddr);

	return servSock;
}

int AcceptTCPConnection(int servSock)
{
	struct sockaddr_storage clntAddr;
	socklen_t clntAddrLen = sizeof(clntAddr);

	int clntSock = accept(servSock, (struct sockaddr *) &clntAddr, &clntAddrLen);
	if (clntSock < 0)
	{
		puts("accept() failed");
		exit(1);
	}

	return clntSock;
}

void HandleTCPClient(int clntSocket, FILE * fp)
{
	char buffer[RCVBUFSIZE];
	char reply[MAXSTRINGLENGTH] = {};
	int numBytesSent;
	FILE * tfp;
	char line[128];

	tfp = tmpfile();

	memset(buffer, 0, sizeof(buffer));
	recv(clntSocket, buffer, RCVBUFSIZE, 0);

	fwrite(buffer, 1, sizeof(buffer), tfp);
	rewind(tfp);

	while (fgets(line, sizeof line, tfp))
	{
		if (strstr(line, "END OF FILE") == NULL)
			fputs(line, fp);
	}

	fclose(tfp);

	strncpy(reply, "GOT IT\n", MAXSTRINGLENGTH);
	numBytesSent = send(clntSocket, reply, sizeof(reply), 0);

	fclose(fp);

	if (numBytesSent < 0)
	{
		puts("sendto() failed");
		exit(1);
	}
}
