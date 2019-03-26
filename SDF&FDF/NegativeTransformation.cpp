#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

int main()
{
	Mat image = imread("../image/bright.jpg", IMREAD_GRAYSCALE);
	CV_Assert(!image.empty());  //예외 처리

	Mat negative = 255 - image;  //negative transformation

	imshow("원 영상", image);
	imshow("반전 영상", negative);

	waitKey();
	return 0;
}
