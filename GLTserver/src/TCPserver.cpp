//////////////////////////////////////////////////////////////////////////////
//	GOAL-LINE TECHNOLOGY													//
//	Senior Project 2014 (Computer Engineering)								//
//	Faculty of Engineering, Mahidol University								//
//	Nattaya Sriphop															//
//	Patchara Chanprakhon													//
//	Rungroj Somwong															//
//////////////////////////////////////////////////////////////////////////////
//	TCPserver.cpp - TCP protocol of server side.							//
//////////////////////////////////////////////////////////////////////////////

#include"TCPserver.h"

using namespace std;

TCPserver::TCPserver(int servPort)
{
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(sockfd<0)
	{
		perror("error-opening socket");
		exit(-1);
	}
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=INADDR_ANY;
	servaddr.sin_port=htons(servPort);
	if(bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr))<0)
	{
		perror("error-bind");
		exit(-1);
	}
	listen(sockfd,5);
	clilen=sizeof(cliaddr);
	newsockfd=accept(sockfd,(struct sockaddr *)&cliaddr,&clilen);
	if(newsockfd<0)
	{
		perror("error-on accept");
		exit(-1);
	}
}
TCPserver::~TCPserver()
{
	close(sockfd);
	close(newsockfd);
}
int TCPserver::sendData(void *data,int size)
{
	int bytes;
	bytes=send(newsockfd,data,size,0);
	return bytes;
}
int TCPserver::recvData(void *data,int size)
{
	int bytes;
	bytes=recv(newsockfd,data,size,0);
	return bytes;
}