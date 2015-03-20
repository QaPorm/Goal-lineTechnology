#include<iostream>
#include<string>
#include<thread>
#include<mutex>
#include<condition_variable>
#include<opencv2/opencv.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/highgui/highgui_c.h>
#include<opencv2/imgproc/imgproc.hpp>
#include"TCPserver.h"

using namespace std;
using namespace cv;

mutex mtx;
condition_variable connection;
int F_countModule=0;
int post[2],goal[3];
Point ball[3];
int rad[3];
int isGoal=0;

void resultDisplay()
{
	Mat nogoal=imread("images/nogoal.png",CV_LOAD_IMAGE_COLOR);
	Mat goal=imread("images/goal.png",CV_LOAD_IMAGE_COLOR);
	namedWindow("Result",CV_WINDOW_AUTOSIZE);
	while(1)
	{
		if(isGoal==1)
			imshow("Result",goal);
		else
			imshow("Result",nogoal);
		waitKey(1);
	}
	destroyWindow("Result");
}

void clientConnect(int side,string name)
{
	int port=side*1111;
	Mat img=Mat::zeros(480,640,CV_8UC3);	//360,640
	int bytes,imgsize=img.total()*img.elemSize();
	uchar sockData[imgsize];
	int32_t notice;
	int32_t buffer;
	int bufferArray[3];
	
	
	unique_lock<mutex> lck(mtx);
	//INITIALIZE STEP/////////////////////////////////////////////////////////////////////////////////
	cout<<"Initialize "<<name<<" Module."<<endl;
	cout<<" > Connecting...";
	cout.flush();
	TCPserver *serverInit=new TCPserver(port);
	cout<<".........................[OK]"<<endl;
	cout<<" > Adjusting Camera...";
	cout.flush();
	namedWindow("Adjusting Camera",CV_WINDOW_AUTOSIZE);
	while(waitKey(1)!=10)	//press ENTER to confirm
	{
		for(int i=0;i<imgsize;i+=bytes)
		{
			bytes=serverInit->recvData(sockData+i,imgsize-i);
			if(bytes==-1)
				exit(-1);
		}
		int ptr=0;
		for(int i=0;i<img.rows;i++)
		{
			for(int j=0;j<img.cols;j++)
			{
				img.at<cv::Vec3b>(i,j)=cv::Vec3b(sockData[ptr+0],sockData[ptr+1],sockData[ptr+2]);
				ptr=ptr+3;
			}
		}
		if(side==1||side==2)
			serverInit->recvData(&buffer,sizeof(buffer));	//recieve position in frame
		else if(side==3)
			serverInit->recvData(&bufferArray,3*sizeof(int));
		imshow("Adjusting Camera",img);
	}
	for(int i=0;i<5;i++)
	{
		destroyWindow("Adjusting Camera");
		waitKey(1);
	}
	notice=htonl(131);
	serverInit->sendData(&notice,sizeof(notice));
	cout<<"...................[OK]"<<endl;
	if(side==1||side==2)
	{
		post[side-1]=htonl(buffer);
		cout<<" > Goal-line position....................["<<post[side-1]<<"]"<<endl;
	}
	else if(side==3)
	{
		for(int i=0;i<3;i++)
			goal[i]=bufferArray[i];
		cout<<" > Bar position..........................["<<goal[0]<<"]"<<endl;
		cout<<" > Left Post position....................["<<goal[1]<<"]"<<endl;
		cout<<" > Right Post position...................["<<goal[2]<<"]"<<endl;
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////
	
	//POSITION CALIBRATE//////////////////////////////////////////////////////////////////////////////
	/*string line;
	while(1)
	{
		cout<<" > Position calibrate? ..................[Y/N] > ";
		cin>>line;
		if(line=="Y"||line=="y")
		{
			//calibrate func
		}
		else if(line=="N"||line=="n")
			break;
	}
	cout<<endl;*/
	//////////////////////////////////////////////////////////////////////////////////////////////////
	
	F_countModule++;	
	while(F_countModule!=4)
		connection.wait(lck);
	lck.unlock();
	notice=htonl(845);	//rdy
	serverInit->sendData(&notice,sizeof(notice));
	delete serverInit;
	port=(side*1111)+50;	//port 1161, 2271, 3381
	TCPserver *serverProcess=new TCPserver(port);
	
	//PROCESSING STEP/////////////////////////////////////////////////////////////////////////////////
	while(1)
	{
		lck.lock();
		serverProcess->recvData(&bufferArray,3*sizeof(int));
		ball[side-1]=Point(bufferArray[0],bufferArray[1]);
		rad[side-1]=bufferArray[2];
		cout<<ball[side-1]<<" "<<rad[side-1]<<endl;
		lck.unlock();
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////
	
	delete serverProcess;
}


int main()
{
	system("clear");
	cout<<"////////////////////////////////////////////////////////"<<endl
		<<"///                                                  ///"<<endl
		<<"///               GOAL-LINE TECHNOLOGY               ///"<<endl
		<<"///                                                  ///"<<endl
		<<"////////////////////////////////////////////////////////"<<endl<<endl;
	
	thread lModule(clientConnect,1,"LEFT");
	sleep(1);	//for delay thread create to make thread following sequence (left -> right -> back)
	thread rModule(clientConnect,2,"RIGHT");
	sleep(1);
	thread bModule(clientConnect,3,"BACK");
	while(F_countModule!=3)
		sleep(1);
	F_countModule++;
	cout<<"System synchronization"<<endl;
	connection.notify_all();
	thread result(resultDisplay);
	result.detach();
	lModule.join();
	rModule.join();
	bModule.join();
	
	return 0;
}



//countModule update before each finish
//fist second third init finish wait
//mainthread check countModule==3
//- mainthread countModule++
//  notify all 
//- wait 10 sec and notify all
//while loop sleep for waiting (client)
//send notice to client tell that server will create
//delete sock1
//exit loop (client)
//client wait for 10
//create server
//client create
//ifconnect start loop recv, send