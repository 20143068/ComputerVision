#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

int main()
{
	Mat image = imread("../image/bright.jpg", IMREAD_GRAYSCALE);
	CV_Assert(!image.empty());  //���� ó��

	Mat negative = 255 - image;  //negative transformation

	imshow("�� ����", image);
	imshow("���� ����", negative);

	waitKey();
	return 0;
}
