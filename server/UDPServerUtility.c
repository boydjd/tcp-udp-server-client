#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "UDPServerUtility.h"

int SetupUDPServerSocket(const char *service)
{
	struct addrinfo addrCriteria;
	memset(&addrCriteria, 0, sizeof(addrCriteria));
	addrCriteria.ai_family = AF_UNSPEC;
	addrCriteria.ai_flags = AI_PASSIVE;
	addrCriteria.ai_socktype = SOCK_DGRAM;
	addrCriteria.ai_protocol = IPPROTO_UDP;

	struct addrinfo *servAddr;
	int rtnVal = getaddrinfo(NULL, service, &addrCriteria, &servAddr);
	if (rtnVal != 0)
	{
		puts("getaddrinfo() failed");
		exit(1);
	}

	int sock = socket(servAddr->ai_family, servAddr->ai_socktype, servAddr->ai_protocol);
	if (sock < 0)
	{
		puts("socket() failed");
		exit(1);
	}

	if (bind(sock, servAddr->ai_addr, servAddr->ai_addrlen) < 0)
	{
		puts("bind() failed");
		exit(1);
	}

	freeaddrinfo(servAddr);

	return sock;
}
