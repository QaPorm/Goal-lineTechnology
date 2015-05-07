//////////////////////////////////////////////////////////////////////////////
//	GOAL-LINE TECHNOLOGY													//
//	Senior Project 2014 (Computer Engineering)								//
//	Faculty of Engineering, Mahidol University								//
//	Nattaya Sriphop															//
//	Patchara Chanprakhon													//
//	Rungroj Somwong															//
//////////////////////////////////////////////////////////////////////////////
//	GLTballDetect.h - ball detecttion and positioning (header).				//
//////////////////////////////////////////////////////////////////////////////

#ifndef __GLTBALLDETECT_H
#define __GLTBALLDETECT_H
#include<iostream>
#include<cmath>
#include<opencv2/opencv.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/highgui/highgui_c.h>
#include<opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

class GLTballDetect
{
private:
	int side;
public:
	GLTballDetect(int moduleSide);
	Point ballPosition(int &radius,Mat input);
};

#endif