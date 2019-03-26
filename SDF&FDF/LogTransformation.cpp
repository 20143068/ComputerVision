#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

int main()
{
	Mat image = imread("../image/building.jpg", 0);
	Mat log_img(image.rows, image.cols, image.type());
	CV_Assert(!image.empty());

	imshow("원본 영상", image);

	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			log_img.at<uchar>(i, j) = 255 / log(256) * log(1 + image.at<uchar>(i, j));  //로그 변환 (s = c * log(r + 1))
		}
	}

	imshow("log transformation 적용 영상", log_img);
	waitKey();
	return 0;
}

