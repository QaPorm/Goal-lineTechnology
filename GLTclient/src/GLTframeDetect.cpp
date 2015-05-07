//////////////////////////////////////////////////////////////////////////////
//	GOAL-LINE TECHNOLOGY													//
//	Senior Project 2014 (Computer Engineering)								//
//	Faculty of Engineering, Mahidol University								//
//	Nattaya Sriphop															//
//	Patchara Chanprakhon													//
//	Rungroj Somwong															//
//////////////////////////////////////////////////////////////////////////////
//	GLTframeDetect.cpp - goal frame detection and positioning.				//
//					     ***suitable for Mahidol University football field.	//
//////////////////////////////////////////////////////////////////////////////

#include"GLTframeDetect.h"

#define LEFT_POST_AREA 200
#define RIGHT_POST_AREA 440

using namespace std;
using namespace cv;

//GLTframeDetect Constructor
GLTframeDetect::GLTframeDetect(int moduleSide)
{
	if(moduleSide<1||moduleSide>3)
	{
		cout<<"error-module side"<<endl;
		exit(-1);
	}
	side=moduleSide;
	if(side==1)
	{
		temp_avrtotal[0].x=1000;
		temp_avrtotal[1].x=1000;
	}
	else if(side==2)
	{
		temp_avrtotal[0].x=0;
		temp_avrtotal[1].x=0;
	}
	outputPos=(int *)malloc(3);
}

//GLTframeDetect Destructor
GLTframeDetect::~GLTframeDetect()
{
	free(outputPos);
}

//set value of Point variables (2 variables)
void GLTframeDetect::setTempPoint(Point &temp1,Point &temp2,Point pt1,Point pt2)
{
	temp1=pt1;
	temp2=pt2;
}

//average line to make 90 degree line
//vertical   -> axis=1;
//horizontal -> axis=2;
//use triangle properties to move line's point(2 points) from outer to inner image, then average it.
void GLTframeDetect::averageLine(Point &output1,Point &output2,Point input1,Point input2,int axis)
{
	Point top,down;
	int avr,H,h1,h2,W,w1,w2;
	
	if(axis==1)
	{
		//find top,down point
		if(input1.y<input2.y)
		{
			top=input1;
			down=input2;
		}
		else if(input1.y>input2.y)
		{
			top=input2;
			down=input1;
		}
		else if(input1.y==input2.y)
		{
			top=input1;
			down=input2;
		}
		H=(top.y*(-1))+down.y;
		h1=(top.y*(-1));
		h2=(top.y*(-1))+480;
		W=abs(top.x-down.x);
		if(H!=0)
		{
			w1=(h1*W)/H;
			w2=(h2*W)/H;
		}
		else
		{
			w1=0;
			w2=0;
		}
		//set point what it in image area
		if(top.x>down.x)
		{
			down.x=top.x-w2;
			top.x=top.x-w1;
			top.y=0;
			down.y=480;
		}
		else if(top.x<down.x)
		{
			down.x=top.x+w2;
			top.x=top.x+w1;
			top.y=0;
			down.y=480;
		}
		//average 2 point for 90 degree line
		avr=(top.x+down.x)/2;
		top.x=avr;
		down.x=avr;
	}
	else if(axis==2)
	{
		if(input1.x<input2.x)
		{
			top=input1;
			down=input2;
		}
		else if(input1.x>input2.x)
		{
			top=input2;
			down=input1;
		}
		else if(input1.x==input2.x)
		{
			top=input1;
			down=input2;
		}
		H=(top.x*(-1))+down.x;
		h1=(top.x*(-1));
		h2=(top.x*(-1))+640;
		W=abs(top.y-down.y);
		if(H!=0)
		{
			w1=(h1*W)/H;
			w2=(h2*W)/H;
		}
		else
		{
			w1=0;
			w2=0;
		}
		if(top.y>down.y)
		{
			down.y=top.y-w2;
			top.y=top.y-w1;
			top.x=0;
			down.x=640;
		}
		else if(top.y<down.y)
		{
			down.y=top.y+w2;
			top.y=top.y+w1;
			top.x=0;
			down.x=640;
		}
		avr=(top.y+down.y)/2;
		top.y=avr;
		down.y=avr;
	}
	else
	{
		cout<<"wrong-axis"<<endl;
		exit(-1);
	}
	output1=top;
	output2=down;
}

//find pixel's position of goal frame in frame
//left module  -> side=1
//right module -> side=2
//back module  -> side=3
//give points of raw line from hough line and points of average line of raw line (pass by reference) to draw them later.
int *GLTframeDetect::framePosition(Point (&rawOutput)[6],Point (&avrOutput)[6],Mat input)
{
	Mat binary,edge,hsv;
	vector<Mat> hsv_sp;
	vector<Vec2f> lines;
	Point pt1,pt2;
	float rho,theta;
	double a,b,x0,y0;
	Point temp[6],temp_avr[2];
	int sum,avr,avrZone[2];
	
	//initial
	for(int i=0;i<6;i++)
		temp[i]=Point(0,0);
	if(side==1)
	{
		temp[0].x=1000;
		temp[1].x=1000;
	}
	else if(side==2)
	{
		temp[0].x=0;
		temp[1].x=0;
	}
	else if(side==3)
	{
		temp[0].y=1000;
		temp[1].y=1000;
		temp[2].x=0;
		temp[3].x=0;
		temp[4].x=1000;
		temp[5].x=1000;
	}
	
	/////////////////////////////////////////////////////////////////////////////////////////////
	//frame's image processing
	inRange(input,Scalar(255*0.6,255*0.6,255*0.6),Scalar(255,255,255),binary);
	Canny(binary,edge,70,210,3);
	HoughLines(edge,lines,1,CV_PI/180,50,0,0);
	/////////////////////////////////////////////////////////////////////////////////////////////

	for(size_t i=0;i<lines.size();i++)
	{
		rho=lines[i][0];
		theta=lines[i][1];
		a=cos(theta);
		b=sin(theta);
		x0=a*rho;
		y0=b*rho;
		pt1.x=cvRound(x0+1000*(-b));
		pt1.y=cvRound(y0+1000*(a));
		pt2.x=cvRound(x0-1000*(-b));
		pt2.y=cvRound(y0-1000*(a));
		if(side==1)
		{
			if(pt1.x>-900&&pt1.x<900&&pt2.x>-900&&pt2.x<900)	//post (vertical)
			{
				if(pt1.x<temp[0].x&&pt2.x<temp[1].x)
					setTempPoint(temp[0],temp[1],pt1,pt2);
			}
		}
		else if(side==2)
		{
			if(pt1.x>-900&&pt1.x<900&&pt2.x>-900&&pt2.x<900)	//post (vertical)
			{
				if(pt1.x>temp[0].x&&pt2.x>temp[1].x)
					setTempPoint(temp[0],temp[1],pt1,pt2);
			}
		}
		else if(side==3)
		{
			if(pt1.y>-900&&pt1.y<900&&pt2.y>-900&&pt2.y<900)	//bar (horizontal)
			{
				if(pt1.y<temp[0].y&&pt2.y<temp[1].y)
					setTempPoint(temp[0],temp[1],pt1,pt2);
			}
			else if(pt1.x>-900&&pt1.x<900&&pt2.x>-900&&pt2.x<900)	//post (vertical)
			{
				if(pt1.x<LEFT_POST_AREA&&pt2.x<LEFT_POST_AREA)	//left post (find from this area)
				{
					if(pt1.x>temp[2].x&&pt2.x>temp[3].x)
						setTempPoint(temp[2],temp[3],pt1,pt2);
				}
				else if(pt1.x>RIGHT_POST_AREA&&pt2.x>RIGHT_POST_AREA)	//right post (find from this area)
				{
					if(pt1.x<temp[4].x&&pt2.x<temp[5].x)
						setTempPoint(temp[4],temp[5],pt1,pt2);
				}
			}
		}
		else
		{
			cout<<"error-wrong side"<<endl;
			exit(-1);
		}
	}
	if(side==1||side==2)
	{
		averageLine(temp_avr[0],temp_avr[1],temp[0],temp[1],1);
		if(lineCollection.size()==30)
			lineCollection.erase(lineCollection.begin());
		lineCollection.push_back(temp_avr[0].x);
		for(sum=0,lineCollection_it=lineCollection.begin();lineCollection_it!=lineCollection.end();++lineCollection_it)
			sum=sum+(*lineCollection_it);
		avr=sum/lineCollection.size();
		avrZone[0]=avr-10;
		avrZone[1]=avr+10;
		if(temp_avrtotal[0].x<avrZone[0]||temp_avrtotal[0].x>avrZone[1])	//if zone change, move line to zone
		{
			temp_total[0].x=avr;
			temp_total[1].x=avr;
			temp_avrtotal[0].x=avr;
			temp_avrtotal[1].x=avr;
		}
		if(temp_avr[0].x>=avrZone[0]&&temp_avr[0].x<=avrZone[1])	//find fittest line in zone
		{
			if(side==1)
			{
				if(temp_avr[0].x<=temp_avrtotal[0].x)
				{
					temp_total[0]=temp[0];
					temp_total[1]=temp[1];
					temp_avrtotal[0]=temp_avr[0];
					temp_avrtotal[1]=temp_avr[1];
				}
			}
			else if(side==2)
			{
				if(temp_avr[0].x>=temp_avrtotal[0].x)
				{
					temp_total[0]=temp[0];
					temp_total[1]=temp[1];
					temp_avrtotal[0]=temp_avr[0];
					temp_avrtotal[1]=temp_avr[1];
				}
			}
		}
		rawOutput[0]=temp_total[0];
		rawOutput[1]=temp_total[1];
		avrOutput[0]=temp_avrtotal[0];
		avrOutput[1]=temp_avrtotal[1];
		outputPos[0]=temp_avrtotal[0].x;	//left or right post
	}
	else if(side==3)
	{
		for(int i=0;i<6;i++)
			rawOutput[i]=temp[i];
		averageLine(temp[0],temp[1],temp[0],temp[1],2);
		averageLine(temp[2],temp[3],temp[2],temp[3],1);
		averageLine(temp[4],temp[5],temp[4],temp[5],1);
		for(int i=0;i<6;i++)
			avrOutput[i]=temp[i];
		outputPos[0]=temp[0].y;	//bar
		outputPos[1]=temp[2].x;	//left post
		outputPos[2]=temp[4].x;	//right post
	}
	return outputPos;
}