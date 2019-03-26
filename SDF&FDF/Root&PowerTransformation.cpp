#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

int main()
{
	Mat image = imread("../image/lion.jpg", 0);
	Mat gamma_img(image.rows, image.cols, image.type());
	double gamma = 4;
	double c = 255 / pow(255, gamma);
	CV_Assert(!image.empty());

	imshow("원본 영상", image);
	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			gamma_img.at<uchar>(i, j) = c * pow(image.at<uchar>(i, j), gamma); // s = c * r ^ k
		}
	}

	imshow("감마 보정 적용 영상", gamma_img);
	waitKey();
	return 0;
}

