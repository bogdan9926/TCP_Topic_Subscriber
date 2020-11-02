#include "helpers.h"
using namespace std;

//verifica daca mesajul primit este unul bun de trimis
bool check_message(msg_recv_UDP* receive_msg, msg_recv_TCP* send_msg) {

    int nr_int;
    double nr_double;

    strcpy(send_msg->topic, receive_msg->topic);

    if(strlen(send_msg->topic)>50) {
        perror ("topic name too big ");
        return false;
    }

    send_msg->topic[strlen(send_msg->topic)]=0;

    if(receive_msg->type==0) {
        //int

        nr_int = ntohl(*(uint32_t*)(receive_msg->data + 1));

         if (receive_msg->data[0]) { 
            nr_int *= -1;
        }
        sprintf(send_msg->data, "%d", nr_int);
        strcpy(send_msg->type, "INT");

    } else if(receive_msg->type==1) {
        //short_real

        nr_double = ntohs(*(uint16_t*)(receive_msg->data));
        nr_double /= 100;
        sprintf(send_msg->data, "%.2f", nr_double);
        strcpy(send_msg->type, "SHORT_REAL");

    } else if(receive_msg->type==2) {
        //float

        nr_double = ntohl(*(uint32_t*)(receive_msg->data + 1));
        nr_double /= pow(10, receive_msg->data[5]);

        if (receive_msg->data[0]) { 
            nr_double *= -1;
        }

        sprintf(send_msg->data, "%f", nr_double);
        strcpy(send_msg->type, "FLOAT");

    } else if(receive_msg->type==3) {
        strcpy(send_msg->type, "STRING");
        strcpy(send_msg->data, receive_msg->data);
    } //else return false;
    return true;

}

void close_all_fd(fd_set *fds, int fdmax) {
    for (int i = 2; i <= fdmax; i++) {
        if (FD_ISSET(i, fds)) {
            close(i);
        }
    }
}

//se creeaza topic daca acesta nu exista, sau se adauga clientul in topic
void subscribe(int fd, char* topic_name, std::vector<topic>& topics) {
    bool present = false;
     for(int i = 0 ; i<topics.size();i++) {
		 if(strcmp (topics[i].name, topic_name ) == 0) {
            present = true;
            topics[i].nr_clients++;
            topics[i].clients.at(fd) = 1;
        }
    }
    if(present == false) {
        topic new_topic;
        strcpy(new_topic.name , topic_name);
        new_topic.clients[fd]=1;
        new_topic.nr_clients = 1;
        topics.push_back(new_topic);
    }
}
//sau se scoate clientul din topic
void unsubscribe(int fd, char* topic_name, std::vector<topic>& topics) {
    
    for(int i = 0 ; i<topics.size();i++) {
	    if(strcmp (topics[i].name, topic_name ) == 0) {
             topics[i].nr_clients--;
           topics[i].clients[fd] = 0;
          }
     }
}


string convertToString(char* a) 
{ 
    int i; 
    string s = a;
    return s; 
} 