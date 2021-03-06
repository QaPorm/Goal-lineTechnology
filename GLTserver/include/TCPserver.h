//////////////////////////////////////////////////////////////////////////////
//	GOAL-LINE TECHNOLOGY													//
//	Senior Project 2014 (Computer Engineering)								//
//	Faculty of Engineering, Mahidol University								//
//	Nattaya Sriphop															//
//	Patchara Chanprakhon													//
//	Rungroj Somwong															//
//////////////////////////////////////////////////////////////////////////////
//	TCPserver.h - TCP protocol of server side. (header)						//
//////////////////////////////////////////////////////////////////////////////

#ifndef __TCPSERVER_H
#define __TCPSERVER_H
#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<cstring>

using namespace std;

class TCPserver
{
private:
	int sockfd,newsockfd;
	socklen_t clilen;
	struct sockaddr_in servaddr,cliaddr;
public:
	TCPserver(int servPort);
	~TCPserver();
	int sendData(void *data,int size);
	int recvData(void *data,int size);
};

#endif