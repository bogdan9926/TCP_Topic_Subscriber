#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstring>
#include "helpers.h"
using namespace std;



void usage()
{
	fprintf(stderr, "Usage: ./subscriber <ID> <IP_SERVER> <PORT_SERVER>.\n");
	exit(0);
}
//creearea mesajului citit de la stdin
int create_message (msg_send_TCP* message, char* buffer ) {

	char *p;
	p = strtok(buffer, " \n");
	if (p == NULL) return 0;

	if (strncmp(p, "subscribe",9) == 0) {
		message->type = 1;

	} else if(strncmp(p, "unsubscribe",11) == 0) 
		message->type = 0;

	else return 0;

	p = strtok(NULL, " \n");

	if (p == NULL) return 0;

	strcpy(message->topic, p);

	if (message->type == 1) {
		p = strtok(NULL, " \n");

		if (p == NULL) return 0;
		message->sf = p[0] - '0';
	}
	p = strtok(NULL, " \n");
	if (message->type == 0 && p!=NULL)
		return 0;
	buffer[strlen(buffer) - 1] = 0;
	return 1;
}

int main(int argc, char *argv[])
{
	int n, ret;
	struct sockaddr_in serv_addr;
	char buffer[BUFLEN];

	msg_send_TCP send_msg;
	msg_recv_TCP* recv_msg;

	if (argc < 3) {
		usage();
	}
	int sockfd;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	DIE(sockfd < 0, "socket");

	
	fd_set read_fds;
	fd_set tmp_fds;
	FD_ZERO(&read_fds);
	FD_ZERO(&tmp_fds);
	FD_SET(0, &read_fds);

	int flag = 1;

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[3]));
	ret = inet_aton(argv[2], &serv_addr.sin_addr);
	DIE(ret == 0, "inet_aton");

	ret = connect(sockfd, (sockaddr*) &serv_addr, sizeof(serv_addr));
	DIE(ret < 0, "connect");

	FD_SET(sockfd, &read_fds);

	ret = send(sockfd, argv[1], strlen(argv[1]) + 1, 0);
	setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));
	
	while (1) {
		tmp_fds = read_fds;
		ret = select(sockfd + 1, &tmp_fds, NULL, NULL, NULL);

		if (FD_ISSET(0, &tmp_fds)) {
			
			// se citeste de la tastatura
			memset(buffer, 0, BUFLEN);
			fgets(buffer, MAXBUFF - 1, stdin);

			if (strncmp(buffer, "exit",4) == 0) {
				break;

			} else if (create_message(&send_msg, buffer)) {
				n = send(sockfd, (char*) &send_msg, sizeof(send_msg),0);
				DIE(n < 0, "send");
			
				if (send_msg.type == 1)
					printf("subscribed %s\n", send_msg.topic);
				else
					printf("unsubscribed %s\n", send_msg.topic);

			} else printf("Commands are `subscribe <topic> <SF>`, `unsubscribe <topic>`, or `exit`.\n");
		}

		if (FD_ISSET(sockfd, &tmp_fds))
		{
			memset(buffer, 0, BUFLEN);
			n = recv(sockfd, buffer, sizeof(msg_recv_TCP), 0);


			if(n==0)
				break;

			recv_msg = (msg_recv_TCP*) buffer;

			printf("%s:%hu - %s - %s - %s\n", recv_msg->ip, recv_msg->udp_port,
                   recv_msg->topic, recv_msg->type, recv_msg->data);
		}
	}

	close(sockfd);

	return 0;
}
