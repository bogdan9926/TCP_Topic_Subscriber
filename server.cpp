#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include "helpers.h"

using namespace std;
void usage()
{
	fprintf(stderr, "Usage: ./server <PORT>\n");
	exit(0);	
}
int main(int argc, char **argv) {

	if (argc < 2) {
		usage();
	}

	//vector de structuri de tip topic
	vector<topic> topics;

	//vector de nume de clienti, folosit doar la afisare
	vector<string> clients(10000);


	char buffer[MAXBUFF];
    int fdmax, sockfd, flag = 1, ret;

    socklen_t udp_socklen = sizeof(sockaddr);
	socklen_t tcp_socklen = sizeof(sockaddr);
    sockaddr_in udp_addr, tcp_addr, new_tcp_addr;

	msg_send_TCP* serv_msg;
	msg_recv_TCP tcp_msg;
	msg_recv_UDP* udp_msg;

    fd_set read_fds;
	fd_set tmp_fds;
    

	int udp_sock = socket(PF_INET, SOCK_DGRAM, 0);
	int tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
	int port_num = atoi(argv[1]);

	udp_addr.sin_family = AF_INET;
	tcp_addr.sin_family = AF_INET;
    udp_addr.sin_port = htons(port_num);
	tcp_addr.sin_port = htons(port_num);
    udp_addr.sin_addr.s_addr = INADDR_ANY;
	tcp_addr.sin_addr.s_addr = INADDR_ANY;

	ret = bind(udp_sock, (sockaddr*) &udp_addr, sizeof(sockaddr_in));
	DIE(ret < 0, "bind_udp");

	ret = bind(tcp_sock, (sockaddr*) &tcp_addr, sizeof(sockaddr_in));
	DIE(ret < 0, "bind_tcp");

	ret = listen(tcp_sock, __INT_MAX__);
	DIE(ret < 0, "listen_tcp");


	FD_ZERO(&read_fds);
    FD_SET(tcp_sock, &read_fds);
    FD_SET(udp_sock, &read_fds);
	FD_SET(0, &read_fds);

    fdmax = tcp_sock;

	while (1) {

		tmp_fds = read_fds;
		memset(buffer, 0, MAXBUFF);
		ret = select(fdmax + 1, &tmp_fds, NULL, NULL, NULL);
		DIE(ret < 0, "select");

		for (int i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &tmp_fds)) {
				
				if(i==0) {
					//stdin
					fgets(buffer, MAXBUFF - 1, stdin);
					DIE(!strcmp(buffer, "exit\n"),"out");
                    printf("Illegal command. Only `exit` allowed.\n");

				} else if (i==udp_sock) {
					//mesaj de la udp
					ret = recvfrom(udp_sock, buffer, BUFLEN, 0, 
					(sockaddr*) &udp_addr, &udp_socklen);
					tcp_msg.udp_port = ntohs(udp_addr.sin_port);
					strcpy(tcp_msg.ip, inet_ntoa(udp_addr.sin_addr));
                    udp_msg = (msg_recv_UDP*)buffer;

					 if (check_message(udp_msg, &tcp_msg)) {

						for(int i = 0 ; i<topics.size();i++) {
							if(!strcmp (topics[i].name, tcp_msg.topic)) {
								for(int j=0; j< topics[i].clients.size();j++) {
									if(topics[i].clients[j]) {
										ret = send(j, (char*) &tcp_msg, 
										sizeof(msg_recv_TCP), 0);
                                		DIE(ret < 0, "Cannot send message to TCP");
									}
										
								}
										
							}
						}
					}
						
					

				} else if (i == tcp_sock) {	
					//un nou client tcp
					sockfd = accept(i, (sockaddr*) &new_tcp_addr, &tcp_socklen);

					setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, 
					sizeof(int));

					FD_SET(sockfd, &read_fds);
					//updatam fdmax
					if(sockfd > fdmax) 
                        fdmax = sockfd;

					ret = recv(sockfd, buffer, MAXBUFF - 1, 0);

					for(int i = 0 ; i<10000;i++) {
						if(clients[i] == convertToString(buffer)) {
							DIE(1<2,"Cannot have 2 clients with the same ID!\n" );
						}
					}
					clients[sockfd] = convertToString(buffer);

					 printf("New client %s connected from %s:%hu.\n", buffer,
                           inet_ntoa(new_tcp_addr.sin_addr), 
						   ntohs(new_tcp_addr.sin_port));
				} else {
					//mesaj de la tcp
					ret = recv(i, buffer, MAXBUFF - 1, 0);	
					 if (!ret) {
						//ctrl+c
                        printf("Client %s disconnected.\n", clients[i].c_str());
						FD_CLR(i, &read_fds);
						clients.erase(clients.begin() + i);

						//updatam fdmax
						for (int j = fdmax; j >= 1; j--) {
        					if(FD_ISSET(j, &read_fds)) {
           						fdmax = j;
            					break;
       						 }
    					}

						//stergem clientul din toate topicurile
						//daca topicul are doar 1 client, stergem intreg topicul 
						for(int j = 0 ; j < topics.size();j++) {
							if(topics[j].clients.at(i)) {

								if(topics[j].nr_clients==1) {
									topics.erase(topics.begin()+j);

								} else {
									topics[j].clients[i] = 0;
									topics[j].nr_clients--;
								}

							}
								
						}	
						close(i);

					 } else {
						//avem comanda de subscribe/unsubscribe
						serv_msg = (msg_send_TCP*)buffer;

						if (serv_msg->type == 1) {

							subscribe(i, serv_msg->topic, topics);

						} else 
                        	unsubscribe(i, serv_msg->topic, topics);
					}

				}			
				 
			}
		}
	}
	

	 close_all_fd(&read_fds, fdmax);

	return 0;
}
						