#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include "udp_server.h"

void* collect_data(void* arg)
{
	udp_server *s = (udp_server*)arg;
	while(1){
		s->reliable_recv_msg();
	}
}

void *send_data(void *arg)
{
	udp_server *s = (udp_server*)arg;
	while(1){
		s->broadcast_msg();
	}
}

static void usage(char* proc)
{
	std::cout<<"Please Enter:# "<<proc<<" [PORT]"<<std::endl;
}

int main(int argc ,char *argv[])
{
	if(argc != 2){
		usage(argv[0]);
		exit(1);
	}

	unsigned short port = atoi(argv[1]);
	udp_server server(port);
	server.init_server();

	pthread_t id1, id2;
	pthread_create(&id1, NULL, collect_data, (void*)&server);
	pthread_create(&id2, NULL, send_data, (void*)&server);

	pthread_join(id1, NULL);
	pthread_join(id2, NULL);

	return 0;
}







