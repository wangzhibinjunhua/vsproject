#include <opencv2\opencv.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <iostream>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>

using namespace std;
using namespace cv;


void canny_test()
{
	Mat image = imread("1.jpg");
	imshow("test", image);

	Mat dst_img, edge, gray_img;
	dst_img.create(image.size(), image.type());

	cvtColor(image, gray_img, CV_BGR2GRAY);

	blur(gray_img, edge, Size(3, 3));

	Canny(edge, edge, 3, 9, 3);

	imshow("new", edge);
}

void filter_test()
{
	Mat img = imread("1.jpg");
	//namedWindow("均值滤波 原图");
	//namedWindow("均值滤波 效果图");

	imshow("均值滤波 原图", img);

	Mat out;
	//boxFilter(img, out, -1, Size(5, 5));
	//blur(img, out, Size(7, 7));
	GaussianBlur(img, out, Size(3, 3), 0, 0);
	imshow("均值滤波 效果图", out);
}

void dilate_test()
{
	Mat img = imread("1.jpg");
	imshow("均值滤波 原图", img);
	Mat out;
	Mat element = getStructuringElement(MORPH_RECT, Size(15, 15));                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                

	dilate(img, out, element);
	imshow("膨胀效果",out);
}


int main()                  
{
	dilate_test();
	waitKey(0);
	return 0;
}