//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2/calib3d.hpp>
//#include <opencv2/opencv.hpp>
//#include "json/json.h"
#include <iostream>
#include <time.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>
#include <fstream>
#include "parseJson.h"
#include "3dDraw.h"
#include "drawing.h"

#pragma comment(lib, "jsoncpp.lib")

#define FILE_NAME "1/000100.jpg"	

using namespace cv;
using namespace std;


// PATH
string pts_path = "1.pts";		//pts file
String path("1/*.jpg");
vector<String> str;


// H Matrix 
int num = 19;
Mat h[19] = {};					//Homograpy Matrix	(3d -> 2d)		
Mat h_T[19] = {};				//Homograpy Matrix	(2d -> 3d)
double wX[4] = {};
double wY[4] = {};
double pX[19][4] = { {},{},{},{} };		// dsc 개수만큼 배열 생성
double pY[19][4] = { {},{},{},{} };
double h11, h12, h13, h21, h22, h23, h31, h32;		//homogeneous matrix의 요소를 담는 변수
double th11, th12, th13, th21, th22, th23, th31, th32;

							

// Image MAT
Mat frame;
Mat warp;		//그려질 평면 
//Mat img = imread("000126.jpg");


// POINT
vector<Point> selPoint(10);
vector<Point> wPoint(10);
//vector<Point> pts[10];
//vector<Point2d> ptsList(10);
//vector<Coord> pts[2];



// etc.
int fx, fy;				//첫번째 원의 point 
int fx2, fy2;			//두번째 원의 point

int radius;		//input radius
int radius1, radius2;
int rad;
//color
int r = 123, g = 220, b = 220;
int color_c1 = 0;
int color_c2 = 0;
int color_t = 3;
int color_l = 5;

//parameter
int line_type = 1;			// 1.solid	2.dotted(1)	3.dotted(2)
int circle_type = 0;		// 1.spin	2.ring	3.dotted	4.solid(fill)
int thick = 4;
int spin = 0;		//spin circle parameter


int cnt = 1;


int fps = 30;

float scale_view = 1;		//image scale
int time_ani = 15;
String SetWindow = "Settings";
//int alpha = 0;



//int num = 19;
//string dsc[19] = {};


//Mat flag = imread("korea.png");
//Mat inner = imread("tae300.png");



//Mat img;
//VideoCapture cap(FILE_NAME);
//int fps = (int)cap.get(CAP_PROP_FPS); 


// Object Class
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



class Shape
{
private:
	int alpha;
	

public:

	int color;
	int alpha_max;
	Scalar rgb;

	void SetColor(int color_c, Scalar* color);
	void Appear() { alpha += alpha_max / time_ani; }
	void Disppear() { alpha -= alpha_max / time_ani; }

};

class Arrow :public Shape
{

private:
	double angle;

public:
	Point Start, End;
	//Point Start_W, End_W;
	//int StartX, StartY, EndX, EndY;

	int line_type;
	int thick;
	double triEx, triEy;					//삼각형 끝 포인트
	double trip1x, trip1y, trip2x, trip2y;	//삼각형 나머지 두 포인트
	vector<string> type{ "solid","dotted" };
	
	void Draw(Mat plane, Point e);
	void Drawing(Point* p, int i);
	//void CalcAngle(double* angle);
	//Arrow();
};

class Circle :public Shape {

public:
	Point center;

	int circle_type;
	int thick;
	double r;
	int fill;
	int color2;
	Scalar rgb2;

	void Draw(Mat dst);
	//vector<Point> p;

};

class Triangle :public Shape {

public:

	int fill;
	int thick;
	vector<Point> p;
	vector<Point> wp;

	void Draw(Mat plane);

};


//vector<Point> Tripoint(Point p1, Point p2, Point p3)
//{
//	vector<Point> triangle;
//	triangle.push_back(p1);
//	triangle.push_back(p2);
//	triangle.push_back(p3);
//	return triangle;
//}

class Animation {

};



void showImg(string filename, Mat img, float factor);
void onMouse(int evt, int x, int y, int flags, void* param);
void calcHomograpy(Point point, Point* h_point);
void calcHomograpy_T(double x, double y, double* dx, double* dy);
void findHomo(int n);
//void drawCircle(Mat plane, Circle c);
//void drawLine(Mat plane, double x, double y, double dx, double dy, Arrow arr);
//void drawPlane(Mat dst, Triangle t);
//void calcHomograpy(double x, double y, double* dx, double* dy);

//double arrStartX, arrStartY, arrEndX, arrEndY;
//double rx, ry;			//circle 센터로부터 arr start,end point 계산할때 사용되는 변수

//struct transP {			//homogeneous matrix를 적용하여 계산한 포인트가 담기는 변수(warp point)
//	double hx;
//	double hy;
//};
//
//struct warpPoint
//{
//	double x, y;
//	vector<Point2d> p;
//};
//
//struct planePoint {
//	Point p[3];
//};


//객체별로 수정 필요

//Scalar red(200, 200, 20);
//Scalar orange(alpha, 50, 20);
//Scalar yellow(255, 0, 0);
//Scalar green(alpha, 40, alpha);
//Scalar blue(30, 90, alpha);
//Scalar white(30, 30, 30);
//Scalar black(100, 100, 100);
//double scale = 1;			//roi size


//double lineEndX, lineEndY;


class eventclass
{
public:
	void HandleEvent(int evt, int x, int y, int flags);
};

//struct transP f1, f2;		//circle point calc h_matrix
//struct transP t1, t2, t3;	//
////struct transP as, ae;		//arrow start,end trans point
////struct transP has, hae;		//arrow homo, start end warp point
////struct warpPoint warp1, warp2, warp3;	// triangle point

parseJson parse;


//int key = waitKey();
//Coord p[10];
//Coord p1, p2;






void Shape::SetColor(int color_c, Scalar* color) {

	Scalar rgb;
	//int r = 123, g = 220, b = 220;

	switch (color_c)
	{
	case 0:
		rgb = Scalar((255 - b) * alpha / alpha_max, (255 - g) * alpha / alpha_max, (255 - r) * alpha / alpha_max);				//custom
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


// Object Make
Arrow arr1, arr2;
Circle cir1, cir2;
Triangle tri;
Point ani, ani2;


int main(int ac, char** av)
{
	selPoint[0] = Point(624, 812);	//cir1
	selPoint[1] = Point(405, 613);	//cir2

	selPoint[2] = Point(720, 782);	//arr1.s 
	selPoint[3] = Point(520, 596);	//arr1.e

	selPoint[4] = Point(519, 593);	//arr2.s
	selPoint[5] = Point(1137, 536);	//arr2.e
	selPoint[6] = Point(1261, 500);

	selPoint[7] = Point(622, 804);	//tri
	selPoint[8] = Point(1278, 513);
	selPoint[9] = Point(1261, 500);


	Mat img = imread("1/000100.jpg");
	//Mat img = imread("1/000136.jpg");
	Mat p_img;
	clock_t start, end, start_1, end_1, start_2, end_2, start_3, end_3;

	Mat plane(img.size(), img.type(), Scalar(50, 130, 55));
	Mat draw = Mat::zeros(plane.size(), plane.type());		//2d draw
	Mat draw_n;
	Mat drawimg = Mat::zeros(img.size(), img.type());
	warp = Mat::zeros(img.size(), img.type());
	Mat result = Mat::zeros(img.size(), img.type());
	Mat roi, img_warp, frame_end, frame_start;
	//Mat plane(hei * 2, wid * 2, img.type(), Scalar(50, 130, 55));	//ROI
	//Mat flag_mini, flag_warp;
	//Mat inner_mini, inner_warp; 

	//int t = 1; //time second 


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

	//wid, hei, rad 계산
	parse.parsing_json();

	for (int i = 0; i < 4; i++) {
		wX[i] = parse.wpointX[i];
		wY[i] = parse.wpointY[i];
	}
	double Wid_Pixel = (abs(wX[1] - wX[0]) + abs(wX[2] - wX[3])) * 0.5;
	double Hei_Pixel = (abs(wY[2] - wY[1]) + abs(wY[3] - wY[0])) * 0.5;

	/*double wid = (abs(wpointX[1] - wpointX[0]) + abs(wpointX[2] - wpointX[3])) * 0.5;
	double hei = (abs(wpointY[2] - wpointY[1]) + abs(wpointY[3] - wpointY[0])) * 0.5;*/
	//radius 계산		 rad_pixel : 1m = length_pixel : length_real

	int Wid_Real = 13500;	//	(millimeter)
	int Hei_Real = 3300; 
	int radius_Real = 500;
	radius1 = (radius_Real * Wid_Pixel) / Wid_Real;
	radius2 = (radius_Real * Hei_Pixel) / Hei_Real;
	radius = radius1 + radius2;


	namedWindow(SetWindow, WINDOW_AUTOSIZE);
	//createTrackbar("x", SetWindow, &fx, wid);
	//createTrackbar("y", SetWindow, &fy, hei);
	createTrackbar("radius", SetWindow, &radius, 50);
	createTrackbar("C1 Color", SetWindow, &color_c1, 7);
	createTrackbar("C2 Color", SetWindow, &color_c2, 7);
	createTrackbar("Line Color", SetWindow, &color_l, 7);
	createTrackbar("R", SetWindow, &r, 255);
	createTrackbar("G", SetWindow, &g, 255);
	createTrackbar("B", SetWindow, &b, 255);
	createTrackbar("Circle/two color/Fill/null", SetWindow, &circle_type, 4);
	createTrackbar("Line/Arrow/null", SetWindow, &line_type, 2);
	//createTrackbar("Transparency", SetWindow, &alpha, 255);
	//createTrackbar("Thickness", SetWindow, &thick, 20);
	//setTrackbarMin("Thickness", SetWindow, 1);
	//createTrackbar("Arrow tip", SetWindow, &arrow_tip, 10);
	//setWindowProperty(SetWindow, WND_PROP_TOPMOST, 1);
	//resize(flag, flag_mini, flag.size() / 4, 0, 0, cv::INTER_AREA);
	//cvtColor(inner, inner, COLOR_BGR2RGB);
	//resize(inner, inner_mini,Size(2*rad-thick,2*rad-thick), 0, 0, cv::INTER_AREA);
	//imshow("korea test", flag_mini);
	//warpPerspective(img, img_warp, h[0], img.size());
	//imshow("warp img", img_warp);


	cout << "str " << str.size() << endl;
	

	//find warp point 
	findHomo(0);
	for (int i = 0; i < 10; i++)
		calcHomograpy(selPoint[i], &wPoint[i]);

	//findHomo(str.size()-1);

	//wp1.x = warp1.x;
	//wp1.y = warp1.y;
	//wp2.x = warp2.x;
	//wp2.y = warp2.y;
	//wp3.x = warp3.x;
	//wp3.y = warp3.y;




	cout << "===============Select Mode==============" << endl << endl;
	cout << "1. Circle and Arrow (input 2 points) " << endl;
	cout << "2. 2 Circle , 1 Arrow (input 4 points) " << endl;
	cout << "3. 2 Circle , 1 Triangle (input 5 points) " << endl;
	int mode;
	//cin >> mode;
	mode = 2;




	while (1) {

		cout << "Click two points for Circle center" << endl << endl;

		showImg("Click two Points", img, scale_view);
		setMouseCallback("Click two Points", onMouse);
		waitKey(0);

	/*	calcHomograpy(fx, fy, &f1.hx, &f1.hy);
		calcHomograpy(fx2, fy2, &f2.hx, &f2.hy);*/
		/*calcHomograpy(trip1x, trip1y, &t1.hx, &t1.hy);
		calcHomograpy(trip2x, trip2y, &t2.hx, &t2.hy);
		wt1.x = t1.hx;
		wt1.y = t1.hy;
		wt2.x = t2.hx;
		wt2.y = t2.hy;*/
	/*	while (p[0].x != fx)
			p[0].SetPoint(fx, fy);

		while (p[1].x != fx2)
		
			p[1].SetPoint(fx2, fy2);*/
		/*calcHomograpy(p[0].x, p[0].y, &arr1_s.hx, &arr1_s.hy);
		calcHomograpy(p[1].x, p[1].y, &arr1_e.hx, &arr1_e.hy);
	*/

		findHomo(0);

		////////////////////////////////////////
		/*	 Object Initialize	*/ 
			
		/*	Circle1	*/
		cir1.circle_type = circle_type;
		cir1.color = color_c1;
		cir1.color2 = 6;
		cir1.fill = 1;
	//	cir1.r = rad;
		cir1.r = 30;
		cir1.thick = radius / 2;
		cout << "thick " << cir1.thick << endl;
		cir1.center = wPoint[0];
		cir1.alpha_max = 30 * (255 / 100);


		/*	Circle2	*/
		cir2.circle_type = circle_type;
		cir2.color = color_c2;
		cir2.color2 = 6;
		cir2.fill = 1;
		//cir2.r = rad;
		cir2.r = 20;
		cir2.thick = radius / 2;
		cir2.center = wPoint[1];
		cir2.alpha_max = 50 * (255.0 / 100.0);


		/*	Triangle	*/
		//p1 = Point(622, 804), p2 = Point(1278, 513), p3 = Point(1261, 500);
		tri.alpha_max = 70 * (255.0 / 100.0);
		tri.color = color_l;
		tri.fill = 1;
		tri.thick = 5;
		tri.wp = { wPoint[7],wPoint[8],wPoint[9] };

		/*	Arrow1	*/
		arr1.color = color_l;
		arr1.thick = radius / 3.0;
		Point click1 = Point(fx, fy);
		Point click2 = Point(fx2, fy2);
		calcHomograpy(click1, &arr1.Start);
		calcHomograpy(click2, &arr1.End);
	/*	arr1.Start = Point(arr1_s.hx, arr1_s.hy);
		arr1.End = Point(arr1_e.hx, arr1_e.hy);*/
		arr1.line_type = 0;
		arr1.alpha_max = 40 * (255.0 / 100.0);


		/*	Arrow2	*/
		arr2.color = color_l;
		arr2.thick = radius / 3.0;
		arr2.Start = wPoint[4];
		arr2.End = wPoint[5];
		arr2.line_type = line_type;
		arr2.alpha_max = 30 * (255.0 / 100.0);
		//////////////////////////////////////////////////////////

		//Animation Settings
		frame_start = imread(str[0]);
		spin = 0;
		int draw_start = 50;

		/*if (img.empty())
		{
			printf("empty image");
			return 0;
		}*/
		//circle point 정의

		/*calcHomograpy(triEx, triEy, &t1.hx, &t1.hy);
		calcHomograpy(trip1x, trip1y, &t2.hx, &t2.hy);
		calcHomograpy(trip2x, trip2y, &t3.hx, &t3.hy);
*/

		/////////////////////////////////////////////
		/*		test		*/
		switch (mode) {

		case 1:	//1. Circle and Arrow 
			//1. Drawing Appear
			
			for (int i = 0; i < time_ani; i++)
			{
				cir1.Appear();
				cir2.Appear();
				arr1.Appear();

				spin += 360 / time_ani;
				
				rad = (((1.0 - 0.5) / time_ani) * i + 0.5) * radius;
				//auto_r = ((-192.0 * rad) / (245 * pow((fps / 2), 2))) * pow(i - (7 / 8) * (fps / 2), 2) + (11 / 10) * rad;
				//setMouseCallback("Result", onMouse);

				draw = Mat::zeros(img.size(), img.type());
				warp = Mat::zeros(img.size(), img.type());

				cir1.Draw(draw);
				cir2.Draw(draw);

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

				//double dx, dy;
				//double aniStartX, aniStartY, aniEndX, aniEndY;

				//arr1.Drawing(&dx, &dy, i);
				
				arr1.Drawing(&ani, i);

				cir1.Draw(draw);
				cir2.Draw(draw);
				arr1.Draw(draw, ani);
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
				arr1.Draw(draw, arr1.End);
				//arr1.Draw(draw, arr1.EndX, arr1.EndY);
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
				arr1.Draw(draw, arr1.End);
				//arr1.Draw(draw, arr1.EndX, arr1.EndY);

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
				rad = (((1.0 - 0.5) / time_ani) * i + 0.5) * radius;
				cir1.Appear();
				cir2.Appear();
				arr1.Appear();
				spin += 360 / time_ani;


				arr1.Drawing(&ani, i);
				//auto_r = ((-192.0 * rad) / (245 * pow((fps / 2), 2))) * pow(i - (7 / 8) * (fps / 2), 2) + (11 / 10) * rad;
				//setMouseCallback("Result", onMouse);

				draw = Mat::zeros(img.size(), img.type());
				warp = Mat::zeros(img.size(), img.type());

				cir1.Draw(draw);
				cir2.Draw(draw);
				arr1.Draw(draw, ani);
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
				arr1.Draw(draw, arr1.End);
		
				blur(draw, draw, Size(2, 2));

				warpPerspective(draw, warp, h_T[cnt], img.size());


				subtract(p_img, warp, result);
				setMouseCallback(SetWindow, onMouse);

				imwrite("result/result3_" + to_string(cnt) + "_swipe.jpg", result);
				showImg("Result", result, scale_view);

				waitKey(1);

			}

			frame_end = imread(str[str.size() - 1]);



			
			//3. Arrow2 Appear
			findHomo(str.size() - 1);
			//calcHomograpy(selPoint[5], &wPoint[5]);
			//calcHomograpy(selPoint[5], &wPoint[6]);
			for (int i = 0; i < time_ani; i++)
			{
				arr2.Appear();
				
				spin += 360 / time_ani;
				setMouseCallback("Result", onMouse);

				draw = Mat::zeros(img.size(), img.type());
				warp = Mat::zeros(img.size(), img.type());


				cir1.Draw(draw);
				cir2.Draw(draw);
				arr1.Draw(draw, arr1.End);

				arr2.Drawing(&ani2, i);
				arr2.Draw(draw, ani2);
			

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
				arr1.Draw(draw, arr1.End);
				arr2.Draw(draw, arr2.End);


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
				rad = (((1.0 - 0.5) / time_ani) * i + 0.5) * radius;
				cir1.Appear();
				cir2.Appear();
				
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

/// //////////////////////////////////////////


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


void calcHomograpy(Point point, Point* h_point)
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
	int beta = 15; //beta는 360의 약수, 점선의 간격을 설정

	//Shape::setColor(color, &rgb);
	SetColor(color, &rgb);
	SetColor(color2, &rgb2);

	switch (circle_type)
	{
	case 0:
		ellipse(plane, Point(center), Size(rad, rad), 0, 0, 360, rgb, -1, 16);
		ellipse(plane, Point(center), Size(rad + 2, rad + 2), spin - 30, 0, 60, Scalar(0, 0, 0), -1, 16);
		ellipse(plane, Point(center), Size(rad, rad), spin - 30, 0, 60, rgb2, -1, 16);
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

//void Arrow::Draw(Mat plane, double dx, double dy)
void Arrow::Draw(Mat plane, Point e)
{
	// drawing과 삼각형위치 계산을 위한 Angle 계산
	if (Start.x == End.x)
	{
		//rx = 0;
		if (End.y > Start.y)
			angle = CV_PI / 2.0;
		//ry = rad;
		else
			angle = CV_PI * (3.0 / 2.0);
		//	ry = -rad;
	}
	else {
		angle = atan(double(abs(End.y - Start.y)) / double(abs(End.x - Start.x)));
		if (End.x > Start.x && End.y >= Start.y)
		{

		}
		else if (Start.x > End.x && End.y >= Start.y)
		{
			angle = CV_PI - angle;
		}
		else if (Start.x > End.x && Start.y > End.y)
		{
			angle = CV_PI + angle;
		}
		else if (End.x > Start.x && Start.y > End.y)
		{
			angle = 2 * CV_PI - angle;
		}
	}


	//Arrow arr;
	//Arrow.setColor(color, &rgb);
	SetColor(color, &rgb);


	Point pts[1][3];
	double len;
	len = sqrt(pow(e.x - (radius / 2) * cos(angle) - Start.x, 2) + pow(e.y - (radius / 2) * sin(angle) - Start.y, 2));

	triEx = e.x;
	triEy = e.y;
	trip1x = triEx - radius * cos(angle + 30 * M_PI / 180);
	trip1y = triEy - radius * sin(angle + 30 * M_PI / 180);
	trip2x = triEx - radius * cos(angle - 30 * M_PI / 180);
	trip2y = triEy - radius * sin(angle - 30 * M_PI / 180);
	pts[0][0] = e;
	pts[0][1] = Point(trip1x, trip1y);
	pts[0][2] = Point(trip2x, trip2y);

	const Point* ppt[1] = { pts[0] };
	int npt[] = { 3 };

	double p1 = len / 15.0;
	double p2 = len / 25.0;


	switch (line_type)
	{
	case 0:
		line(plane, Point(Start.x, Start.y), Point(e.x - (radius / 2.0) * cos(angle), e.y - (radius / 2.0) * sin(angle)), rgb, thick, 16);
		fillPoly(plane, ppt, npt, 1, rgb, 16);
		break;

	case 1:
		fillPoly(plane, ppt, npt, 1, rgb, 16);
		for (int i = 1; i < p2; i++)
		{

			line(plane, Point(Start.x + (i - 1) * (len / p2) * cos(angle) + len / p2 * cos(angle) / 1.5,
				Start.y + ((i - 1) * (len / p2) * sin(angle)) + len / p2 * sin(angle) / 1.5),
				Point(Start.x + (i * (len / p2) * cos(angle)) - (radius / 2.0) * cos(angle),
					Start.y + (i * (len / p2) * sin(angle)) - (radius / 2.0) * sin(angle)), rgb, thick * 0.8, 16);

		}
		break;

	case 2:
		fillPoly(plane, ppt, npt, 1, rgb, 16);
		for (int i = 1; i < p1 + 1; i++)
		{
			circle(plane, Point(Start.x + (i - 1) * (len / p1) * cos(angle) + len / p1 * cos(angle) / 4.0, Start.y + ((i - 1) * (len / p1) * sin(angle)) + len / p1 * sin(angle) / 4.0), 4, rgb, -1, 8);
		}
		break;
	}
}


//draw 3d calib ROI 
void Triangle::Draw(Mat dst)
{
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


void Arrow::Drawing(Point* p, int i)
{
	double dx, dy;
	dx = round(double(Start.x + (End.x - Start.x) / pow(time_ani, 1.0 / 2.5) * pow(i + 1, 1.0 / 2.5)));
	dy = round(double(Start.y + (End.y - Start.y) / pow(time_ani, 1.0 / 2.5) * pow(i + 1, 1.0 / 2.5)));

	*p = Point(dx, dy);
}


//void Arrow::CalcAngle(double* angle)
//{
//	if (Start.x == End.x)
//	{
//		//rx = 0;
//		if (End.y > Start.y)
//			*angle = CV_PI / 2.0;
//		//ry = rad;
//		else
//			*angle = CV_PI * (3.0 / 2.0);
//		//	ry = -rad;
//	}
//	else {
//		*angle = atan(double(abs(End.y - Start.y)) / double(abs(End.x - Start.x)));
//		if (End.x > Start.x && End.y >= Start.y)
//		{
//
//		}
//		else if (Start.x > End.x && End.y >= Start.y)
//		{
//			*angle = CV_PI - *angle;
//		}
//		else if (Start.x > End.x && Start.y > End.y)
//		{
//			*angle = CV_PI + *angle;
//		}
//		else if (End.x > Start.x && Start.y > End.y)
//		{
//			*angle = 2 * CV_PI - *angle;
//		}
//		//rx = rad * cos(angle);
//		//ry = rad * sin(angle);
//	}
//}

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

