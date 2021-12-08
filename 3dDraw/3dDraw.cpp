//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2/calib3d.hpp>
//#include <opencv2/opencv.hpp>
#include <iostream>
#include <time.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>
#include <fstream>
//#include "json/json.h"
#include "parseJson.h"
#include "3dDraw.h"
#include "drawing.h"

#pragma comment(lib, "jsoncpp.lib")

#define FILE_NAME "1/000100.jpg"	

using namespace cv;
using namespace std;

string pts_path = "1.pts";		//pts file

//double wpointX[4] = {};			
//double wpointY[4] = {};
//double pointX[19][4] = { {},{},{},{} };		// dsc 개수만큼 배열 생성
//double pointY[19][4] = { {},{},{},{} };
int num = 19;
Mat h[19] = {};					//Homograpy Matrix	(3d -> 2d)		
Mat h_T[19] = {};				//Homograpy Matrix	(2d -> 3d)
Point p1, p2, p3, wp1, wp2, wp3, wt1, wt2, wt3;	// triangle point

double wX[4] = {};
double wY[4] = {};
double pX[19][4] = { {},{},{},{} };		// dsc 개수만큼 배열 생성
double pY[19][4] = { {},{},{},{} };

//int num = 19;
//string dsc[19] = {};
String path("1/*.jpg");
vector<String> str;

//Mat flag = imread("korea.png");
//Mat inner = imread("tae300.png");
Mat frame;
int fps = 30;
Mat warp;		//그려질 평면 
//Mat img = imread("000126.jpg");

//Mat img;
//VideoCapture cap(FILE_NAME);
//int fps = (int)cap.get(CAP_PROP_FPS); 
String SetWindow = "Settings";

class Coord			// point (x,y)
{

public:
	int x, y;
	void SetPoint(int a, int b);
	void GetPoint(int a, int b);

	//Coord(int x, int y);
};

void Coord::SetPoint(int a, int b)
{
	x = a;
	y = b;
}

vector<Point> pts[10];
vector<Point2d> ptsList(10);
//vector<Coord> pts[2];

enum Colors
{
	Custom,
	Red,
	Orange,
	Yellow,
	Green,
	Blue,
	Cyan,
	Black
};

//enum Shapes
//{
//	Circle,
//	Arrow,
//	Triangle,
//	line,
//	Polygon
//};

int alpha = 0;

class Shape
{
public:
	Scalar rgb;
	int color;
	int alpha_max;
	int alpha;

	void SetColor(int color_c, Scalar* color);
	void Appear() { alpha += alpha_max / 15; }
	void Disppear() { alpha -= alpha_max / 15; }

};

class Arrow :public Shape
{

private:
	double angle;

public:
	int StartX, StartY, EndX, EndY;

	vector<Point> start, end;
	vector<Point> w_start, w_end;

	int line_type;
	int thick;
	vector<string> type{ "solid","dotted" };
	
	void Draw(Mat plane, double dx, double dy);
	void CalcAngle(double* angle);
	//void CalcAngle(Arrow arr, double* angle);
	void Drawing(double* dx, double* dy, int i);
	//Arrow();

};

class Circle :public Shape {
public:
	Point center;

	int circle_type;
	int thick;
	double r;
	int fill;
	Scalar color2;

	void Draw(Mat dst);
	vector<Point> p;

};

class Triangle :public Shape {

public:

	int fill;
	int thick;
	vector<Point> p;
	vector<Point> wp;

	void Draw(Mat plane);

};


vector<Point> Tripoint(Point p1, Point p2, Point p3)
{
	vector<Point> triangle;
	triangle.push_back(p1);
	triangle.push_back(p2);
	triangle.push_back(p3);
	return triangle;
}





class Animation {

};



void showImg(string filename, Mat img, float factor);
//void drawCircle(Mat plane, Circle c);
//void drawLine(Mat plane, double x, double y, double dx, double dy, Arrow arr);
//void drawPlane(Mat dst, Triangle t);
void onMouse(int evt, int x, int y, int flags, void* param);
void calcHomograpy(double x, double y, double* dx, double* dy);
void calcHomograpy1(Point point, Point* h_point);
void calcHomograpy_T(double x, double y, double* dx, double* dy);
void findHomo(int n);

//pts_3d 
//Point p1(181, 1800);
//Point p2(1949, 547);
//Point p3(3715, 666);
//Point p4(2905, 2060);
//double pointX[5][4] = { { 157,842,1330,493 },{ 248,946,1363,473 },{ 360,1056,1395,455 },{ 962,1622,1450,544 },  { 1390,292,1323,703 } };
//double pointY[5][4] = { { 371,257,493,690 } ,{ 339,250,506,670 } ,{ 309,251,528,642 } ,{ 246,340,664,506 },{ 292,426,755,451 } }
////000126.jpg
//double pointX[4] = { 157,842,1330,493 }, pointY[4] = { 371,257,493,690 };
//double pointX[4] = { 457,1243,1395,569 }, pointY[4] = { 638,459,534,745 };
//000128.jpg
//double pointX[4] = { 248,946,1363,473 }, pointY[4] = { 339,250,506,670};
//
////000130.jpg
//double pointX[4] = { 360,1056,1395,455 }, pointY[4] = { 309,251,528,642 };
//
////000243.jpg
//double pointX[4] = { 962,1622,1450,544 }, pointY[4] = { 246,340,664,506 };
//
////000250.jpg
//double pointX[4] = { 1390,292,1323,703 }, pointY[4] = { 292,426,755,451 };
//double pointX[4] = { 814,1414,1201,614 }, pointY[4] = { 432,707,785,479 };
//013055_3.jpg
//double pointX[4] = { 181,1949,3715,2905 }, pointY[4] = { 1800,547,666,2060 };					//pts_3d
//double wpointX[4] = { 202,598,599,202 }, wpointY[4] = { 601,602,763,762 };					//world_coords
////k0098_4.png
//double pointX[4] = { 609,2982,3823,280 }, pointY[4] = { 1038,1010,1256,1317 };				//pts_3d
//double wpointX[4] = { 763,39,40,762 }, wpointY[4] = { 764,763,223,222 };						//world_coords
//pts_3d
//double wpointX[4] = { 214,583,620,165 }, wpointY[4] = { 129,127,561,562 };						//world_coords
//double wpointX[4] = { 175,611,612,175 }, wpointY[4] = { 519,518,605,605 };	//좁은영역

double h11, h12, h13, h21, h22, h23, h31, h32;		//homogeneous matrix의 요소를 담는 변수
double th11, th12, th13, th21, th22, th23, th31, th32;
double arrStartX, arrStartY, arrEndX, arrEndY;
double rx, ry;			//circle 센터로부터 arr start,end point 계산할때 사용되는 변수

int fx, fy;				//첫번째 원의 point 
int fx2, fy2;			//두번째 원의 point
int temp_x, temp_y;
double triEx, triEy;		//삼각형 끝 포인트
double trip1x, trip1y, trip2x, trip2y;	//삼각형 나머지 두 포인트

struct transP {			//homogeneous matrix를 적용하여 계산한 포인트가 담기는 변수(warp point)
	double hx;
	double hy;
};

struct warpPoint
{
	double x, y;
	vector<Point2d> p;
};

struct planePoint {
	Point p[3];
};

int rad = 27;
double auto_r;
int rad1, rad2;
//int r_auto;

int max_a;			// transparency (range: 0~255)
int thick = 4;
int spin = 0;		//spin circle parameter
//double angle;		//circle1과 circle2간의 각도
//double angle_n;

//color
int r = 123, g = 220, b = 220;
int color_c1 = 2;
int color_c2 = 3;
int color_t = 3;
int color_l = 2;
Scalar rgb;
Scalar rgb1;
Scalar rgb2;
//객체별로 수정 필요

//Scalar red(200, 200, 20);
//Scalar orange(alpha, 50, 20);
//Scalar yellow(255, 0, 0);
//Scalar green(alpha, 40, alpha);
//Scalar blue(30, 90, alpha);
//Scalar white(30, 30, 30);
//Scalar black(100, 100, 100);
//double scale = 1;			//roi size
float scale_view = 1;		//image scale

//line type
int line_type = 0;			// solid or dotted
int circle_type = 3;		// ring, spin, dotted
//int arrow_tip = 7;			//if tip = 0 , result is line

double lineEndX, lineEndY;
double len;

class eventclass
{
public:
	void HandleEvent(int evt, int x, int y, int flags);
};

struct transP f1, f2;		//circle point calc h_matrix
struct transP t1, t2, t3;	//
struct transP as, ae;		//arrow start,end trans point
struct transP has, hae;		//arrow homo, start end warp point
struct warpPoint warp1, warp2, warp3;	// triangle point

parseJson parse;
//double* pointX = (double*)malloc(sizeof(int*) * size_i);
//void parsing_json() {
//
//	/*double** pointX;
//	pointX = new double* [size_i];
//	for (int i = 0; i < size_i; i++)
//	{
//		pointX[i] = new double[4];
//	}
//	for (int i = 0; i < size_i; i++)
//	{
//		for (int j = 0; j < 4; j++)
//		{
//			pointX[i][j] = { };
//		}
//	}*/
//
//	ifstream json_file;
//	json_file.open(pts_path);
//
//	Json::Value root;
//	json_file >> root;
//
//
//	//cout<<root["points"].asString()<<endl;
//   // cout << root["world_coords"] << endl;
//
//	Json::Value world_pts = root["world_coords"];
//	wpointX[0] = world_pts["X1"].asDouble();
//	wpointX[1] = world_pts["X2"].asDouble();
//	wpointX[2] = world_pts["X3"].asDouble();
//	wpointX[3] = world_pts["X4"].asDouble();
//	wpointY[0] = world_pts["Y1"].asDouble();
//	wpointY[1] = world_pts["Y2"].asDouble();
//	wpointY[2] = world_pts["Y3"].asDouble();
//	wpointY[3] = world_pts["Y4"].asDouble();
//
//	/*for (int i = 0; i < 4; i++)
//	{
//		cout << "world X" << i + 1 << " " << wpointX[i] << endl;
//		cout << "world Y" << i + 1 << " " << wpointY[i] << endl;
//	}*/
//
//
//	Json::Value pts = root["points"];
//	for (int i = 0; i < pts.size(); i++)
//	{
//		pointX[i][0] = pts[i]["pts_3d"]["X1"].asDouble();
//		pointX[i][1] = pts[i]["pts_3d"]["X2"].asDouble();
//		pointX[i][2] = pts[i]["pts_3d"]["X3"].asDouble();
//		pointX[i][3] = pts[i]["pts_3d"]["X4"].asDouble();
//
//		pointY[i][0] = pts[i]["pts_3d"]["Y1"].asDouble();
//		pointY[i][1] = pts[i]["pts_3d"]["Y2"].asDouble();
//		pointY[i][2] = pts[i]["pts_3d"]["Y3"].asDouble();
//		pointY[i][3] = pts[i]["pts_3d"]["Y4"].asDouble();
//
//	}
//}



int key = waitKey();
Coord p[10];
//Coord p1, p2;
int cnt = 1;
Arrow arr1, arr2;
Circle cir1, cir2;
Triangle tri;


int time_ani;

void Shape::SetColor(int color_c, Scalar* color) {

	Scalar rgb;

	switch (color_c)
	{
	case 0:
		//rgb = Scalar((255 - b) * alpha / alpha_max, (255 - g) * alpha / alpha_max, (255 - r) * alpha / alpha_max);				//custom
		break;
	case 1:
		rgb = Scalar(alpha, alpha, 0);		//red
		break;
	case 2:
		rgb = Scalar(alpha, alpha / 2, 0);		//orange
		break;
	case 3:
		rgb = Scalar(alpha, 0, 0);			//yellow
		break;
	case 4:
		rgb = Scalar(alpha, 0, alpha);		//green
		break;
	case 5:
		rgb = Scalar(0, alpha, alpha);		//blue
		break;
	case 6:
		rgb = Scalar(0, 0, alpha);			//cyan
		break;
	case 7:
		rgb = Scalar(alpha, alpha, alpha);	//black
		break;
	}

	*color = rgb;
}




int main(int ac, char** av)
{

	Mat img = imread("1/000100.jpg");
	//Mat img = imread("1/000136.jpg");
	Mat p_img;
	clock_t start, end, start_1, end_1, start_2, end_2, start_3, end_3;

	Mat plane(img.size(), img.type(), Scalar(50, 130, 55));
	//Mat plane(hei * 2, wid * 2, img.type(), Scalar(50, 130, 55));	//ROI
	Mat draw = Mat::zeros(plane.size(), plane.type());		//2d draw
	Mat draw_n;
	Mat drawimg = Mat::zeros(img.size(), img.type());
	warp = Mat::zeros(img.size(), img.type());
	Mat result = Mat::zeros(img.size(), img.type());
	Mat roi, img_warp, frame_end, frame_start;
	//Mat flag_mini, flag_warp;
	//Mat inner_mini, inner_warp; 

	int t = 1; //time second 


	glob(path, str, false);

	if (str.size() == 0)
		cout << "image non exist. \n" << endl;

	//parsing_json();
	/*if (!cap.isOpened())
	{
		printf("Can't open the camera");
		return -1;
	}*/
	//cap >> img;
	parse.parsing_json();

	for (int i = 0; i < 4; i++) {
		wX[i] = parse.wpointX[i];
		wY[i] = parse.wpointY[i];
	}
	double wid = (abs(wX[1] - wX[0]) + abs(wX[2] - wX[3])) * 0.5;
	double hei = (abs(wY[2] - wY[1]) + abs(wY[3] - wY[0])) * 0.5;

	/*double wid = (abs(wpointX[1] - wpointX[0]) + abs(wpointX[2] - wpointX[3])) * 0.5;
	double hei = (abs(wpointY[2] - wpointY[1]) + abs(wpointY[3] - wpointY[0])) * 0.5;*/
	rad1 = (500 * wid) / 13500;
	rad2 = (500 * hei) / 3300;
	rad = rad1 + rad2;


	namedWindow(SetWindow, WINDOW_AUTOSIZE);
	createTrackbar("x", SetWindow, &fx, wid);
	createTrackbar("y", SetWindow, &fy, hei);
	createTrackbar("radius", SetWindow, &rad, 100);
	createTrackbar("C1 Color", SetWindow, &color_c1, 7);
	createTrackbar("C2 Color", SetWindow, &color_c2, 7);
	createTrackbar("Line Color", SetWindow, &color_l, 7);
	createTrackbar("R", SetWindow, &r, 255);
	createTrackbar("G", SetWindow, &g, 255);
	createTrackbar("B", SetWindow, &b, 255);
	createTrackbar("Transparency", SetWindow, &alpha, 255);
	createTrackbar("Thickness", SetWindow, &thick, 20);
	setTrackbarMin("Thickness", SetWindow, 1);
	createTrackbar("Line/Arrow/null", SetWindow, &line_type, 2);
	//createTrackbar("Arrow tip", SetWindow, &arrow_tip, 10);
	createTrackbar("Circle/two color/Fill/null", SetWindow, &circle_type, 4);
	//setWindowProperty(SetWindow, WND_PROP_TOPMOST, 1);


	//resize(flag, flag_mini, flag.size() / 4, 0, 0, cv::INTER_AREA);
	//cvtColor(inner, inner, COLOR_BGR2RGB);
	//resize(inner, inner_mini,Size(2*rad-thick,2*rad-thick), 0, 0, cv::INTER_AREA);

	//imshow("korea test", flag_mini);
	//warpPerspective(img, img_warp, h[0], img.size());
	//imshow("warp img", img_warp);


	cout << "str " << str.size() << endl;
	findHomo(0);
	//findHomo(str.size()-1);

	wp1.x = warp1.x;
	wp1.y = warp1.y;
	wp2.x = warp2.x;
	wp2.y = warp2.y;
	wp3.x = warp3.x;
	wp3.y = warp3.y;

	//coord test
	int cnt = 0;





	cout << "===============Select Mode==============" << endl << endl;
	cout << "1. Circle and Arrow (input 2 points) " << endl;
	cout << "2. 2 Circle , 1 Arrow (input 4 points) " << endl;
	cout << "3. 2 Circle , 1 Triangle (input 5 points) " << endl;
	int mode;
	//cin >> mode;
	mode = 3;




	while (1) {

		cout << "Click two points for Circle center" << endl << endl;

		showImg("Click two Points", img, scale_view);
		setMouseCallback("Click two Points", onMouse);
		waitKey(0);


		findHomo(0);
		calcHomograpy(fx, fy, &f1.hx, &f1.hy);
		calcHomograpy(fx2, fy2, &f2.hx, &f2.hy);

		calcHomograpy(trip1x, trip1y, &t1.hx, &t1.hy);
		calcHomograpy(trip2x, trip2y, &t2.hx, &t2.hy);
		wt1.x = t1.hx;
		wt1.y = t1.hy;
		wt2.x = t2.hx;
		wt2.y = t2.hy;

		while (p[0].x != fx)
			p[0].SetPoint(fx, fy);
		//pts[0].push_back(Point2d(fx, fy));
		while (p[1].x != fx2)
			//pts[0].push_back(Point2d(fx2, fy2));
			p[1].SetPoint(fx2, fy2);

		//cout << "Point 1 : (" << p[0].x << "," << p[0].y << ")" << endl;
		//cout << "Point 2 : (" << p[1].x << "," << p[1].y << ")" << endl;


		struct transP arr1_s, arr1_e;

		findHomo(0);
		calcHomograpy(p[0].x, p[0].y, &arr1_s.hx, &arr1_s.hy);
		calcHomograpy(p[1].x, p[1].y, &arr1_e.hx, &arr1_e.hy);
		//calcHomograpy(pts[0], &arr1_s);
		//calcHomograpy(pts[0][0],pts[0][1], &pts[1]);





		//circle1
		cir1.circle_type = circle_type;
		cir1.color = color_c1;
		cir1.color2 = Scalar(24, 56, 111);
		cir1.fill = 1;
		cir1.r = rad;
		cir1.thick = cir1.r / 2;
		cout << "thick " << cir1.thick << endl;
		cir1.center = Point(f1.hx, f1.hy);
		cir1.alpha_max = 70 * (255 / 100);

		//circle2
		cir2.circle_type = circle_type;
		cir2.color = color_c2;
		cir2.color2 = Scalar(127, 176, 11);
		cir2.fill = 1;
		cir2.r = rad;
		cir2.thick = cir2.r / 2;
		if (cir2.center == Point(0, 0))
			cir2.center = Point(f2.hx, f2.hy);
		cir2.alpha_max = 70 * (255.0 / 100.0);


		//triangle
		p1 = Point(622, 804), p2 = Point(1278, 513), p3 = Point(1261, 500);
		tri.alpha_max = 70 * (255.0 / 100.0);
		tri.color = color_l;
		tri.fill = 0;
		tri.thick = 5;

		//Tripoint(p1, p2, p3);

		//tri.p = Tripoint(p1, p2, p3);
		cout << "p: " << tri.p << endl;

		tri.p = { p1,p2,p3 };
		calcHomograpy(p1.x, p1.y, &warp1.x, &warp1.y);
		calcHomograpy(p2.x, p2.y, &warp2.x, &warp2.y);
		calcHomograpy(p3.x, p3.y, &warp3.x, &warp3.y);

		tri.wp = { Point(warp1.x, warp1.y), Point(warp2.x, warp2.y), Point(warp3.x,warp3.y) };


		// arrow 1
		arr1.color = color_l;
		arr1.thick = rad / 3.0;
		arr1.StartX = ceil(arr1_s.hx);
		arr1.StartY = ceil(arr1_s.hy);
		arr1.EndX = arr1_e.hx;
		arr1.EndY = arr1_e.hy;
		arr1.line_type = 0;
		arr1.alpha_max = 40 * (255.0 / 100.0);


		//arrow2
		arr2.color = color_l;
		arr2.thick = rad / 3.0;
		arr2.StartX = f2.hx;
		arr2.StartY = f2.hy;
		arr2.EndX = warp2.x;
		arr2.EndY = warp2.y;
		arr2.line_type = line_type;
		arr2.alpha_max = 30 * (255.0 / 100.0);


		//Set value & initialize
		max_a = 90;
		frame_start = imread(str[0]);
		alpha = 0;
		spin = 0;

		/*if (img.empty())
		{
			printf("empty image");
			return 0;
		}*/
		//circle point 정의


		//angle calcurate (for arrow tip and dotted line)
		/*arr1.CalcAngle(arr1, &arr1.angle);
		arr2.CalcAngle(arr2, &arr2.angle);*/
		//arr1.CalcAngle(&arr1.angle);
		//arr2.CalcAngle(&arr2.angle);

		//if (arr1.StartX == arr1.EndX)
		//{
		//	rx = 0;
		//	if (arr1.EndY > arr1.StartY)
		//		angle = 
		//		ry = rad;
		//	else
		//		ry = -rad;
		//}
		//else {
		//	angle = atan(double(abs(arr1.EndY - arr1.StartY)) / double(abs(arr1.EndX - arr1.StartX)));
		//	if (arr1.EndX > arr1.StartX && arr1.EndY >= arr1.StartY)
		//	{

		//	}
		//	else if (arr1.StartX > arr1.EndX && arr1.EndY >= arr1.StartY)
		//	{
		//		angle = CV_PI -angle;
		//	}
		//	else if (arr1.StartX > arr1.EndX && arr1.StartY > arr1.EndY)
		//	{
		//		angle = CV_PI + angle;
		//	}
		//	else if (arr1.EndX > arr1.StartX && arr1.StartY > arr1.EndY)
		//	{
		//		angle = 2 * CV_PI -angle;
		//	}
		//	rx = rad * cos(angle);
		//	ry = rad * sin(angle);
		//}




		calcHomograpy(triEx, triEy, &t1.hx, &t1.hy);
		calcHomograpy(trip1x, trip1y, &t2.hx, &t2.hy);
		calcHomograpy(trip2x, trip2y, &t3.hx, &t3.hy);


		int draw_start = 50;
		time_ani = 15;

		switch (mode) {

		case 1:	//1. Circle and Arrow 
			//1. Drawing Appear
			for (int i = 0; i < time_ani; i++)
			{
				cir1.Appear();
				cir2.Appear();
				arr1.Appear();

				spin += 360 / time_ani;
				//auto_r = ((-192.0 * rad) / (245 * pow((fps / 2), 2))) * pow(i - (7 / 8) * (fps / 2), 2) + (11 / 10) * rad;
				//setMouseCallback("Result", onMouse);

				draw = Mat::zeros(img.size(), img.type());
				warp = Mat::zeros(img.size(), img.type());

				cir1.Draw(draw);
				cir2.Draw(draw);
				//drawCircle(draw, cir1);
				//drawCircle(draw, cir2);
				blur(draw, draw, Size(2, 2));

				moveWindow("draw", 2200, 10);
				showImg("draw", draw, scale_view);
				warpPerspective(draw, warp, h_T[0], img.size());


				subtract(frame_start, warp, result);
				setMouseCallback(SetWindow, onMouse);

				imwrite("result/result1_" + to_string(i) + "_appear.jpg", result);
				showImg("Result", result, scale_view);

				waitKey(1);

			}

			waitKey(1);


			//2. Arrow Appear
			for (int i = 0; i < time_ani; i++)
			{

				spin += 360 / time_ani;
				setMouseCallback("Result", onMouse);

				draw = Mat::zeros(img.size(), img.type());
				warp = Mat::zeros(img.size(), img.type());

				double dx, dy;
				//double aniStartX, aniStartY, aniEndX, aniEndY;

				arr1.Drawing(&dx, &dy, i);


				cir1.Draw(draw);
				cir2.Draw(draw);
				arr1.Draw(draw, dx, dy);
				blur(draw, draw, Size(2, 2));

				showImg("draw", draw, scale_view);
				warpPerspective(draw, warp, h_T[0], img.size());


				subtract(frame_start, warp, result);


				setMouseCallback(SetWindow, onMouse);

				imwrite("result/result2_" + to_string(i) + "_arrow.jpg", result);
				showImg("Result", result, scale_view);
				waitKey(1);


			}

			spin = 0;
			// 3. frame swipe
			for (int cnt = 0; cnt < str.size() - 2; cnt++) {

				p_img = imread(str[cnt]);
				findHomo(cnt);

				//spin += 5;
				//spin += 360 / fps * 2;
				// setMouseCallback("Result", onMouse);

				draw = Mat::zeros(img.size(), img.type());
				warp = Mat::zeros(img.size(), img.type());

				cir1.Draw(draw);
				cir2.Draw(draw);
				arr1.Draw(draw, arr1.EndX, arr1.EndY);
				blur(draw, draw, Size(2, 2));

				warpPerspective(draw, warp, h_T[cnt], img.size());


				subtract(p_img, warp, result);
				setMouseCallback(SetWindow, onMouse);

				imwrite("result/result3_" + to_string(cnt) + "_swipe.jpg", result);
				showImg("Result", result, scale_view);

				waitKey(1);

			}

			//4. Drawing Disappear
			frame_end = imread(str[str.size() - 1]);

			findHomo(str.size() - 1);
			for (int i = 0; i < time_ani; i++)
			{
				cir1.Disppear();
				cir2.Disppear();
				arr1.Disppear();


				setMouseCallback("Result", onMouse);

				draw = Mat::zeros(img.size(), img.type());
				warp = Mat::zeros(img.size(), img.type());

				cir1.Draw(draw);
				cir2.Draw(draw);
				arr1.Draw(draw, arr1.EndX, arr1.EndY);

				blur(draw, draw, Size(2, 2));

				warpPerspective(draw, warp, h_T[str.size() - 1], img.size());


				subtract(frame_end, warp, result);
				setMouseCallback(SetWindow, onMouse);

				imwrite("result/result4_" + to_string(i) + "_disappear.jpg", result);
				showImg("Result", result, scale_view);
				waitKey(1);

			}
			break;

		case 2:	// 2. Circle , 2 Arrow (input 4 points) 
			//1. Drawing Appear
			double dx, dy;
			for (int i = 0; i < time_ani; i++)
			{
				cir1.Appear();
				cir2.Appear();
				arr1.Appear();
				spin += 360 / time_ani;


				arr1.Drawing(&dx, &dy, i);
				//auto_r = ((-192.0 * rad) / (245 * pow((fps / 2), 2))) * pow(i - (7 / 8) * (fps / 2), 2) + (11 / 10) * rad;
				//setMouseCallback("Result", onMouse);

				draw = Mat::zeros(img.size(), img.type());
				warp = Mat::zeros(img.size(), img.type());

				cir1.Draw(draw);
				cir2.Draw(draw);
				//drawCircle(draw, cir1);
				//drawCircle(draw, cir2);
				arr1.Draw(draw,dx,dy);
				//drawLine(draw, arr1.StartX, arr1.StartY, dx, dy, arr1);
				blur(draw, draw, Size(2, 2));

				moveWindow("draw", 2200, 10);
				showImg("draw", draw, scale_view);
				warpPerspective(draw, warp, h_T[0], img.size());

				subtract(frame_start, warp, result);
				setMouseCallback(SetWindow, onMouse);

				imwrite("result/result1_" + to_string(i) + "_appear.jpg", result);
				showImg("Result", result, scale_view);

				waitKey(1);

			}

			waitKey(1);



			spin = 0;
			// 2. frame swipe
			for (int cnt = 0; cnt < str.size() - 2; cnt++) {

				p_img = imread(str[cnt]);
				findHomo(cnt);

				//spin += 5;
				//spin += 360 / fps * 2;
				// setMouseCallback("Result", onMouse);

				draw = Mat::zeros(img.size(), img.type());
				warp = Mat::zeros(img.size(), img.type());

				cir1.Draw(draw);
				cir2.Draw(draw);
				arr1.Draw(draw, arr1.EndX, arr1.EndY);
				//drawLine(draw, arr1.StartX, arr1.StartY, arr1.EndX, arr1.EndY, arr1);
				blur(draw, draw, Size(2, 2));

				warpPerspective(draw, warp, h_T[cnt], img.size());


				subtract(p_img, warp, result);
				setMouseCallback(SetWindow, onMouse);

				imwrite("result/result3_" + to_string(cnt) + "_swipe.jpg", result);
				showImg("Result", result, scale_view);

				waitKey(1);

			}

			frame_end = imread(str[str.size() - 1]);

			findHomo(str.size() - 1);
			//calcHomograpy(p2.x, p2.y, &warp2.x, &warp2.y);

			//3. Arrow2 Appear
			double dx2, dy2;
			//arr2.alpha = arr2.alpha_max;
		
			for (int i = 0; i < time_ani; i++)
			{
				arr2.Appear();
				
				spin += 360 / time_ani;
				setMouseCallback("Result", onMouse);

				draw = Mat::zeros(img.size(), img.type());
				warp = Mat::zeros(img.size(), img.type());


				cir1.Draw(draw);
				cir2.Draw(draw);
				arr1.Draw(draw, arr1.EndX, arr1.EndY);
				//drawLine(draw, arr1.StartX, arr1.StartY, arr1.EndX, arr1.EndY, arr1);

				arr2.Drawing(&dx2, &dy2, i);
				arr2.Draw(draw, dx2, dy2);
				//drawLine(draw, arr2.StartX, arr2.StartY, dx2, dy2, arr2);

				blur(draw, draw, Size(2, 2));

				showImg("draw", draw, scale_view);
				warpPerspective(draw, warp, h_T[str.size() - 1], img.size());


				subtract(frame_end, warp, result);


				setMouseCallback(SetWindow, onMouse);

				imwrite("result/result2_" + to_string(i) + "_arrow.jpg", result);
				showImg("Result", result, scale_view);
				waitKey(1);


			}


			//4. Drawing Disappear
			frame_end = imread(str[str.size() - 1]);

			findHomo(str.size() - 1);
			for (int i = 0; i < time_ani; i++)
			{
				
				cir1.Disppear();
				cir2.Disppear();
				arr1.Disppear();
				arr2.Disppear();
		

				setMouseCallback("Result", onMouse);

				draw = Mat::zeros(img.size(), img.type());
				warp = Mat::zeros(img.size(), img.type());

				cir1.Draw(draw);
				cir2.Draw(draw);
				arr1.Draw(draw, arr1.EndX, arr1.EndY);
				arr2.Draw(draw, arr2.EndX, arr2.EndY);
				//drawLine(draw, arr1.StartX, arr1.StartY, arr1.EndX, arr1.EndY, arr1);
				//drawLine(draw, arr2.StartX, arr2.StartY, arr2.EndX, arr2.EndY, arr2);

				blur(draw, draw, Size(2, 2));

				warpPerspective(draw, warp, h_T[str.size() - 1], img.size());


				subtract(frame_end, warp, result);
				setMouseCallback(SetWindow, onMouse);

				imwrite("result/result4_" + to_string(i) + "_disappear.jpg", result);
				showImg("Result", result, scale_view);
				waitKey(1);

			}



			break;

		case 3:	// 3. 2 Circle , 1 Triangle (input 5 points)
				//1. Drawing Appear
			for (int i = 0; i < time_ani; i++)
			{
				cir1.Appear();
				cir2.Appear();
				cout << "alpha " << cir1.alpha << endl;
				spin += 360 / time_ani;
				//auto_r = ((-192.0 * rad) / (245 * pow((fps / 2), 2))) * pow(i - (7 / 8) * (fps / 2), 2) + (11 / 10) * rad;
				//setMouseCallback("Result", onMouse);

				draw = Mat::zeros(img.size(), img.type());
				warp = Mat::zeros(img.size(), img.type());

				cir1.Draw(draw);
				cir2.Draw(draw);
			

				moveWindow("draw", 2200, 10);
				showImg("draw", draw, scale_view);
				warpPerspective(draw, warp, h_T[0], img.size());

				subtract(frame_start, warp, result);
				setMouseCallback(SetWindow, onMouse);

				imwrite("result/result1_" + to_string(i) + "_appear.jpg", result);
				showImg("Result", result, scale_view);

				waitKey(1);

			}

			waitKey(1);


			//2. Triangle Appear
			for (int i = 0; i < time_ani; i++)
			{

				tri.Appear();
				spin += 360 / time_ani;
				setMouseCallback("Result", onMouse);

				draw = Mat::zeros(img.size(), img.type());
				warp = Mat::zeros(img.size(), img.type());

				double dx, dy;
				//double aniStartX, aniStartY, aniEndX, aniEndY;

				dx = double(arr1.StartX + (arr1.EndX - arr1.StartX) / pow(time_ani, 1.0 / 2.5) * pow(i + 1, 1.0 / 2.5));
				dy = double(arr1.StartY + (arr1.EndY - arr1.StartY) / pow(time_ani, 1.0 / 2.5) * pow(i + 1, 1.0 / 2.5));

				int dxx = ceil(dx);
				int dyy = ceil(dy);
				cir1.Draw(draw);
				cir2.Draw(draw);
				tri.Draw(draw);
			

				showImg("draw", draw, scale_view);
				warpPerspective(draw, warp, h_T[0], img.size());

				subtract(frame_start, warp, result);
				setMouseCallback(SetWindow, onMouse);

				imwrite("result/result2_" + to_string(i) + "_arrow.jpg", result);
				showImg("Result", result, scale_view);
				waitKey(1);


			}

			spin = 0;
			// 3. frame swipe
			for (int cnt = 0; cnt < str.size() - 2; cnt++) {

				p_img = imread(str[cnt]);
				findHomo(cnt);

				//warpPerspective(img, drawimg, h[cnt], drawimg.size());	
				//spin += 5;
				//spin += 360 / fps * 2;
				// setMouseCallback("Result", onMouse);

				draw = Mat::zeros(img.size(), img.type());
				warp = Mat::zeros(img.size(), img.type());

				cir1.Draw(draw);
				cir2.Draw(draw);
				tri.Draw(draw);

				warpPerspective(draw, warp, h_T[cnt], img.size());
				subtract(p_img, warp, result);
				setMouseCallback(SetWindow, onMouse);

				imwrite("result/result3_" + to_string(cnt) + "_swipe.jpg", result);
				showImg("Result", result, scale_view);

				waitKey(1);

			}

			//4. Drawing Disappear
			frame_end = imread(str[str.size() - 1]);

			findHomo(str.size() - 1);
			for (int i = 0; i < time_ani; i++)
			{
				tri.Disppear();
				cir1.Disppear();
				cir2.Disppear();

				setMouseCallback("Result", onMouse);

				draw = Mat::zeros(img.size(), img.type());
				warp = Mat::zeros(img.size(), img.type());

				cir1.Draw(draw);
				cir2.Draw(draw);
				tri.Draw(draw);

				warpPerspective(draw, warp, h_T[str.size() - 1], img.size());

				subtract(frame_end, warp, result);
				setMouseCallback(SetWindow, onMouse);

				imwrite("result/result4_" + to_string(i) + "_disappear.jpg", result);
				showImg("Result", result, scale_view);
				waitKey(1);

			}


			break;
		}



		waitKey(1);
	}


	return 0;
}

void findHomo(int n)
{

	parse.parsing_json();

	for (int i = 0; i < 4; i++) {
		wX[i] = parse.wpointX[i];
		wY[i] = parse.wpointY[i];
	}

	for (int j = 0; j < str.size(); j++)
	{
		for (int i = 0; i < 4; i++) {
			pX[j][i] = parse.pointX[j][i];
			pY[j][i] = parse.pointY[j][i];
		}
	}
	//double newpointX[4] = { 0,wid,wid,0 };
	//double newpointY[4] = { 0,0,hei,hei };

	vector<Point2d> pts_src, pts_dst;

	//for (size_t i(0); i < 4; i++) {
	//	pts_src.push_back(Point2d(pointX[i], pointY[i]));
	//	pts_dst.push_back(Point2d(wpointX[i], wpointY[i]));
	//	//pts_dst.push_back(Point2d(newpointX[i], newpointY[i]));
	//}
	//for (size_t i(0); i < 4; i++) {
	//	pts_src.push_back(Point2d(pointX[n][i], pointY[n][i]));
	//	pts_dst.push_back(Point2d(wpointX[i], wpointY[i]));
	//}

	for (size_t i(0); i < 4; i++) {
		pts_src.push_back(Point2d(pX[n][i], pY[n][i]));
		pts_dst.push_back(Point2d(wX[i], wY[i]));
	}
	//img to plane
	h[n] = findHomography(pts_src, pts_dst, RANSAC);

	h11 = h[n].at<double>(0, 0);
	h12 = h[n].at<double>(0, 1);
	h13 = h[n].at<double>(0, 2);
	h21 = h[n].at<double>(1, 0);
	h22 = h[n].at<double>(1, 1);
	h23 = h[n].at<double>(1, 2);
	h31 = h[n].at<double>(2, 0);
	h32 = h[n].at<double>(2, 1);

	//plane to img
	h_T[n] = findHomography(pts_dst, pts_src, RANSAC);

	th11 = h_T[n].at<double>(0, 0);
	th12 = h_T[n].at<double>(0, 1);
	th13 = h_T[n].at<double>(0, 2);
	th21 = h_T[n].at<double>(1, 0);
	th22 = h_T[n].at<double>(1, 1);
	th23 = h_T[n].at<double>(1, 2);
	th31 = h_T[n].at<double>(2, 0);
	th32 = h_T[n].at<double>(2, 1);
}

void calcHomograpy(double x, double y, double* dx, double* dy)
{
	*dx = (h11 * x + h12 * y + h13) / (h31 * x + h32 * y + 1);
	*dy = (h21 * x + h22 * y + h23) / (h31 * x + h32 * y + 1);
}

void calcHomograpy1(Point point, Point* h_point)
{
	double dx = (h11 * point.x + h12 * point.y + h13) / (h31 * point.x + h32 * point.y + 1);
	double dy = (h21 * point.x + h22 * point.y + h23) / (h31 * point.x + h32 * point.y + 1);
	*h_point = Point(dx, dy);
}

void calcHomograpy_T(double x, double y, double* dx, double* dy)
{
	*dx = (th11 * x + th12 * y + th13) / (th31 * x + th32 * y + 1);
	*dy = (th21 * x + th22 * y + th23) / (th31 * x + th32 * y + 1);
}


void showImg(string filename, Mat img, float factor)
{
	namedWindow(filename, 0);
	imshow(filename, img);
	resizeWindow(filename, img.cols * factor, img.rows * factor);
}




void Circle::Draw(Mat plane)
{
	//double theta = atan2(c.y - c.ey, c.x - c.ex) * 180.0 / CV_PI;
	int beta = 15; //beta는 360의 약수
	alpha = alpha;
	max_a = alpha_max;
	//Shape::setColor(color, &rgb);
	SetColor(color, &rgb);

	switch (circle_type)
	{
	case 0:
		ellipse(plane, Point(center), Size(rad, rad), 0, 0, 360, rgb, -1, 16);
		ellipse(plane, Point(center), Size(rad + 2, rad + 2), spin - 30, 0, 60, Scalar(0, 0, 0), -1, 16);
		ellipse(plane, Point(center), Size(rad, rad), spin - 30, 0, 60, color2 * alpha / max_a, -1, 16);
		ellipse(plane, Point(center), Size(rad * 0.6, rad * 0.6), 0, 0, 360, Scalar(0, 0, 0), -1, 16);
		break;

	case 1:
		ellipse(plane, Point(center), Size(rad, rad), 0, 0, 360, rgb, -1, 16);
		ellipse(plane, Point(center), Size(rad - thick, rad - thick), 0, 0, 360, Scalar(0, 0, 0), -1, 16);
		break;

	case 2:
		ellipse(plane, Point(center), Size(rad, rad), 0, 0, 360, rgb, -1, 16);
		for (int i = 0; i < 360 / beta; i++)
		{
			if (i % 2 == 0)
			{
				ellipse(plane, Point(center), Size(rad + 2, rad + 2), spin + beta * i, 0, beta * 0.6, Scalar(0, 0, 0), -1, 16);
			}
		}
		ellipse(plane, Point(center), Size(rad - thick * 0.5, rad - thick * 0.5), 0, 0, 360, Scalar(0, 0, 0), -1, 16);
		break;

	case 3:
		ellipse(plane, Point(center), Size(rad, rad), 0, 0, 360, rgb, -1, 16);
		break;

	case 4:
		//ellipse(plane, Point(c.center), Size(rad, rad), 0, 0, 360, Scalar(c.color[0], c.color[1], c.color[2]), -1, 16);
		//circle(plane, Point(c.center), rad, Scalar(c.color[0], c.color[1], c.color[2]), -1, 16);
		break;
	}
}


void Arrow::Draw(Mat plane, double dx, double dy)
{
		if (StartX == EndX)
		{
			//rx = 0;
			if (EndY > StartY)
				angle = CV_PI / 2.0;
			//ry = rad;
			else
				angle = CV_PI * (3.0 / 2.0);
			//	ry = -rad;
		}
		else {
			angle = atan(double(abs(EndY - StartY)) / double(abs(EndX - StartX)));
			if (EndX > StartX && EndY >= StartY)
			{

			}
			else if (StartX > EndX && EndY >= StartY)
			{
				angle = CV_PI - angle;
			}
			else if (StartX > EndX && StartY > EndY)
			{
				angle = CV_PI + angle;
			}
			else if (EndX > StartX && StartY > EndY)
			{
				angle = 2 * CV_PI - angle;
			}
		}

	alpha = alpha;
	max_a = alpha_max;
	//Arrow arr;
	//Arrow.setColor(color, &rgb);
	SetColor(color, &rgb);


	//int k = 1;
	//
	Point pts[1][3];
	len = sqrt(pow(dx - (rad / 2) * cos(angle) - StartX, 2) + pow(dy - (rad / 2) * sin(angle) - StartY, 2));

	triEx = dx;
	triEy = dy;
	trip1x = triEx - rad * cos(angle + 30 * M_PI / 180);
	trip1y = triEy - rad * sin(angle + 30 * M_PI / 180);
	trip2x = triEx - rad * cos(angle - 30 * M_PI / 180);
	trip2y = triEy - rad * sin(angle - 30 * M_PI / 180);
	pts[0][0] = Point(dx, dy);
	pts[0][1] = Point(trip1x, trip1y);
	pts[0][2] = Point(trip2x, trip2y);

	const Point* ppt[1] = { pts[0] };
	int npt[] = { 3 };

	double p1 = len / 15.0;
	double p2 = len / 25.0;


	switch (line_type)
	{
	case 0:
		line(plane, Point(StartX, StartY), Point(dx - (rad / 2.0) * cos(angle), dy - (rad / 2.0) * sin(angle)), rgb, thick, 16);
		fillPoly(plane, ppt, npt, 1, rgb, 16);
		break;

	case 1:
		fillPoly(plane, ppt, npt, 1, rgb, 16);
		for (int i = 1; i < p2; i++)
		{

			line(plane, Point(StartX + (i - 1) * (len / p2) * cos(angle) + len / p2 * cos(angle) / 1.5,
				StartY + ((i - 1) * (len / p2) * sin(angle)) + len / p2 * sin(angle) / 1.5),
				Point(StartX + (i * (len / p2) * cos(angle)) - (rad / 2.0) * cos(angle),
					StartY + (i * (len / p2) * sin(angle)) - (rad / 2.0) * sin(angle)), rgb, thick * 0.8, 16);

		}
		break;

	case 2:
		fillPoly(plane, ppt, npt, 1, rgb, 16);
		for (int i = 1; i < p1 + 1; i++)
		{
			circle(plane, Point(StartX + (i - 1) * (len / p1) * cos(angle) + len / p1 * cos(angle) / 4.0, StartY + ((i - 1) * (len / p1) * sin(angle)) + len / p1 * sin(angle) / 4.0), 4, rgb, -1, 8);
		}
		break;
	}


}




//draw 3d calib ROI 
void Triangle::Draw(Mat dst)
{
	alpha = alpha;
	max_a = alpha_max;
	SetColor(color, &rgb);

	Point pts[1][3];

	pts[0][0] = wp[0];
	pts[0][1] = wp[1];
	pts[0][2] = wp[2];

	const Point* ppt[1] = { pts[0] };
	int npt[] = { 3 };

	if (fill == 1)
	{
		fillPoly(dst, ppt, npt, 1, rgb, 8);
	}
	else
	{
		line(dst, wp[0], wp[1], rgb, thick, 8);
		line(dst, wp[1], wp[2], rgb, thick, 8);
		line(dst, wp[2], wp[0], rgb, thick, 8);
	}
}


void Arrow::Drawing(double* dx, double* dy, int i)
{
	*dx = round(double(StartX + (EndX - StartX) / pow(time_ani, 1.0 / 2.5) * pow(i + 1, 1.0 / 2.5)));
	*dy = round(double(StartY + (EndY - StartY) / pow(time_ani, 1.0 / 2.5) * pow(i + 1, 1.0 / 2.5)));
}

void Arrow::CalcAngle(double* angle)
{
	if (StartX == EndX)
	{
		//rx = 0;
		if (EndY > StartY)
			*angle = CV_PI / 2.0;
		//ry = rad;
		else
			*angle = CV_PI * (3.0 / 2.0);
		//	ry = -rad;
	}
	else {
		*angle = atan(double(abs(EndY - StartY)) / double(abs(EndX - StartX)));
		if (EndX > StartX && EndY >= StartY)
		{

		}
		else if (StartX > EndX && EndY >= StartY)
		{
			*angle = CV_PI - *angle;
		}
		else if (StartX > EndX && StartY > EndY)
		{
			*angle = CV_PI + *angle;
		}
		else if (EndX > StartX && StartY > EndY)
		{
			*angle = 2 * CV_PI - *angle;
		}
		//rx = rad * cos(angle);
		//ry = rad * sin(angle);
	}
}

void eventclass::HandleEvent(int evt, int x, int y, int flags)
{

	if (evt == EVENT_LBUTTONDOWN)
	{

		fx = x;
		fy = y;
		cout << "\nx,y:" << x << "," << y << endl;

	}

	if (evt == EVENT_RBUTTONDOWN)
	{
		fx2 = x;
		fy2 = y;
		cout << "\n2x,2y:" << x << "," << y << endl;
	}

}


void onMouse(int evt, int x, int y, int flags, void* param)
{
	eventclass* p = (eventclass*)param;
	p->HandleEvent(evt, x, y, flags);
}

