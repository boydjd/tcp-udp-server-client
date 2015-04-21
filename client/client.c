#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

/* 
 * Modified heavily from http://www.cs.ucsb.edu/~almeroth/classes/W01.176B/hw2/examples/udp-client.c and UDPEchoClient.c
 * found in:
 * Donahoo, Michael J., and Kenneth L. Calvert. TCP/IP Sockets in C Practical Guide for Programmers. Amsterdam: Morgan Kaufmann, 2009. Print.
 *
 */

int main(int argc, char**argv)
{
	int sockfd, n;
	struct sockaddr_in servaddr;
	struct sockaddr_in tcpservaddr;
	char sendline[1000];
	char recvline[1000];
	FILE * fp;
	char line[128];

	if (argc != 4)
	{
		printf("usage:  client <IP address> <UDP port> <TCP port>\n");
		exit(1);
	}

    char *udp = argv[2];
    char *tcp = argv[3]; 

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_port = htons(atoi(udp));

	bzero(&tcpservaddr, sizeof(tcpservaddr));
	tcpservaddr.sin_family = AF_INET;
	tcpservaddr.sin_addr.s_addr = inet_addr(argv[1]);
	tcpservaddr.sin_port = htons(atoi(tcp));

	while (fgets(sendline, 10000, stdin) != NULL)
	{
		if (strncmp(recvline, "FileName", 8) == 0)
		{
			sendline[strcspn(sendline, "\n")] = '\0';
			fp = fopen(sendline, "r");
			if (fp == NULL)
			{
				puts("fopen() failed");
				exit(1);
			}
		}
		sendto(sockfd, sendline, strlen(sendline), 0,
			   (struct sockaddr *)&servaddr, sizeof(servaddr));
		n = recvfrom(sockfd, recvline, 10000, 0, NULL, NULL);
		recvline[n] = 0;
		fputs(recvline, stdout);


		if (strncmp(recvline, "READY", 5) == 0)
			break;
	}

	// Open TCP connection

	int tcpSockFd;

	tcpSockFd = socket(AF_INET, SOCK_STREAM, 0);
	int connection = connect(tcpSockFd, (struct sockaddr *) &tcpservaddr, sizeof(tcpservaddr));

	// Send file
	while(fgets(line, sizeof line, fp) != NULL)
	{
		send(tcpSockFd, line, strlen(line), 0);
	}

	fclose(fp);

	send(tcpSockFd, "END OF FILE", 11, 0);

	if ((n = recv(tcpSockFd, recvline, 1000, 0)) < 0)
	{
		puts("recv() failed");
		exit(1);
	}

	if (strncmp(recvline, "GOT IT", 6) == 0)
	{
		puts("Transfer successfully");
	}
	else
	{
		puts("Transfer failed");
	}

	close(connection);

	return 0;
}
