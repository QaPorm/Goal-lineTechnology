#include"GLTballDetect.h"
#define MIN_MEAN 100
#define MAX_MEAN 150
#define MIN_VARS 7000
#define MAX_VARS 9200

using namespace std;
using namespace cv;

//GLTballDetect Constructor
GLTballDetect::GLTballDetect(int moduleSide)
{
	if(moduleSide<1||moduleSide>3)
	{
		cout<<"error-module side"<<endl;
		exit(-1);
	}
	side=moduleSide;
}

Point GLTballDetect::ballPosition(int &radius,Mat input)
{
	Mat avgImg,moving,subImg,roiImg;
	Mat openEle=getStructuringElement(MORPH_ELLIPSE,Size(4,4));
	Mat closeEle=getStructuringElement(MORPH_ELLIPSE,Size(10,10));
	vector<Vec3f> circles;
	Point center;
	int rad,dis;
	Scalar mean,sd;
	int variance;
	
	if(side==1||side==2)
		HoughCircles(input,circles,CV_HOUGH_GRADIENT,1,50,255,10,5,12);
	else if(side==3)
		HoughCircles(input,circles,CV_HOUGH_GRADIENT,1,50,255,10,3,9);
	for(int i=0;i<circles.size();i++)
	{
		center=Point(cvRound(circles[i][0]),cvRound(circles[i][1]));
		rad=cvRound(circles[i][2]);
		subImg=Mat::zeros(rad*2,rad*2,CV_8UC1);	//create subimage of circle
		circle(subImg,Point(rad,rad),rad,Scalar(255,255,255),-1,8,0);
		Rect roi=Rect((center.x)-rad,(center.y)-rad,rad*2,rad*2);	//select roi (circle)
		if(roi.x>=0&&roi.y>=0&&roi.width+roi.x<input.cols&&roi.height+roi.y<input.rows)
		{
			roiImg=input(roi);
			bitwise_and(subImg,roiImg,subImg);	//circle subimage
			meanStdDev(subImg,mean,sd);
			variance=pow(sd[0],2);
			if(mean[0]>=MIN_MEAN&&mean[0]<=MAX_MEAN&&variance>=MIN_VARS&&variance<=MAX_VARS)
				circle(input,center,rad,Scalar(255,255,255),2,8,0);
		}
	}
	radius=rad;
	return center;
}