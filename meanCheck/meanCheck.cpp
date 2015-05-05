//tool for find right mean and variance
//for Goal-line Technology (Senior project)

#include<iostream>
#include<string>
#include<thread>
#include<opencv2/opencv.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/highgui/highgui_c.h>
#include<raspicam/raspicam_cv.h>
#include"GLTballDetect.h"

using namespace std;
using namespace cv;

int m_min=0,m_max=255,sd_min=0,sd_max=99999;
int out=0;

void region()
{
	string line;
	int num;
	while(1)
	{
		cout<<"select mean var : ";
		cin>>line;
		if(line=="q")
			out=1;
		else
		{
			string::size_type sz;
			num=stoi(line,&sz);
			if(num==1)
			{
				cout<<"min mean : ";
				cin>>num;
				m_min=num;
			}
			else if(num==2)	
			{
				cout<<"max mean : ";
				cin>>num;
				m_max=num;
			}
			else if(num==3)
			{
				cout<<"min var : ";
				cin>>num;
				sd_min=num;
			}
			else if(num==4)
			{
				cout<<"max var : ";
				cin>>num;
				sd_max=num;
			}
		}
	}
}

int main()
{
	Mat frame,gray,outimg;
	vector<Mat> sp(3);
	Scalar m,sd;
	Point pos;
	int rad;
	
	//camera settin/////////////////////////////////////////////////////////
	raspicam::RaspiCam_Cv live;
	live.set(CV_CAP_PROP_FORMAT,CV_8UC1);
	live.set(CV_CAP_PROP_FRAME_WIDTH,640);
	live.set(CV_CAP_PROP_FRAME_HEIGHT,360);
	live.set(CV_CAP_PROP_EXPOSURE,1);
	if(!live.open())
	{
		cout<<"error-cannot open camera"<<endl;
		return -1;
	}
	////////////////////////////////////////////////////////////////////////
	
	thread config(region);
	config.detach();
	GLTballDetect *bDetect=new GLTballDetect(1);
	while(out!=1)
	{
		//live//////////////////////////////////////////////////////////////////
		live.grab();
		live.retrieve(gray);
		////////////////////////////////////////////////////////////////////////
		
		meanStdDev(gray,m,sd);	//find mead,sd input frame
		pos=bDetect->ballPosition(rad,gray,outimg,m_min,m_max,sd_min,sd_max);
		
		stringstream ss1,ss2;
		string ms,sds;
		ss1<<m[0];
		sd[0]=sd[0]*sd[0];
		ss2<<sd[0];
		ms="mean-"+ss1.str();
		sds="var-"+ss2.str();
		rectangle(outimg,Point(10,10),Point(150,28),Scalar(255,255,255),-1);
		putText(outimg,ms.c_str(),Point(15,23),FONT_HERSHEY_SIMPLEX,0.5,Scalar(0,0,0));
		rectangle(outimg,Point(160,10),Point(300,28),Scalar(255,255,255),-1);
		putText(outimg,sds.c_str(),Point(165,23),FONT_HERSHEY_SIMPLEX,0.5,Scalar(0,0,0));
		stringstream m1,m2,v1,v2;
		m1<<m_min;
		m2<<m_max;
		v1<<sd_min;
		v2<<sd_max;
		string ms1,ms2,vs1,vs2;
		ms1="mn-"+m1.str();
		ms2="mx-"+m2.str();
		vs1="vn-"+v1.str();
		vs2="vx-"+v2.str();
		rectangle(outimg,Point(10,(outimg.rows)-25),Point(110,(outimg.rows)-10),Scalar(255,255,255),-1);
		putText(outimg,ms1.c_str(),Point(15,(outimg.rows)-12),FONT_HERSHEY_SIMPLEX,0.5,Scalar(0,0,0));
		rectangle(outimg,Point(120,(outimg.rows)-25),Point(220,(outimg.rows)-10),Scalar(255,255,255),-1);
		putText(outimg,ms2.c_str(),Point(125,(outimg.rows)-12),FONT_HERSHEY_SIMPLEX,0.5,Scalar(0,0,0));
		rectangle(outimg,Point(230,(outimg.rows)-25),Point(330,(outimg.rows)-10),Scalar(255,255,255),-1);
		putText(outimg,vs1.c_str(),Point(235,(outimg.rows)-12),FONT_HERSHEY_SIMPLEX,0.5,Scalar(0,0,0));
		rectangle(outimg,Point(340,(outimg.rows)-25),Point(440,(outimg.rows)-10),Scalar(255,255,255),-1);
		putText(outimg,vs2.c_str(),Point(345,(outimg.rows)-12),FONT_HERSHEY_SIMPLEX,0.5,Scalar(0,0,0));
		
		imshow("output",outimg);
		waitKey(1);
	}
	live.release();
	
	return 0;
}