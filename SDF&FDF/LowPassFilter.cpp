#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

void log_mag(Mat complex_mat, Mat& dst)
{
	Mat  planes[2];
	split(complex_mat, planes);
	magnitude(planes[0], planes[1], dst);
	log(dst + 1, dst);
	normalize(dst, dst, 0, 255, CV_MINMAX);
	dst.convertTo(dst, CV_8U);
}

void shuffling(Mat mag_img, Mat& dst)
{
	int  cx = mag_img.cols / 2;
	int  cy = mag_img.rows / 2;
	Rect q1(cx, 0, cx, cy);		// 1��и� �簢��
	Rect q2(0, 0, cx, cy);		// 2��и� �簢��
	Rect q3(0, cy, cx, cy);		// 3��и� �簢��
	Rect q4(cx, cy, cx, cy);		// 4��и� �簢��

	dst = Mat(mag_img.size(), mag_img.type());
	mag_img(q1).copyTo(dst(q3));
	mag_img(q3).copyTo(dst(q1));
	mag_img(q2).copyTo(dst(q4));
	mag_img(q4).copyTo(dst(q2));
}

Mat scramble(Mat signal)
{
	Mat dst = signal.clone();
	for (int i = 0, j = 0; i<dst.cols - 1; i++)
	{
		if (i > j) {
			swap(dst.at<Vec2f>(i), dst.at<Vec2f>(j));
		}

		int m = dst.cols >> 1;
		while ((j >= m) && (m >= 2))
		{
			j -= m;
			m >>= 1;
		}
		j += m;
	}
	return dst;
}

Mat  zeropadding(Mat img)
{
	int  m = 1 << (int)ceil(log2(img.rows));
	int  n = 1 << (int)ceil(log2(img.cols));
	Mat dst(m, n, img.type(), Scalar(0));

	Rect rect(Point(0, 0), img.size());
	img.copyTo(dst(rect));
	dst.convertTo(dst, CV_32F);
	return dst;
}

void butterfly(Mat& dst, int dir)
{
	int length = dst.cols;
	int pair = 1;
	for (int k = 0; k< ceil(log2(length)); k++)
	{
		int half_pair = pair;
		pair <<= 1;
		float theta = dir * (-2.0 * CV_PI / pair);
		float wpr = -2.0 * sin(0.5*theta) * sin(0.5*theta);
		float wpi = sin(theta);
		float wre = 1.0;
		float wim = 0.0;

		for (int m = 0; m < half_pair; m++) {
			for (int even = m; even < length; even += pair)
			{
				int odd = even + half_pair;
				Vec2f G_even = dst.at<Vec2f>(even);
				Vec2f G_odd = dst.at<Vec2f>(odd);

				Vec2f G_odd_exp(0, 0);
				G_odd_exp[0] = G_odd[0] * wre - G_odd[1] * wim;
				G_odd_exp[1] = G_odd[1] * wre + G_odd[0] * wim;

				dst.at<Vec2f>(even) = G_even + G_odd_exp;
				dst.at<Vec2f>(odd) = G_even - G_odd_exp;
			}
			float tmp = wre;
			wre += tmp * wpr - wim * wpi;
			wim += wim * wpr + tmp * wpi;
		}
	}
	if (dir == -1)	dst /= dst.cols;
}

void FFT_2D(Mat complex_img, Mat& dst, int dir)
{
	dst = Mat(complex_img.size(), complex_img.type());
	for (int i = 0; i < complex_img.rows; i++)
	{
		Mat scr_sn = scramble(complex_img.row(i));
		butterfly(scr_sn, dir);
		scr_sn.copyTo(dst.row(i));
	}
	transpose(dst, dst);
	for (int i = 0; i < dst.rows; i++) {

		Mat scr_sn = scramble(dst.row(i));
		butterfly(scr_sn, dir);
		scr_sn.copyTo(dst.row(i));
	}

	transpose(dst, dst);
}

// ������ ��� ���� ����
Mat get_lowpassFilter(Size size, int radius)
{
	Point center = size / 2;
	Mat filter(size, CV_32FC2, Vec2f(0, 0));
	circle(filter, center, radius, Vec2f(1, 1), -1);
	return filter;
}

void FFT(Mat image, Mat& dft_coef, Mat& dft_img)  // FFT ��ü ���� ����
{
	Mat complex_img;
	Mat pad_img = zeropadding(image);  // ������
	Mat  tmp[] = { pad_img, Mat::zeros(pad_img.size(), pad_img.type()) };
	merge(tmp, 2, complex_img);	  // ���� ��� ����
	FFT_2D(complex_img, dft_coef, 1);  // FFT ����				
	shuffling(dft_coef, dft_coef);  // ���ø�
	log_mag(dft_coef, dft_img);
}

Mat IFFT(Mat dft_coef, Size size)  // Inverse FFT ��ü ���� ����
{
	Mat idft_coef, idft_img[2];
	shuffling(dft_coef, dft_coef);  // �� ���ø� ����
	FFT_2D(dft_coef, idft_coef, -1);
	split(idft_coef, idft_img);	 // ���� ��� �и�

	Rect img_rect(Point(0, 0), size);
	idft_img[0](img_rect).convertTo(idft_img[0], CV_8U);	// ������ �κ� ����
	return idft_img[0];
}

int main()
{
	Mat image = imread("../image/number4.jpg", 0);
	CV_Assert(image.data);
	Mat dft_coef, dft_img, low_dft, filtered_mat;

	FFT(image, dft_coef, dft_img);  // FFT ���� �� ���ø�
	Mat low_filter = get_lowpassFilter(dft_coef.size(), 70);  // ������ ���� ����

	multiply(dft_coef, low_filter, filtered_mat);  //���Ұ� ����
	log_mag(filtered_mat, low_dft);

	imshow("���� ����", image);
	imshow("FFT + log scale + shuffling ���� ����", dft_img);
	imshow("Low Pass Filter ���� ����", low_dft);  // ���͵� ����Ʈ�� ����	
	imshow("Inverse FFT ���� �� ������ ����", IFFT(filtered_mat, image.size()));	// ����Ʈ�� ���� ����
	waitKey();
	return 0;
}