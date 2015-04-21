#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "TCPServerUtility.h"
#include "UDPServerUtility.h"

/*
 * Modified heavily from UDPEchoServer.c and TCPEchoServer.c found in:
 * Donahoo, Michael J., and Kenneth L. Calvert. TCP/IP Sockets in C Practical Guide for Programmers. Amsterdam: Morgan Kaufmann, 2009. Print.
 *
 */

#define MAXSTRINGLENGTH 100

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		puts("Usage: server <UDP Port Number> <TCP Port Number>");
		exit(1);
	}

	char *service = argv[1];
	char *tcpService = argv[2];

	int sock = SetupUDPServerSocket(service);
	if (sock < 0)
	{
		puts("SetupUDPServerSocket() failed");
		exit(1);
	}

	for (;;)
	{
		struct sockaddr_storage clntAddr;
		socklen_t clntAddrLen = sizeof(clntAddr);

		char buffer[MAXSTRINGLENGTH] = {};
		char reply[MAXSTRINGLENGTH] = {};

		ssize_t numBytesRcvd = recvfrom(
								   sock,
								   buffer,
								   MAXSTRINGLENGTH,
								   0,
								   (struct sockaddr *) &clntAddr,
								   &clntAddrLen
							   );

		if (numBytesRcvd < 0)
		{
			puts("recvfrom() failed");
			exit(1);
		}

		buffer[strcspn(buffer, "\n")] = '\0';

		if (strncmp(buffer, "HELLO", 5) == 0)
		{
			strncpy(reply, "OK\n", MAXSTRINGLENGTH);
			ssize_t numBytesSent = sendto(sock, reply, sizeof(reply), 0, (struct sockaddr *) &clntAddr, sizeof(clntAddr));

			if (numBytesSent < 0)
			{
				puts("sendto() failed");
				exit(1);
			}

			memset(buffer, 0, sizeof(buffer));

			numBytesRcvd = recvfrom(
							   sock,
							   buffer,
							   MAXSTRINGLENGTH,
							   0,
							   (struct sockaddr *) &clntAddr,
							   &clntAddrLen
						   );

			if (numBytesRcvd < 0)
			{
				puts("recvfrom() failed");
				exit(1);
			}

			buffer[strcspn(buffer, "\n")] = '\0';

			if (strncmp(buffer, "UPLOAD", 6) == 0)
			{
				strncpy(reply, "FileName\n", MAXSTRINGLENGTH);
				numBytesSent = sendto(sock, reply, sizeof(reply), 0, (struct sockaddr *) &clntAddr, sizeof(clntAddr));

				if (numBytesSent < 0)
				{
					puts("sendto() failed");
					exit(1);
				}

				memset(buffer, 0, sizeof(buffer));

				numBytesRcvd = recvfrom(
								   sock,
								   buffer,
								   MAXSTRINGLENGTH,
								   0,
								   (struct sockaddr *) &clntAddr,
								   &clntAddrLen
							   );

				if (numBytesRcvd < 0)
				{
					puts("recvfrom() failed");
					exit(1);
				}

				buffer[strcspn(buffer, "\n")] = '\0';

				FILE * fp;
				fp = fopen(buffer, "w+");

				// create tcp socket
				int servSock = SetupTCPServerSocket(tcpService);
				if (servSock < 0)
				{
					puts("SetupTCPServerSocket() failed");
					exit(1);
				}

				strncpy(reply, "READY\n", MAXSTRINGLENGTH);
				numBytesSent = sendto(sock, reply, sizeof(reply), 0, (struct sockaddr *) &clntAddr, sizeof(clntAddr));

				if (numBytesSent < 0)
				{
					puts("sendto() failed");
					exit(1);
				}

				int clntSock = AcceptTCPConnection(servSock);
				if (clntSock < 0)
				{
					puts("AcceptTCPConnection() failed");
					exit(1);
				}

				// Handle file upload
				HandleTCPClient(clntSock, fp);

				break;
			}
		}
	}
	exit(0);
}
