#pragma once
#include "Head.h"

typedef struct num_disp
{
	float disp=0;
	int num=0;
}num_disp;

class DisparityMapRefine
{
private:
	Mat img;
	Mat img_disp;
	float thres_color;
	int thres_distance;
	int k = 5;
	Mat edge;

	vector<vector<Point>> cross(Point);
	vector<vector<Point>>  cross_(Point);

	bool judge(Point, Point);

	vector<num_disp> histogram(vector<vector<Point>>);

	Mat gx();
	Mat gy();
	Mat g1x();
	Mat g1y();
	Mat gaussianEdge();

public:
	DisparityMapRefine(Mat a, Mat b, float c, int d)
	{
		assert(a.channels() == 3 && b.channels() == 1);
		img = a;
		img_disp = b;
		thres_color = c;
		thres_distance = d;
		edge = gaussianEdge();
	}

	Mat disp_refine();
};