//////////////////////////////////////////////////////////////////////////////
//	GOAL-LINE TECHNOLOGY													//
//	Senior Project 2014 (Computer Engineering)								//
//	Faculty of Engineering, Mahidol University								//
//	Nattaya Sriphop															//
//	Patchara Chanprakhon													//
//	Rungroj Somwong															//
//////////////////////////////////////////////////////////////////////////////
//	glt_client.cpp - connect with server and send goal frame position,		//
//					 then loop detect ball and send position.				//
//////////////////////////////////////////////////////////////////////////////

#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<sys/time.h>
#include<string>
#include<thread>
#include<opencv2/opencv.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/highgui/highgui_c.h>
#include<opencv2/imgproc/imgproc.hpp>
#include<raspicam/raspicam_cv.h>
#include"TCPclient.h"
#include"GLTframeDetect.h"
#include"GLTballDetect.h"

using namespace std;
using namespace cv;

TCPclient *client;
const string SERVER_IP_ADDR="192.168.1.1";
int initFlag=0,readyFlag=0,exitFlag=0;
int side,pixPos,pixPosBack[3];
Mat frame,frameGray;
Point raw[6],avr[6];

void preview_and_Send()
{
	int32_t buf;
	int bytes,imgsize;
	Mat fCapture,showImg,res;
	
	raspicam::RaspiCam_Cv fCam;
	fCam.set(CV_CAP_PROP_FORMAT,CV_8UC3);
	fCam.set(CV_CAP_PROP_FRAME_WIDTH,640);
	fCam.set(CV_CAP_PROP_FRAME_HEIGHT,360);
	if(!fCam.open())
	{
		cout<<"error-cannot open camera"<<endl;
		exit(-1);
	}
	
	while(initFlag!=1)
	{
		fCam.grab();
		fCam.retrieve(fCapture);
		
		frame=fCapture.clone();
		showImg=fCapture.clone();
		if(side==1||side==2)
		{
			line(showImg,raw[0],raw[1],Scalar(0,0,255),1,CV_AA);
			line(showImg,avr[0],avr[1],Scalar(0,255,0),1,CV_AA);
		}
		else if(side==3)
		{
			line(showImg,raw[0],raw[1],Scalar(0,0,255),1,CV_AA);
			line(showImg,raw[2],raw[3],Scalar(0,0,255),1,CV_AA);
			line(showImg,raw[4],raw[5],Scalar(0,0,255),1,CV_AA);
			line(showImg,avr[0],avr[1],Scalar(0,255,0),1,CV_AA);
			line(showImg,avr[2],avr[3],Scalar(0,255,0),1,CV_AA);
			line(showImg,avr[4],avr[5],Scalar(0,255,0),1,CV_AA);
		}
		res=showImg.reshape((0,1));
		imgsize=res.total()*res.elemSize();
		for(int i=0;i<imgsize&&initFlag!=1;i+=bytes)
			bytes=client->sendData(res.data+i,imgsize-i);
		if(side==1||side==2)
		{
			buf=htonl(pixPos);
			client->sendData(&buf,sizeof(buf));	//send position
		}
		else if(side==3)
			client->sendData(&pixPosBack,3*sizeof(int));
	}
	fCam.release();
}

void recvHandler()
{
	int32_t buffer;
	while(readyFlag!=1)
	{
		client->recvData(&buffer,sizeof(buffer));
		cout<<htonl(buffer)<<" "<<initFlag<<endl;
		cout.flush();
		if(htonl(buffer)==131)	//receive ack
			initFlag=1;
		else if(htonl(buffer)==845)
			readyFlag=1;
	}
}

int main(int argc,char *argv[])
{	
	int port;
	int *bufPos;
	timeval now;
	
	if(argc==2)
	{
		side=atoi(argv[1]);
		if(side>=1&&side<=3)
			port=side*1111;
		else
		{
			cout<<"error-main parameter"<<endl
				<<"   ./mainClient [module no.]"<<endl
				<<"   Left module  -> 1"<<endl
				<<"   Right module -> 2"<<endl
				<<"   Back module  -> 3"<<endl;
			exit(-1);
		}
	}
	else
	{
		cout<<"error-main parameter"<<endl
			<<"   ./mainClient [module no.]"<<endl
			<<"   Left module  -> 1"<<endl
			<<"   Right module -> 2"<<endl
			<<"   Back module  -> 3"<<endl;
		exit(-1);
	}
	client=new TCPclient(SERVER_IP_ADDR,port);
	thread preview(preview_and_Send);
	thread recv(recvHandler);
	
	//GLT FRAME DETECTION////////////////////////////////////////////////////////
	GLTframeDetect *fDetect=new GLTframeDetect(side);
	while(initFlag!=1)
	{	
		if(frame.empty()!=1)
		{
			bufPos=fDetect->framePosition(raw,avr,frame);
			if(side==1||side==2)
				pixPos=bufPos[0];
			else if(side==3)
			{
				pixPosBack[0]=bufPos[0];
				pixPosBack[1]=bufPos[1];
				pixPosBack[2]=bufPos[2];
			}
		}
	}
	preview.join();
	delete fDetect;
	/////////////////////////////////////////////////////////////////////////////

	while(readyFlag!=1)
		sleep(1);
	recv.join();
	delete client;
	sleep(10);
	port=(side*1111)+50;	//port 1161, 2272, 3383
	client=new TCPclient(SERVER_IP_ADDR,port);
	
	//GLT BALL DETECTION/////////////////////////////////////////////////////////
	Point ballPos;
	int rad,pos_rad[4];
	GLTballDetect *bDetect=new GLTballDetect(side);
	
	raspicam::RaspiCam_Cv bCam;
	bCam.set(CV_CAP_PROP_FORMAT,CV_8UC1);
	bCam.set(CV_CAP_PROP_FRAME_WIDTH,640);
	bCam.set(CV_CAP_PROP_FRAME_HEIGHT,360);
	bCam.set(CV_CAP_PROP_EXPOSURE,1);
	if(!bCam.open())
	{
		cout<<"error-cannot open camera"<<endl;
		exit(-1);
	}
	
	while(1)
	{
		bCam.grab();
		bCam.retrieve(frameGray);
		
		ballPos=bDetect->ballPosition(rad,frameGray);
		pos_rad[0]=ballPos.x;
		pos_rad[1]=ballPos.y;
		pos_rad[2]=rad;
		
		gettimeofday(&now,NULL);
		pos_rad[3]=((now.tv_sec*1000000)+now.tv_usec)/1000;	//timestamp
		client->sendData(&pos_rad,4*sizeof(int));
	}
	delete client;
	bCam.release();
	/////////////////////////////////////////////////////////////////////////////
	
	return 0;
}