#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

int main()
{
	Mat image = imread("../image/wall.jpg", 0);
	Mat invlog_img(image.rows, image.cols, image.type());
	CV_Assert(!image.empty());

	imshow("원본 영상", image);

	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			invlog_img.at<uchar>(i, j) = pow(10, image.at<uchar>(i, j) / (255 / log(256))) - 1; //역로그 변환 (s = 10^(r/c) - 1)
		}
	}

	imshow("inverse log transformation 적용 영상", invlog_img);
	waitKey();
	return 0;
}
