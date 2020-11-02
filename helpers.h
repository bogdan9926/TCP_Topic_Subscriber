#ifndef _HELPERS_H
#define _HELPERS_H 1
using namespace std;
#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <cstdint>
#include <cstring>
#include <string>
#include <netinet/in.h>
#include <cmath>
#include <zconf.h>
#include <string.h>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <list>
#include <algorithm>
#include <iostream>
#include <bits/stdc++.h>

#define MAXBUFF 1552
#define BUFLEN (sizeof(msg_recv_TCP) + 1)

#define DIE(assertion, call_description)	\
	do {									\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",	\
					__FILE__, __LINE__);	\
			perror(call_description);		\
			exit(EXIT_FAILURE);				\
		}									\
	} while(0)

//structura de tip topic in care retin clientii fiecarui topic
struct topic {
    char name[50];
    vector<int>clients;
    int nr_clients;
    topic():clients(1000) { }
};

//mesaj tcp->server
struct msg_send_TCP {
    int type;
    char topic[50];
    int sf;
};

//mesaj server->tcp
struct msg_recv_TCP {

    char topic[51];
    char type[11];
    char data[1501];

    char ip[16];
    uint16_t udp_port;
    
};

//mesaj udp->server
struct  msg_recv_UDP {

    char topic[50];
    uint8_t type;
    char data[1501];
};

bool check_message(msg_recv_UDP* receive_msg, msg_recv_TCP* send_msg);
void close_all_fd(fd_set *fds, int max_fd);
void subscribe(int fd, char* topic_name, vector<topic>& topics);
void unsubscribe(int fd, char* topic_name, vector<topic>& topics);
string convertToString(char* a);

#endif
