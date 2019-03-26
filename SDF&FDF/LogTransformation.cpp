#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

int main()
{
	Mat image = imread("../image/building.jpg", 0);
	Mat log_img(image.rows, image.cols, image.type());
	CV_Assert(!image.empty());

	imshow("���� ����", image);

	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			log_img.at<uchar>(i, j) = 255 / log(256) * log(1 + image.at<uchar>(i, j));  //�α� ��ȯ (s = c * log(r + 1))
		}
	}

	imshow("log transformation ���� ����", log_img);
	waitKey();
	return 0;
}

