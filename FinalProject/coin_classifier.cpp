#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

bool print_menu(); //메뉴 출력 함수
void Preprocessing(Mat img);  //전처리 함수
void Gamma(Mat img);  //감마 보정 함수

int main(void)
{
	cout << "" << endl;
	cout << "		******************************************************" << endl;
	cout << "		*             컴퓨터비전 기말 프로젝트               *" << endl;
	cout << "		*                 20143068 서 준 교                  *" << endl;
	cout << "		******************************************************" << endl;

	string file_name;
	Mat img; 

	while (1) 
	{
		if (!print_menu())  //메뉴 출력
			break;

		cout << "   이미지 파일명을 입력하세요. : ";
		cin >> file_name;

		img = imread(file_name, IMREAD_GRAYSCALE);
		CV_Assert(!img.empty());  //예외처리
		Mat origin_img = img;
		imshow("원본 영상", origin_img);

		Preprocessing(img);  //입력 영상 전처리

		origin_img = imread(file_name, IMREAD_GRAYSCALE);

		//형변환 과정
		IplImage* cvtimg;
		IplImage* org_img;
		IplImage copy1, copy2;  //타입 변환 위해 만든 임시 저장소
		copy1 = img; copy2 = origin_img;
		cvtimg = &copy1; org_img = &copy2;

		CvMemStorage* storage = cvCreateMemStorage(0);  //원의 정보가 담길 공간의 메모리 초기화
		IplImage* result = cvCreateImage(cvGetSize(org_img), IPL_DEPTH_8U, 3);  //실행 결과가 담길 이미지 선언

		CvSeq* seqCircle = cvHoughCircles(cvtimg, storage, CV_HOUGH_GRADIENT, 1, 40, 100, 15, 40, 50);
		//누적기 해상도, 두 원간의 최소 거리, 캐니 최대 경계값, 투표 최소 개수, 최소반지름, 최대반지름
		cvCvtColor(org_img, result, CV_GRAY2BGR);

		vector<Scalar> colorVec = { Scalar(0, 0, 255), Scalar(0, 100, 255), Scalar(0, 255, 255), 
			Scalar(0, 255, 0), Scalar(255, 0, 0), Scalar(155, 51, 51), Scalar(255, 0, 100) };
		vector<String> colorStr = { "빨간", "주황", "노랑", "초록", "파랑", "남", "보라" };
		vector<int> rad;
		for (int i = 0; i < seqCircle->total; i++)
		{
			float* circle;
			int cx, cy, radius;  //원의 중심, 반지름

			circle = (float*)cvGetSeqElem(seqCircle, i);  //i번째 index에 해당하는 원의 정보를 가져온다.

			cx = cvRound(circle[0]);  //반올림
			cy = cvRound(circle[1]);
			radius = cvRound(circle[2]);

			cout << "   " << colorStr[i] << "색 동전의 중심 좌표 : " << "(" << cx << ", "
				<< cy << "), " << "반지름 : " << radius << endl;
			cvCircle(result, cvPoint(cx, cy), radius, colorVec[i], 3, 8, 0);  //boundary 표시
			cvCircle(result, cvPoint(cx, cy), 1, colorVec[i], 2, 8, 0);  //원의 중심 표시
			rad.push_back(radius);
		}

		int min, max;  //반지름의 최대, 최소값
		int ten_cnt = 0; int fifty_cnt = 0; int hundred_cnt = 0;  //각 동전의 개수
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

		cout << "   동전의 개수는 총" << seqCircle->total << "개 입니다." << endl;
		cout << "   10원 동전의 갯수 : " << ten_cnt << endl;
		cout << "   50원 동전의 갯수 : " << fifty_cnt << endl;
		cout << "   100원 동전의 갯수 : " << hundred_cnt << endl;

		cvShowImage("실행 결과", result);
		waitKey(0);
		destroyAllWindows();
		cvReleaseMemStorage(&storage);  //메모리 할당 해제
	}
	return 0;
}

bool print_menu()
{
	int flag;

	cout << "" << endl;
	cout << "   동전 영상에서 서로 다른 동전들의 개수, 위치를 파악하는 프로그램" << endl;
	cout << "   1. 프로그램 실행" << endl;
	cout << "   2. 프로그램 종료" << endl;
	cout << "   1 또는 2를 입력하세요. : ";
	cin >> flag;

	if (flag == 1)
		return true;

	else if (flag == 2)
	{
		cout << "   프로그램을 종료합니다." << endl;
		return false;
	}

	else
	{
		cout << "   잘못된 입력입니다. 프로그램을 다시 실행하세요." << endl;
		return false;
	}
}

void Preprocessing(Mat img) //전처리 함수
{
	medianBlur(img, img, 5);  //salt & pepper 노이즈 제거
	Gamma(img);  //감마 보정 (root transformation)
	equalizeHist(img, img);   //히스토그램 평활화
	threshold(img, img, 50, 255, CV_THRESH_BINARY);  //영상 이진화
	dilate(img, img, Mat(), Point(-1, -1), 5);  //모폴로지 팽창 연산
	erode(img, img, Mat(), Point(-1, -1), 5);  //모폴로지 침식 연산
	GaussianBlur(img, img, Size(5, 5), 3);  //가우시안 잡음 제거
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

