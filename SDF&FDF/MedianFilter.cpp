
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

void medianFilter(Mat img, Mat& dst, int size)
{
	dst = Mat(img.size(), CV_8U, Scalar(0));
	Size msize(size, size);
	Point h_m = msize / 2;

	for (int i = h_m.y; i < img.rows - h_m.y; i++) {
		for (int j = h_m.x; j < img.cols - h_m.x; j++)
		{
			Point start = Point(j, i) - h_m;
			Rect roi(start, msize);
			Mat  mask;
			img(roi).copyTo(mask);

			Mat one_row = mask.reshape(1, 1);
			cv::sort(one_row, one_row, SORT_EVERY_ROW);

			int medi_idx = (int)(one_row.total() / 2);		// 중간 위치 
			dst.at<uchar>(i, j) = one_row.at<uchar>(medi_idx);	// 중간값 출력화소 저장
		}
	}
}

int main()
{
	Mat image = imread("../image/coin.jpg", IMREAD_COLOR);
	CV_Assert(image.data);

	Mat gray, med_img;
	cvtColor(image, gray, CV_BGR2GRAY);
	medianFilter(gray, med_img, 5);							// 사용자 정의 함수

	imshow("원본 영상", gray);
	imshow("Median Filter 적용 영상", med_img);

	waitKey();
	return 0;
}