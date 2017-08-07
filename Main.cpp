#include "pfm.h"

int main()
{
	int a = 1;
	if (a == 1)
	{
		Mat img = imread("trainingQ//Vintage//im0.png");
		Mat img_disp;

		Mat_<float> &disp=(Mat_<float>&) img_disp;
		float scale = 1.0;

		readPFM(disp, scale, "trainingQ//Vintage//disp0XXX.pfm");


		DisparityMapRefine test(img, img_disp, 15, 30);

		
		Mat res = test.disp_refine();

		Mat img_filter(res.rows, res.cols, CV_32FC1);
		medianBlur(res, img_filter, 5);

		Mat img_show(img.rows, img.cols, CV_8UC1);
		convertScaleAbs(img_filter, img_show);

		imshow("color", img);
		imshow("res", img_show);
		waitKey(0);

		Mat_<float> &img_res = (Mat_<float>&) img_filter;
		writPFM(img_res, scale, "trainingQ//Vintage//disp0HX.pfm");
	}
	if (a == 0)
	{
		Mat img_refine = imread("trainingH//Vintage//disp_refine.png");

		Mat img_filter(img_refine.rows,img_refine.cols,CV_8UC1);

		//adaptiveBilateralFilter(img_refine, img_filter, Size(5, 5), 20);
		medianBlur(img_refine, img_filter, 5);

		cvtColor(img_filter, img_filter, CV_BGR2GRAY);

		imshow("img", img_refine);
		imshow("filter", img_filter);
		imwrite("trainingH//Vintage//disp0HX.png", img_filter);
		waitKey(0);
	}
}