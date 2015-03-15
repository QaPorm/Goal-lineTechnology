#ifndef __TCPCLIENT_H
#define __TCPCLIENT_H
#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<cstring>

using namespace std;

class TCPclient
{
private:
	int sockfd;
	struct sockaddr_in servaddr;
public:
	TCPclient(string ip,int servPort);
	~TCPclient();
	int sendData(void *data,int size);
	int recvData(void *data,int size);
};

#endif