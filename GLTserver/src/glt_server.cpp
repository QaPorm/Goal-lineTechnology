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
int countModule=0,isGoal=0;
int post[2],goal[3];
Point ball[3];
int rad[3],timestamp[3];

void resultDisplay()
{
	Mat nogoal=imread("images/nogoal.png",CV_LOAD_IMAGE_COLOR);
	Mat goal=imread("images/goal.png",CV_LOAD_IMAGE_COLOR);
	namedWindow("Result",CV_WINDOW_AUTOSIZE);
	while(1)
	{
		if(isGoal==0)
		{
			imshow("Result",nogoal);
			waitKey(1);
		}
		else if(isGoal==1)
		{
			while(waitKey(1)!=10)
				imshow("Result",goal);
			isGoal=0;
		}
	}
	destroyWindow("Result");
}

void clientConnect(int side,string name)
{
	int port=side*1111;
	Mat img=Mat::zeros(360,640,CV_8UC3);	//360,640
	int bytes,imgsize=img.total()*img.elemSize();
	uchar sockData[imgsize];
	int32_t notice;
	int32_t buffer;
	int bufferArray[3],bufferRecv[4];
	
	unique_lock<mutex> lck(mtx);
	//INITIALIZE STEP/////////////////////////////////////////////////////////////////////////////////
	cout<<"Initialize "<<name<<" Module."<<endl;
	cout<<" > Connecting...";
	cout.flush();
	TCPserver *serverInit=new TCPserver(port);
	cout<<"................................[OK]"<<endl;
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
	notice=htonl(131);
	serverInit->sendData(&notice,sizeof(notice));
	cout<<"..........................[OK]"<<endl;
	if(side==1||side==2)
	{
		post[side-1]=htonl(buffer);
		cout<<" > Goal-line position...........................["<<post[side-1]<<"]"<<endl;
	}
	else if(side==3)
	{
		for(int i=0;i<3;i++)
			goal[i]=bufferArray[i];
		cout<<" > Bar position.................................["<<goal[0]<<"]"<<endl;
		cout<<" > Left Post position...........................["<<goal[1]<<"]"<<endl;
		cout<<" > Right Post position..........................["<<goal[2]<<"]"<<endl;
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////
	
	//POSITION CALIBRATE//////////////////////////////////////////////////////////////////////////////
	Point car[2];
	Mat showCar;
	int key,temp[3];
	if(side==1||side==2)
	{
		car[0]=Point(post[side-1],-1000);
		car[1]=Point(post[side-1],1000);
		if(car[0].x<=0)
		{
			car[0]=Point(5,-1000);
			car[1]=Point(5,1000);
		}
		while(1)
		{
			showCar=img.clone();
			line(showCar,car[0],car[1],Scalar(255,0,0),1,CV_AA);
			imshow("Adjusting Camera",showCar);
			key=waitKey(1);
			if(key==65361) // <-
			{
				car[0].x-=1;
				car[1].x-=1;
			}
			else if(key==65363)	// ->
			{
				car[0].x+=1;
				car[1].x+=1;
			}
			if(key==10)
				break;
		}
		for(int i=0;i<5;i++)
		{
			destroyWindow("Adjusting Camera");
			waitKey(1);
		}
		if(post[side-1]!=car[0].x)
		{
			cout<<" > Calibrated position..........................["<<car[0].x<<"]"<<endl;
			post[side-1]=car[0].x;
		}
	}
	else if(side==3)
	{
		for(int i=0;i<3;i++)
		{
			if(i==0)
			{
				car[0]=Point(-1000,goal[0]);
				car[1]=Point(1000,goal[0]);
				if(car[0].y<=0)
				{
					car[0]=Point(-1000,5);
					car[1]=Point(1000,5);
				}
				while(1)
				{
					showCar=img.clone();
					line(showCar,car[0],car[1],Scalar(255,0,0),1,CV_AA);
					imshow("Adjusting Camera",showCar);
					key=waitKey(1);
					if(key==65362) // ^
					{
						car[0].y-=1;
						car[1].y-=1;
					}
					else if(key==65364)	// v
					{
						car[0].y+=1;
						car[1].y+=1;
					}
					if(key==10)
						break;
				}
				temp[0]=car[0].y;
			}
			else
			{
				car[0]=Point(goal[i],-1000);
				car[1]=Point(goal[i],1000);
				if(car[0].x<=0)
				{
					car[0]=Point(5,-1000);
					car[1]=Point(5,1000);
				}
				else if(car[0].x>=640)
				{
					car[0]=Point(635,-1000);
					car[1]=Point(635,1000);
				}
				while(1)
				{
					showCar=img.clone();
					line(showCar,car[0],car[1],Scalar(255,0,0),1,CV_AA);
					imshow("Adjusting Camera",showCar);
					key=waitKey(1);
					if(key==65361) // <-
					{
						car[0].x-=1;
						car[1].x-=1;
					}
					else if(key==65363)	// ->
					{
						car[0].x+=1;
						car[1].x+=1;
					}
					if(key==10)
						break;
				}
				temp[i]=car[0].x;
			}
		}
		for(int i=0;i<5;i++)
		{
			destroyWindow("Adjusting Camera");
			waitKey(1);
		}
		if(goal[0]!=temp[0])
		{
			cout<<" > Calibrated bar position......................["<<temp[0]<<"]"<<endl;
			goal[0]=temp[0];
		}
		if(goal[1]!=temp[1])
		{
			cout<<" > Calibrated left post position................["<<temp[1]<<"]"<<endl;
			goal[1]=temp[1];
		}
		if(goal[2]!=temp[2])
		{
			cout<<" > Calibrated right post position...............["<<temp[2]<<"]"<<endl;
			goal[2]=temp[2];
		}
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////
	
	countModule++;	
	while(countModule!=4)
		connection.wait(lck);
	lck.unlock();
	notice=htonl(845);	//rdy
	serverInit->sendData(&notice,sizeof(notice));
	delete serverInit;
	port=(side*1111)+50;	//port 1161, 2272, 3383
	TCPserver *serverProcess=new TCPserver(port);
	
	//PROCESSING STEP/////////////////////////////////////////////////////////////////////////////////
	while(1)
	{
		serverProcess->recvData(&bufferRecv,4*sizeof(int));
		ball[side-1]=Point(bufferRecv[0],bufferRecv[1]);
		rad[side-1]=bufferRecv[2];
		timestamp[side-1]=bufferRecv[3];
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////
	
	delete serverProcess;
}

int main()
{
	int leftGoal=0,rightGoal=0,backGoal=0;
	
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
	while(countModule!=3)
		sleep(1);
	countModule++;
	cout<<"System synchronization"<<endl;
	connection.notify_all();
	thread result(resultDisplay);
	result.detach();
	
	//DECISION////////////////////////////////////////////////////////////////////////////////////////
	while(1)
	{
		leftGoal=0;
		rightGoal=0;
		backGoal=0;
		//set condition
		if((ball[0].x+rad[0])<post[0])
			leftGoal=1;
		if((ball[1].x-rad[1])>post[1])
			rightGoal=1;
		if((ball[2].y-rad[2]>goal[0])&&(ball[2].x-rad[2]>goal[1])&&(ball[2].x+rad[2]<goal[2]))
			backGoal=1;
		else if((ball[2].y-rad[2]<=goal[0])&&(ball[2].x-rad[2]<=goal[1])&&(ball[2].x+rad[2]>=goal[2]))	//out goal but can see in frame
			backGoal=2;
		//check condition
		
		
		if(backGoal==0)
		{
			if(leftGoal==1&&rightGoal==1)
			{
				if(sqrt(pow(ball[0].x-ball[1].x,2)+pow(ball[0].y-ball[1].y,2))<=20)	//check left and right has similar position
					isGoal=1;
			}
		}
		else if(backGoal==1)
		{
			if(leftGoal==1)
			{
				if((ball[0].y<=200&&ball[2].y<=200)||(ball[0].y>=160&&ball[2].y>=160))	//check left and back has smilar height
					isGoal=1;
			}
			if(rightGoal==1)
			{
				if((ball[1].y<=200&&ball[2].y<=200)||(ball[1].y>=160&&ball[2].y>=160))	//check right and back has similar height
					isGoal=1;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////
	
	lModule.join();
	rModule.join();
	bModule.join();
	
	return 0;
}


//init and processing step, link protocol
//(server)each thread countModule++ after finish
//(server)all thread except mainthread wait
//(server)mainthread check countModule==3
//(server)mainthread countModule++
//(server)notify all 
//(client)while loop sleep for waiting
//(server)send notice to client tell that serverProcess will create
//(server)delete serverInit
//(client)exit loop
//(client)wait for 10 s
//(server)create serverProcess
//(client)connect
//(server)after connect start loop recv
//(client)after connebt start loop send