#ifndef __GLTFRAMEDETECT_H
#define __GLTFRAMEDETECT_H
#include<iostream>
#include<opencv2/opencv.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/highgui/highgui_c.h>
#include<opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

class GLTframeDetect
{
private:
	int side;
	int *outputPos;
	Point temp_total[2];
	Point temp_avrtotal[2];
	vector<int> lineCollection;
	vector<int>::iterator lineCollection_it;
	void setTempPoint(Point &temp1,Point &temp2,Point pt1,Point pt2);
	void averageLine(Point &output1,Point &output2,Point input1,Point input2,int axis);
public:
	GLTframeDetect(int moduleSide);
	~GLTframeDetect();
	int *framePosition(Point (&rawOutput)[6],Point (&avrOutput)[6],Mat input,Mat &out);
};

#endif