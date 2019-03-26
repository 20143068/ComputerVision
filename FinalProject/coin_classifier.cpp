#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

bool print_menu(); //�޴� ��� �Լ�
void Preprocessing(Mat img);  //��ó�� �Լ�
void Gamma(Mat img);  //���� ���� �Լ�

int main(void)
{
	cout << "" << endl;
	cout << "		******************************************************" << endl;
	cout << "		*             ��ǻ�ͺ��� �⸻ ������Ʈ               *" << endl;
	cout << "		*                 20143068 �� �� ��                  *" << endl;
	cout << "		******************************************************" << endl;

	string file_name;
	Mat img; 

	while (1) 
	{
		if (!print_menu())  //�޴� ���
			break;

		cout << "   �̹��� ���ϸ��� �Է��ϼ���. : ";
		cin >> file_name;

		img = imread(file_name, IMREAD_GRAYSCALE);
		CV_Assert(!img.empty());  //����ó��
		Mat origin_img = img;
		imshow("���� ����", origin_img);

		Preprocessing(img);  //�Է� ���� ��ó��

		origin_img = imread(file_name, IMREAD_GRAYSCALE);

		//����ȯ ����
		IplImage* cvtimg;
		IplImage* org_img;
		IplImage copy1, copy2;  //Ÿ�� ��ȯ ���� ���� �ӽ� �����
		copy1 = img; copy2 = origin_img;
		cvtimg = &copy1; org_img = &copy2;

		CvMemStorage* storage = cvCreateMemStorage(0);  //���� ������ ��� ������ �޸� �ʱ�ȭ
		IplImage* result = cvCreateImage(cvGetSize(org_img), IPL_DEPTH_8U, 3);  //���� ����� ��� �̹��� ����

		CvSeq* seqCircle = cvHoughCircles(cvtimg, storage, CV_HOUGH_GRADIENT, 1, 40, 100, 15, 40, 50);
		//������ �ػ�, �� ������ �ּ� �Ÿ�, ĳ�� �ִ� ��谪, ��ǥ �ּ� ����, �ּҹ�����, �ִ������
		cvCvtColor(org_img, result, CV_GRAY2BGR);

		vector<Scalar> colorVec = { Scalar(0, 0, 255), Scalar(0, 100, 255), Scalar(0, 255, 255), 
			Scalar(0, 255, 0), Scalar(255, 0, 0), Scalar(155, 51, 51), Scalar(255, 0, 100) };
		vector<String> colorStr = { "����", "��Ȳ", "���", "�ʷ�", "�Ķ�", "��", "����" };
		vector<int> rad;
		for (int i = 0; i < seqCircle->total; i++)
		{
			float* circle;
			int cx, cy, radius;  //���� �߽�, ������

			circle = (float*)cvGetSeqElem(seqCircle, i);  //i��° index�� �ش��ϴ� ���� ������ �����´�.

			cx = cvRound(circle[0]);  //�ݿø�
			cy = cvRound(circle[1]);
			radius = cvRound(circle[2]);

			cout << "   " << colorStr[i] << "�� ������ �߽� ��ǥ : " << "(" << cx << ", "
				<< cy << "), " << "������ : " << radius << endl;
			cvCircle(result, cvPoint(cx, cy), radius, colorVec[i], 3, 8, 0);  //boundary ǥ��
			cvCircle(result, cvPoint(cx, cy), 1, colorVec[i], 2, 8, 0);  //���� �߽� ǥ��
			rad.push_back(radius);
		}

		int min, max;  //�������� �ִ�, �ּҰ�
		int ten_cnt = 0; int fifty_cnt = 0; int hundred_cnt = 0;  //�� ������ ����
		int size = rad.size();

		sort(rad.begin(), rad.end());
		min = rad[0]; max = rad[size - 1];
		
		for (int i = 0; i < rad.size(); i++) 
		{
			if (min == rad[i] && rad[i] <= min + 1)
				fifty_cnt++;
			else if (max - 1 <= rad[i] && rad[i] <= max)
				hundred_cnt++;
			else
				ten_cnt++;
		}

		cout << "   ������ ������ ��" << seqCircle->total << "�� �Դϴ�." << endl;
		cout << "   10�� ������ ���� : " << ten_cnt << endl;
		cout << "   50�� ������ ���� : " << fifty_cnt << endl;
		cout << "   100�� ������ ���� : " << hundred_cnt << endl;

		cvShowImage("���� ���", result);
		waitKey(0);
		destroyAllWindows();
		cvReleaseMemStorage(&storage);  //�޸� �Ҵ� ����
	}
	return 0;
}

bool print_menu()
{
	int flag;

	cout << "" << endl;
	cout << "   ���� ���󿡼� ���� �ٸ� �������� ����, ��ġ�� �ľ��ϴ� ���α׷�" << endl;
	cout << "   1. ���α׷� ����" << endl;
	cout << "   2. ���α׷� ����" << endl;
	cout << "   1 �Ǵ� 2�� �Է��ϼ���. : ";
	cin >> flag;

	if (flag == 1)
		return true;

	else if (flag == 2)
	{
		cout << "   ���α׷��� �����մϴ�." << endl;
		return false;
	}

	else
	{
		cout << "   �߸��� �Է��Դϴ�. ���α׷��� �ٽ� �����ϼ���." << endl;
		return false;
	}
}

void Preprocessing(Mat img) //��ó�� �Լ�
{
	medianBlur(img, img, 5);  //salt & pepper ������ ����
	Gamma(img);  //���� ���� (root transformation)
	equalizeHist(img, img);   //������׷� ��Ȱȭ
	threshold(img, img, 50, 255, CV_THRESH_BINARY);  //���� ����ȭ
	dilate(img, img, Mat(), Point(-1, -1), 5);  //�������� ��â ����
	erode(img, img, Mat(), Point(-1, -1), 5);  //�������� ħ�� ����
	GaussianBlur(img, img, Size(5, 5), 3);  //����þ� ���� ����
}

void Gamma(Mat img)
{
	double gamma = 2;
	double c = 255 / pow(255, gamma);
	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			img.at<uchar>(i, j) = c * pow(img.at<uchar>(i, j), gamma); // s = c * r ^ k
		}
	}
}

