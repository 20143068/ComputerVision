#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

Mat getGaussianMask(Size size, double sigmaX, double sigmaY)  // ����þ� ���� ����ũ ����
{
	double ratio = 1 / (sigmaX * sigmaY * CV_PI);
	double sigmaX2 = 2 * sigmaX * sigmaX;
	double sigmaY2 = 2 * sigmaY * sigmaY;

	Point center = size / 2;
	Mat mask(size, CV_64F);

	for (int i = 0; i < size.height; i++) {
		for (int j = 0; j < size.width; j++)
		{
			int x2 = (j - center.x) * (j - center.x);
			int y2 = (i - center.y) * (i - center.y);
			mask.at<double>(i, j) = ratio * exp(-((x2 / sigmaX2) + (y2 / sigmaY2)));  // ����þ� �Լ� ���� ���
		}
	}
	return (mask / sum(mask)[0]);
}

int main()
{
	Mat image = imread("../image/coin1.jpg", 0);
	CV_Assert(image.data);

	Size  size(5, 29);
	double sigmaX = 0.3*((size.width - 1)*0.5 - 1) + 0.8;
	double sigmaY = 0.3*((size.height - 1)*0.5 - 1) + 0.8;

	Mat gauss_img1, gauss_img2;
	Mat gaussian_2d = getGaussianMask(size, sigmaX, sigmaY);  // 2���� ����þ� ����ũ ����
	Mat gaussian_1dX = getGaussianKernel(size.width, -1, CV_64F);  // 1���� ����þ� ��� ����
	Mat gaussian_1dY = getGaussianKernel(size.height, -1, CV_64F);

	filter2D(image, gauss_img1, -1, gaussian_2d);
	GaussianBlur(image, gauss_img2, size, sigmaX, sigmaY);  // ����þ� ���� ����

	imshow("���� ����", image);
	imshow("Gaussian Filtering ���� ����", gauss_img2);

	waitKey();
	return 0;
}