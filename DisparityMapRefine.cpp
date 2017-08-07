#include "DisparityMapRefine.h"

Mat DisparityMapRefine::gx()
{
	int p = 2 * k + 1;
	Mat x(p, p, CV_32F);
	for (int i = 0; i < x.rows; i++)
	{
		for (int j = 0; j < x.cols; j++)
		{
			x.at<float>(i, j) = j - 5;
		}
	}
	return x;
}

Mat DisparityMapRefine::gy()
{
	int p = 2 * k + 1;
	Mat y(p, p, CV_32F);
	for (int i = 0; i < y.rows; i++)
	{
		for (int j = 0; j < y.cols; j++)
		{
			y.at<float>(i, j) = 5 - i;
		}
	}
	return y;
}

Mat DisparityMapRefine::g1x()
{
	Mat x = gx();
	Mat y = gy();
	Mat x1(x.rows, x.cols, CV_32F);
	for (int i = 0; i < x1.rows; i++)
	{
		for (int j = 0; j < x1.cols; j++)
		{
			float a = x.at<float>(i, j);
			float b = y.at<float>(i, j);
			x1.at<float>(i, j) = (-a / (2 * pi))*(exp(-(a*a + b*b) / (2)));
		}
	}
	return x1;
}

Mat DisparityMapRefine::g1y()
{
	Mat x = gx();
	Mat y = gy();
	Mat y1(x.rows, x.cols, CV_32F);
	for (int i = 0; i < y1.rows; i++)
	{
		for (int j = 0; j < y1.cols; j++)
		{
			float a = x.at<float>(i, j);
			float b = y.at<float>(i, j);
			y1.at<float>(i, j) = (-b / (2 * pi))*(exp(-(a*a + b*b) / (2)));
		}
	}
	return y1;
}


Mat DisparityMapRefine::gaussianEdge()
{
	Mat img_edge_x(img.rows, img.cols, CV_32F);
	Mat img_edge_y(img.rows, img.cols, CV_32F);
	Mat x1 = g1x();
	Mat y1 = g1y();

	Mat disp_gray(img.rows, img.cols, CV_8UC1);
	convertScaleAbs(img_disp, disp_gray);


	filter2D(disp_gray, img_edge_x, CV_32F, x1);
	filter2D(disp_gray, img_edge_y, CV_32F, y1);

	Mat disp_edge(img.rows, img.cols, CV_32F);
	for (int i = 0; i < img.rows; i++)
	{
		for (int j = 0; j < img.cols; j++)
		{
			float a = img_edge_x.at<float>(i, j);
			float b = img_edge_y.at<float>(i, j);
			float c = sqrt(a*a + b*b);
			if (c < 3) { disp_edge.at<float>(i, j) = 0; }
			else {
				disp_edge.at<float>(i, j) = c;
			}
		}
	}

	Mat disp_edge_8u(img.rows, img.cols, CV_8UC1);
	convertScaleAbs(disp_edge, disp_edge_8u, 10);

	return disp_edge_8u;
}

bool DisparityMapRefine::judge(Point a, Point b)
{
	Vec3b a_color = img.at<Vec3b>(a.y, a.x);
	Vec3b b_color = img.at<Vec3b>(b.y, b.x);
	float diff_color = sqrt((a_color[0] - b_color[0])*(a_color[0] - b_color[0]) + (a_color[1] - b_color[1])*(a_color[1] - b_color[1]) + (a_color[2] - b_color[2])*(a_color[2] - b_color[2]));
	int distance = sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
	if ((diff_color <= thres_color) && (distance <= thres_distance))
	{
		return true;
	}
	else
	{
		return false;
	}
}

vector<vector<Point>>  DisparityMapRefine::cross(Point center)
{
	vector<vector<Point>> cross_region;
	//down
	for (int i = 0;; i++)
	{
		if ((center.y + i) >= img.rows)
		{
			break;
		}
		Point neighbor(center.x, center.y + i);
		if (!judge(center, neighbor))
		{
			break;
		}

		vector<Point> points;
		points.push_back(neighbor);
		//right
		for (int j = 1;; j++)
		{
			if ((neighbor.x + j) >= img.cols)
			{
				break;
			}
			Point right(neighbor.x + j, neighbor.y);
			if (!judge(neighbor, right))
			{
				break;
			}
			points.push_back(right);
		}

		//left
		for (int j = 1;; j++)
		{
			if ((neighbor.x - j) < 0)
			{
				break;
			}
			Point left(neighbor.x - j, neighbor.y);
			if (!judge(neighbor, left))
			{
				break;
			}
			points.push_back(left);
		}
		cross_region.push_back(points);
	}

	//up
	for (int i = 1;; i++)
	{
		if ((center.y - i) <0)
		{
			break;
		}
		Point neighbor(center.x, center.y - i);
		if (!judge(center, neighbor))
		{
			break;
		}

		vector<Point> points;
		points.push_back(neighbor);
		//right
		for (int j = 1;; j++)
		{
			if ((neighbor.x + j) >= img.cols)
			{
				break;
			}
			Point right(neighbor.x + j, neighbor.y);
			if (!judge(neighbor, right))
			{
				break;
			}
			points.push_back(right);
		}

		//left
		for (int j = 1;; j++)
		{
			if ((neighbor.x - j) < 0)
			{
				break;
			}
			Point left(neighbor.x - j, neighbor.y);
			if (!judge(neighbor, left))
			{
				break;
			}
			points.push_back(left);
		}
		cross_region.push_back(points);
	}

	return cross_region;
}

vector<num_disp> DisparityMapRefine::histogram(vector<vector<Point>> cross_region)
{
	vector<num_disp> histogram;
	for (int i = 0; i < cross_region.size(); i++)
	{
		vector<Point> points = cross_region[i];
		for (int j = 0; j < points.size(); j++)
		{
			int disp = img_disp.at<float>(points[j].y, points[j].x);
			bool existence=false;
			for (int k = 0; k < histogram.size(); k++)
			{
				if (histogram[k].disp == disp)
				{
					existence = true;
					histogram[k].num++;
					break;
				}
			}
			if (!existence)
			{
				num_disp e;
				e.disp = disp;
				e.num=1;
				histogram.push_back(e);
			}
		}
	}

	return histogram;
}

Mat DisparityMapRefine::disp_refine()
{
	Mat img_disp_refine = img_disp.clone();
	for (int i = 0; i < img.rows; i++)
	{
		for (int j = 0; j < img.cols; j++)
		{
			Point center(j, i);
			vector<vector<Point>> cross_region = cross(center);
			vector<num_disp> hist = histogram(cross_region);

			if (hist.size() > 0)
			{
				num_disp max = hist[0];
				for (int q = 0; q < hist.size(); q++)
				{
					if (hist[q].num > max.num)
					{
						max = hist[q];
					}
				}
				float v = img_disp_refine.at<float>(i, j);

				int sum = 0;
				for (int i = 0; i < cross_region.size(); i++)
				{
					for (int j = 0; j < cross_region[i].size(); j++)
					{
						int x = cross_region[i][j].x;
						int y = cross_region[i][j].y;
						if (edge.at<uchar>(y, x) != 0)
						{
							sum++;
						}
					}
				}

				if (abs(v - max.disp)>10) 
				{
					img_disp_refine.at<float>(i, j) = max.disp;
				}
			}
		}
	}
	return img_disp_refine;
}




//提升视觉效果
vector<vector<Point>>  DisparityMapRefine::cross_(Point center)
{
	vector<vector<Point>> cross_region;
	//down
	for (int i = 1; i <= thres_distance; i++)
	{
		if ((center.y + i) >= img.rows)
		{
			break;
		}
		Point neighbor(center.x, center.y + i);
		if (judge(center, neighbor))
		{
			vector<Point> points;
			points.push_back(neighbor);


			//right
			for (int j = 1; j <= thres_distance; j++)
			{
				if ((neighbor.x + j) >= img.cols)
				{
					break;
				}
				Point right(neighbor.x + j, neighbor.y);
				if (judge(neighbor, right))
				{
					points.push_back(right);
				}
			}

			//left
			for (int j = 1; j <= thres_distance; j++)
			{
				if ((neighbor.x - j) < 0)
				{
					break;
				}
				Point left(neighbor.x - j, neighbor.y);
				if (judge(neighbor, left))
				{
					points.push_back(left);
				}
			}
			cross_region.push_back(points);
		}
	}

	//up
	for (int i = 1; i <= thres_distance; i++)
	{
		if ((center.y - i) < 0)
		{
			break;
		}
		Point neighbor(center.x, center.y - i);
		if (judge(center, neighbor))
		{
			vector<Point> points;
			points.push_back(neighbor);


			//right
			for (int j = 1; j <= thres_distance; j++)
			{
				if ((neighbor.x + j) >= img.cols)
				{
					break;
				}
				Point right(neighbor.x + j, neighbor.y);
				if (judge(neighbor, right))
				{
					points.push_back(right);
				}
			}

			//left
			for (int j = 1; j <= thres_distance; j++)
			{
				if ((neighbor.x - j) < 0)
				{
					break;
				}
				Point left(neighbor.x - j, neighbor.y);
				if (judge(neighbor, left))
				{
					points.push_back(left);
				}
			}
			cross_region.push_back(points);
		}
	}

	return cross_region;
}
