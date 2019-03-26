#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

void  calc_Histo(const Mat& image, Mat& hist, int bins, int range_max = 256)  //������׷� ��� �Լ�
{
	int		histSize[] = { bins };	// ������׷� ��ް���
	float   range[] = { 0, (float)range_max };	// ������׷� ����
	int		channels[] = { 0 };	 // ä�� ���
	const float* ranges[] = { range };

	calcHist(&image, 1, channels, Mat(), hist, 1, histSize, ranges);
}

void draw_histo(Mat hist, Mat &hist_img, Size size = Size(256, 200))  //������׷� �׷��� �׸��� �Լ�
{
	hist_img = Mat(size, CV_8U, Scalar(255));
	float  bin = (float)hist_img.cols / hist.rows;
	normalize(hist, hist, 0, size.height, NORM_MINMAX);  //������׷� ����ȭ

	for (int i = 0; i < hist.rows; i++)
	{
		float idx1 = i * bin;
		float idx2 = (i + 1) * bin;
		Point2f pt1 = Point2f(idx1, 0);
		Point2f pt2 = Point2f(idx2, hist.at <float>(i));

		if (pt2.y > 0)
			rectangle(hist_img, pt1, pt2, Scalar(0), -1);
	}
	flip(hist_img, hist_img, 0);
}


void create_hist(Mat img, Mat &hist, Mat &hist_img) // ������׷� ����ϰ� �׷����� �׸��� �Լ�
{
	int  histsize = 256, range = 256;
	calc_Histo(img, hist, histsize, range);  // ������׷� ���
	draw_histo(hist, hist_img);  // ������׷� �׷��� �׸���
}

int main()
{
	Mat image = imread("../image/building.jpg", 0);
	CV_Assert(!image.empty());

	Mat hist, dst1, dst2, hist_img, hist_img1, hist_img2;
	create_hist(image, hist, hist_img);  // ������׷� �� �׷��� �׸���

	Mat accum_hist = Mat(hist.size(), hist.type(), Scalar(0));  // ������׷� ������ ���
	accum_hist.at<float>(0) = hist.at<float>(0);

	for (int i = 1; i < hist.rows; i++) {
		accum_hist.at<float>(i) = accum_hist.at<float>(i - 1) + hist.at<float>(i);
	}

	accum_hist /= sum(hist)[0];  //�������� ����ȭ�ϴ� ����
	accum_hist *= 255;
	dst1 = Mat(image.size(), CV_8U);

	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			int idx = image.at<uchar>(i, j);
			dst1.at<uchar>(i, j) = (uchar)accum_hist.at<float>(idx);
		}
	}

	create_hist(dst1, hist, hist_img1);  // ������׷� �� �׷��� �׸���

	imshow("���� ����", image), imshow("������׷� �׷���", hist_img);	// ���� ������׷�
	imshow("������׷� ��Ȱȭ ���� ����", dst1), imshow("������׷� ��Ȱȭ ���� �׷���", hist_img1);

	waitKey();
	return 0;
}