#pragma once
#include "DisparityMapRefine.h"
//#include "CCC/COMCV.h"
#include <fstream>

using namespace std;

void writPFM(Mat_<float> &disp, float &scale, string path)
{
	ofstream out(path, ios_base::binary);

	out << "Pf" << endl << disp.cols << " " << disp.rows << endl << scale << endl;
	for (int i = disp.rows - 1; i >= 0; i--)//因为存储是从最后行开始存储的
		for (int j = 0; j < disp.cols; j++)
			out.write((const char*)(&disp(i, j)), sizeof(float));

	out.close();
}
void readPFM(Mat_<float> &disp, float &scale, string path)
{
	ifstream in(path, ios_base::binary);

	int cols;
	int rows;
	char tmp[10];
	in.get(tmp, 10, '\n'); in.get();//文件头
	in.get(tmp, 10, ' '); in.get(); cols = atoi(tmp);//列数
	in.get(tmp, 10, '\n'); in.get(); rows = atoi(tmp); //行数
	in.get(tmp, 10, '\n'); in.get(); scale = atof(tmp);//缩放因子

	disp.create(rows, cols);
	for (int i = disp.rows - 1; i >= 0; i--)//因为存储是从最后行开始存储的
		for (int j = 0; j < disp.cols; j++)
			in.read((char*)(&disp(i, j)), sizeof(float));

	in.close();
}