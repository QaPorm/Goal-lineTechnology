#include"TCPclient.h"

using namespace std;

TCPclient::TCPclient(string ip,int servPort)
{
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(sockfd<0)
	{
		perror("error-opening socket");
		exit(-1);
	}
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=inet_addr(ip.c_str());
	servaddr.sin_port=htons(servPort);
	if(connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr))<0) 
	{
		perror("error-connecting");
		exit(-1);
	}
}
TCPclient::~TCPclient()
{
	close(sockfd);
}
int TCPclient::sendData(void *data,int size)
{
	int bytes;
	bytes=send(sockfd,data,size,0);
	return bytes;
}
int TCPclient::recvData(void *data,int size)
{
	int bytes;
	bytes=recv(sockfd,data,size,0);
	return bytes;
}