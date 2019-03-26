#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

void  calc_Histo(const Mat& image, Mat& hist, int bins, int range_max = 256)  //히스토그램 계산 함수
{
	int		histSize[] = { bins };	// 히스토그램 계급개수
	float   range[] = { 0, (float)range_max };	// 히스토그램 범위
	int		channels[] = { 0 };	 // 채널 목록
	const float* ranges[] = { range };

	calcHist(&image, 1, channels, Mat(), hist, 1, histSize, ranges);
}

void draw_histo(Mat hist, Mat &hist_img, Size size = Size(256, 200))  //히스토그램 그래프 그리는 함수
{
	hist_img = Mat(size, CV_8U, Scalar(255));
	float  bin = (float)hist_img.cols / hist.rows;
	normalize(hist, hist, 0, size.height, NORM_MINMAX);  //히스토그램 정규화

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


void create_hist(Mat img, Mat &hist, Mat &hist_img) // 히스토그램 계산하고 그래프를 그리는 함수
{
	int  histsize = 256, range = 256;
	calc_Histo(img, hist, histsize, range);  // 히스토그램 계산
	draw_histo(hist, hist_img);  // 히스토그램 그래프 그리기
}

int main()
{
	Mat image = imread("../image/building.jpg", 0);
	CV_Assert(!image.empty());

	Mat hist, dst1, dst2, hist_img, hist_img1, hist_img2;
	create_hist(image, hist, hist_img);  // 히스토그램 및 그래프 그리기

	Mat accum_hist = Mat(hist.size(), hist.type(), Scalar(0));  // 히스토그램 누적합 계산
	accum_hist.at<float>(0) = hist.at<float>(0);

	for (int i = 1; i < hist.rows; i++) {
		accum_hist.at<float>(i) = accum_hist.at<float>(i - 1) + hist.at<float>(i);
	}

	accum_hist /= sum(hist)[0];  //누적합을 정규화하는 과정
	accum_hist *= 255;
	dst1 = Mat(image.size(), CV_8U);

	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			int idx = image.at<uchar>(i, j);
			dst1.at<uchar>(i, j) = (uchar)accum_hist.at<float>(idx);
		}
	}

	create_hist(dst1, hist, hist_img1);  // 히스토그램 및 그래프 그리기

	imshow("원본 영상", image), imshow("히스토그램 그래프", hist_img);	// 원본 히스토그램
	imshow("히스토그램 평활화 적용 영상", dst1), imshow("히스토그램 평활화 적용 그래프", hist_img1);

	waitKey();
	return 0;
}