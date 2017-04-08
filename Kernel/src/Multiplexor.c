#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdbool.h>
#include "Results.h"
#include <commons/collections/list.h>
#include "CustomCommons.h"

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

t_list* listeners;
t_list* clients;
int fdmax;
fd_set rfds;
fd_set rfdsTemp;

void AddFdToMaster(int fd){
	FD_SET(fd, &rfds);

	if (fd > fdmax) {    // keep track of the max
		fdmax = fd;
	}
}

void AddListenerToMaster(int fd){
	AddFdToMaster(fd);

	list_add(listeners, fd);
}

void AddClientToMaster(int fd){
	AddFdToMaster(fd);

	list_add(clients, fd);
}

void ResetSet(int listener){
	FD_ZERO(&rfds);
	AddListenerToMaster(listener);
}

ResultWithValue SelectReaders(){
	int retval = select(fdmax+1, &rfdsTemp, NULL, NULL, NULL);

	if(retval == -1)
		return ErrorWithValue(strerror("select()"),NULL);
	else
		return OkWithValue(retval);
}

bool contains(t_list* list, void* value){

	bool equals(void* item) {
		return item == value;
	}

	return list_any_satisfy(list, equals);
}

bool isListener(int fd){
	return contains(listeners, fd);
}

bool isClient(int fd){
	return contains(clients, fd);
}

void PrintClientData(struct sockaddr_storage remoteaddr,int newfd,char remoteIP[INET6_ADDRSTRLEN]){
	printf("selectserver: new connection from %s on "
			"socket %d\n",
			inet_ntop(remoteaddr.ss_family,
				get_in_addr((struct sockaddr*)&remoteaddr),
				remoteIP, INET6_ADDRSTRLEN),
			newfd);
}

ResultWithValue GetNewConnection(int listener){
	int newfd;        // newly accept()ed socket descriptor
	struct sockaddr_storage remoteaddr; // client address

	char remoteIP[INET6_ADDRSTRLEN];

    // handle new connections
	socklen_t addrlen;

	addrlen = sizeof remoteaddr;

	newfd = accept(listener,
			(struct sockaddr *)&remoteaddr,
			&addrlen);

    if (newfd == -1)
        return ErrorWithValue(strerror("accept"),NULL);


    AddClientToMaster(newfd);
    PrintClientData(remoteaddr,newfd,remoteIP);

    return OkWithValue(NULL);
}


void AlRecibirMensaje(char* buffer, int bytesRecibidos){
	buffer[bytesRecibidos] = '\0';
	printf("Me llegaron %d bytes con %s", bytesRecibidos, buffer);
}


ResultWithValue CheckForIncomingData(){
	int i;
	ResultWithValue r;

	for(i = 0; i <= fdmax; i++)
	{
		if (FD_ISSET(i, &rfdsTemp))
		{
			if(isListener(i))
			{
				r = GetNewConnection(i);

				if(r.result.noError != true)
					return r;
			}
			else if(isClient(i)){
				RecibirMensaje(i, AlRecibirMensaje);
			}
		}
	}

	return OkWithValue(NULL);
}


Result Multiplexar(int listener){
	listeners 	= list_create();
	clients 	= list_create();

	ResultWithValue r;

	/* Watch stdin (fd 0) to see when it has input. */
	ResetSet(listener);

	fdmax = listener;

	for(;;) {
		rfdsTemp = rfds;


		r = SelectReaders();

		if (r.result.noError != true)
	    	return r.result;


		r = CheckForIncomingData();

		if (r.result.noError != true)
			return r.result;
	}

	return r.result;
}
