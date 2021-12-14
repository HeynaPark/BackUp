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


vector<int> selTime(10);
// POINT
vector<Point2d> selPoint(10);
vector<Point2d> wPoint(10);
int fx, fy;				//첫번째 원의 point 
int fx2, fy2;			//두번째 원의 point





//color
class TestProgram {
public:

	int color_c1 = 0;
	int color_c2 = 0;
	int color_t = 6;
	int color_l = 4;

	int line_type = 0;			// 1.solid	2.dotted(1)	3.dotted(2)
	int circle_type = 0;		// 1.spin	2.ring	3.dotted	4.solid(fill)
	int thick = 2;
	int r =54, g=173, b=255;
	int max_alpha=100;
};

//parameter


float scale_view = 1;		//image scale

String SetWindow = "Settings";

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



enum eColors
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

//enum eShapes
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


public:
	Scalar rgb;
	int mAbs_val_();
	int mAlpha_;
	int mAlpha_max_;
	int r, g, b;
	//double mAbsolute_val_;


	int mTime_ani_ = 30;
	enum COLOR { CUSTOM, RED, ORANGE, YELLOW, GREEN, BLUE, CYAN, BLACK,
		GOLD, OLIVE, AQUAMARIN, SKYBLUE, VYOLET, PINK, MINT, BRIKRED, BROUN
	};
	/*int mColor_;
	int mColor_spin;*/
	COLOR mColor_;
	COLOR mColor_spin_;

	void SetColor(COLOR c, Scalar* color);
	void Appear(int t) { mAlpha_ += (mAlpha_max_*10 +40) / t; }		//alpha_max값을 1~11단계로 입력하는 계산식
	void Disppear(int t) { mAlpha_ -= (mAlpha_max_ * 10 + 40) / t; }


};


//void calcAutoRadius(double* value)
//{
//	parseJson parse;
//	parse.parsing_json();
//
//	for (int i = 0; i < 4; i++) {
//		wX[i] = parse.wpointX[i];
//		wY[i] = parse.wpointY[i];
//	}
//	double widPixel = (abs(wX[1] - wX[0]) + abs(wX[2] - wX[3])) * 0.5;
//	double heiPixel = (abs(wY[2] - wY[1]) + abs(wY[3] - wY[0])) * 0.5;
//
//	const int widReal = 13500;	//	(millimeter)
//	const int heiReal = 3300;
//	const int radiusReal = 500;
//
//	int radius1, radius2;
//	radius1 = (radiusReal * widPixel) / widReal;
//	radius2 = (radiusReal * heiPixel) / heiReal;
//	*value = radius1 + radius2;
//	cout << radius1 << " " << radius2 << endl;
//
//}

int Shape::mAbs_val_()
{
	parseJson parse;
	parse.parsing_json();

	for (int i = 0; i < 4; i++) {
		wX[i] = parse.wpointX[i];
		wY[i] = parse.wpointY[i];
	}
	double widPixel = (abs(wX[1] - wX[0]) + abs(wX[2] - wX[3])) * 0.5;
	double heiPixel = (abs(wY[2] - wY[1]) + abs(wY[3] - wY[0])) * 0.5;

	const int widReal = 13500;	//	(millimeter)
	const int heiReal = 3300;
	const int radiusReal = 500; // 0.5M

	int radius1, radius2;
	radius1 = double(radiusReal * widPixel) / widReal;
	radius2 = double(radiusReal * heiPixel) / heiReal;

	return  radius1 + radius2;

}
 

class Circle :public Shape {


	Scalar spin_rgb;
public:
	Point mCenter_;

	int mSpin_;
	int mRadius_;
	int mRadius_start;
	int mRadius_end;
	//int mCircle_type_;
	
	int mThickness_;

	enum cirType { CIRCLE_FILL, CIRCLE_RING, CIRCLE_DOTTED, CIRCLE_SPIN };
	cirType mCircle_type_;

	void Draw(Mat dst);
	void SpinOn() { mSpin_ += 360 / mTime_ani_; }
	void expandRad(int t, int r) { mRadius_ = ((1.0 - mRadius_start) * t / mTime_ani_ + mRadius_start) * r; }
	void contracRad(int t, int r) { mRadius_ = ((mRadius_end - 1.0) * t / mTime_ani_ + 1.0) * r; cout << "contract" << (mRadius_end - 1.0) * t / mTime_ani_  << endl; }

	//vector<Point> p;
};



class Arrow :public Shape
{

private:
	
	double triEx, triEy;					//삼각형 끝 포인트
	double trip1x, trip1y, trip2x, trip2y;	//삼각형 나머지 두 포인트
	double angle;
public:

	Point mStart_, mEnd_;
	Point mSetStart_, mSetEnd_;
	int mSetStart_Flag_, mSetEnd_Flag_;
	

	int mThick_;
	enum class arrType { SOLID, DOTTED_LONG, DOTTED_SHORT };
	int mArrType_;
	void Draw(Mat plane, Point2d e);
	void Drawing(Point2d* p, int i);
	void CalcSetPos(Point p, Point* calcP);

};



class Triangle :public Shape {

public:

	vector<Point> mPoint_;
	vector<Point> mWarpPoint_;

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
void calcHomograpy(Point2d point, Point2d* h_point);
void calcHomograpy_T(Point2d point, Point2d* h_point);
void findHomo(int n);


//Scalar red(200, 200, 20);
//Scalar orange(alpha, 50, 20);
//Scalar yellow(255, 0, 0);
//Scalar green(alpha, 40, alpha);
//Scalar blue(30, 90, alpha);
//Scalar white(30, 30, 30);
//Scalar black(100, 100, 100);

class Eventclass
{
public:
	void HandleEvent(int evt, int x, int y, int flags);
};


// Object Make
Arrow arr1, arr2;
Circle cir1, cir2;

Triangle tri;
Point2d ani, ani2;
Shape sh;
TestProgram test;

struct CirclePARAM :Circle {


};

struct CirclePARAM getParameter()
{

	struct CirclePARAM circle;

	vector<Circle> cir;

	//cir[0].mAlpha_max_ = 50;
	//cir[0].mColor_ = test.color_c1;
	//cir[0].mColor_spin = 3;
	//cir[0].mCenter_ = wPoint[0];
	//cir[0].mRadius_ = sh.mAbs_val_();
	//cir[0].mRadius_start = 50;
	//cir[0].mThickness_ = cir.mRadius_ / 2.0;
	//cir[0].mCircle_type_ = test.circle_type;

	return circle;
}


//초기화 하는 구조체

//초기화 하는 함수?

void initialize()
{
	//생성됐을때 들어가야 하는 값


}

//

void Shape::SetColor(COLOR c, Scalar* color) {

	//rgb;
	int alpha = mAlpha_;
	int alpha_max = mAlpha_max_;
	r = test.r;
	g = test.g;
	b = test.b;
	//int r = 123, g = 220, b = 220;

	switch (c)
	{
	case CUSTOM:
		//rgb = Scalar((255 - b) * alpha / alpha_max, (255 - g) * alpha / alpha_max, (255 - r) * alpha / alpha_max);				//custom
		rgb = Scalar((255 - b) / 255.0 * alpha, (255 - g) / 255.0 * alpha, (255 - r) / 255.0 * alpha);
		break;
	case RED:
		rgb = Scalar(alpha, alpha, 0);		//red
		break;
	case ORANGE:
		rgb = Scalar(alpha, alpha / 2, 0);		//orange
		break;
	case YELLOW:
		rgb = Scalar(alpha, 0, 0);			//yellow
		break;
	case GREEN:
		rgb = Scalar(alpha, 0, alpha);		//green
		break;
	case BLUE:
		rgb = Scalar(0, alpha, alpha);		//blue
		break;
	case CYAN:
		rgb = Scalar(0, 0, alpha);			//cyan
		break;
	case BLACK:
		rgb = Scalar(alpha, alpha, alpha);	//black
		break;

	case AQUAMARIN:
		r = 127, g =255, b=212;
		rgb = Scalar((255 - b) / 255.0 * alpha, (255 - g) / 255.0 * alpha, (255 - r) / 255.0 * alpha);
		break;
	case GOLD:
		r = 255,g = 215, b=0;
		rgb = Scalar((255 - b) / 255.0 * alpha, (255 - g) / 255.0 * alpha, (255 - r) / 255.0 * alpha);
		break;
	case OLIVE:
		r = 85, g =107 , b =47;
		rgb = Scalar((255 - b) / 255.0 * alpha, (255 - g) / 255.0 * alpha, (255 - r) / 255.0 * alpha);
		break;
	case SKYBLUE:
		r = 135, g = 206, b = 250;
		rgb = Scalar((255 - b) / 255.0 * alpha, (255 - g) / 255.0 * alpha, (255 - r) / 255.0 * alpha);
		break;
	case MINT:
		r = 0, g = 206, b = 209;
		rgb = Scalar((255 - b) / 255.0 * alpha, (255 - g) / 255.0 * alpha, (255 - r) / 255.0 * alpha);
		break;
	case VYOLET:
		r = 0, g = 206, b = 209;
		rgb = Scalar((255 - b) / 255.0 * alpha, (255 - g) / 255.0 * alpha, (255 - r) / 255.0 * alpha);
		break;
	case PINK:
		r = 0, g = 206, b = 209;
		rgb = Scalar((255 - b) / 255.0 * alpha, (255 - g) / 255.0 * alpha, (255 - r) / 255.0 * alpha);
		break;
	}

	*color = rgb;
}




int main(int ac, char** av)
{
	glob(path, str, false);

	if (str.size() == 0)
		cout << "image non exist. \n" << endl;

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
	int blur_size = 2;

	struct CirclePARAM c1;
	//c1 = getParameter();


	selTime[0] = 0; // 첫 프레임
	selTime[1] = str.size() - 1;	// 마지막 프레임

	int frame1 = 0;
	int frame2 = selTime[1];


	//frame for draw
	frame_start = imread(str[frame1]);
	frame_end = imread(str[frame2]);
	//frame_end = imread(str[frame2]);
	//frame_end = imread(str[str.size() - 2]);

	selPoint[0] = Point(624, 812);	//cir1
	selPoint[1] = Point(405, 613);	//cir2
					   
	selPoint[2] = Point(720.0, 782.0);	//arr1.s 
	selPoint[3] = Point(520.0, 596.0);	//arr1.e
					   
	selPoint[4] = Point(519, 593);	//arr2.s
	selPoint[5] = Point(1137, 536);	//arr2.e
	selPoint[6] = Point(961, 492);
					   
	selPoint[7] = Point(715, 773);	//tri
	selPoint[8] = Point(1261, 499);	//goal area 1
	selPoint[9] = Point(1339, 536);	//goal area 2




	//parsing_json();
	/*if (!cap.isOpened())
	{
		printf("Can't open the camera");
		return -1;
	}*/
	//cap >> img;



	namedWindow(SetWindow, WINDOW_AUTOSIZE);
	//createTrackbar("radius", SetWindow, &radius, 50);
	createTrackbar("C1 Color", SetWindow,  &test.color_c1, 7);
	createTrackbar("C2 Color", SetWindow, &test.color_c2, 7);
	createTrackbar("Line Color", SetWindow, &test.color_l, 7);
	createTrackbar("Tri Color", SetWindow, &test.color_t, 7);
	createTrackbar("R", SetWindow, &test.r, 255);
	createTrackbar("G", SetWindow, &test.g, 255);
	createTrackbar("B", SetWindow, &test.b, 255);
	createTrackbar("Circle/two color/Fill/null", SetWindow, &test.circle_type, 4);
	createTrackbar("Line/Arrow/null", SetWindow, &test.line_type, 2);
	createTrackbar("Transparency", SetWindow, &test.max_alpha, 255);
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
	findHomo(frame1);
	for (int i = 0; i < 10; i++)
		calcHomograpy(selPoint[i], &wPoint[i]);

	//findHomo(str.size()-1);


	cout << "===============Select Mode==============" << endl << endl;
	cout << "1. Circle and Arrow (input 2 points) " << endl;
	cout << "2. 2 Circle , 1 Arrow (input 4 points) " << endl;
	cout << "3. 2 Circle , 1 Triangle (input 5 points) " << endl;
	int mode;
	//cin >> mode;
	mode = 1;




	while (1) {

		findHomo(0);

		////////////////////////////////////////
		/*	 Object Initialize	*/

		/*	Circle1	*/
		cir1.mCenter_ = wPoint[0];
		cir1.mColor_ = 
		cir1.mColor_spin_ = Circle::MINT;
		cir1.mCircle_type_ = Circle::CIRCLE_SPIN;
		cir1.mRadius_ = sh.mAbs_val_();
		cir1.mRadius_start = 0.5;
		cir1.mRadius_end = 0.7;
		cir1.mThickness_ = cir1.mRadius_ * 0.5;
		cir1.mAlpha_max_ = 7;

		/*	Circle2	*/
		cir2.mCenter_ = wPoint[1];
		cir2.mColor_ = Circle::OLIVE;
		cir2.mColor_spin_ = Circle::AQUAMARIN;
		cir2.mCircle_type_ = Circle::CIRCLE_DOTTED;
		cir2.mRadius_ = sh.mAbs_val_();
		cir2.mRadius_start = 0.7;
		cir1.mRadius_end = 0.9;
		cir2.mThickness_ = cir2.mRadius_ * 0.5;
		cir2.mAlpha_max_ = 6;


		/*	Triangle	*/
		tri.mWarpPoint_ = { Point(240, 413), Point(362, 708), Point(425, 709) };
		tri.mAlpha_max_ = 5;
		tri.mColor_ = Triangle::YELLOW;
		//tri.mThick_ = 3;


		/*	Arrow1	*/
		double line_thick = 0.4;

	
		//template의 circle center값과 arrow 값이 같을 경우 계산된 포인트를 넣어주어야함
		
		arr1.mStart_ = wPoint[0];
		arr1.mEnd_ = wPoint[1];

		if (cir1.mCenter_ == arr1.mStart_)
		{
			//arr1.mStart_ = arr1.mSetStart_;
			arr1.mSetStart_Flag_ = true;
		}
		if (cir2.mCenter_ == arr1.mEnd_)
		{
			arr1.mSetEnd_Flag_ = true;
			//arr1.mEnd_ = arr1.mSetEnd_;
		}



		arr1.mArrType_ = test.line_type;
		
		arr1.mThick_ = cir1.mRadius_ * line_thick;
		arr1.mColor_ = Arrow::GOLD;
		arr1.mAlpha_max_ = 7;
	

		/*	Arrow2	*/

		arr2.mStart_ = wPoint[4];
		if (cir2.mCenter_ == arr2.mStart_)
			arr2.mSetStart_Flag_ = true;
			//arr2.mStart_ = arr2.mSetStart_;
		arr2.mEnd_ = wPoint[5];
		arr2.mArrType_ = test.line_type;
		arr2.mThick_ = cir1.mRadius_ * line_thick;
		arr2.mColor_ = Arrow::GREEN;
		arr2.mAlpha_max_ = 8;

		/* 2 Circle and 1 Arrow */


		//////////////////////////////////////////////////////////


		/*if (img.empty())
		{
			printf("empty image");
			return 0;
		}*/
		//circle point 정의

		

		Mat img_selet = imread(str[str.size() - 1]);
		//p_img = imread(str[cnt]);
		//cout << "Click two points for Circle center" << endl << endl;
		showImg("Click two Points", frame_start, scale_view);
		setMouseCallback("Click two Points", onMouse);

		//Preview

		/////////////////////////////////////////////
		/*		test		*/
		switch (mode) {

		case 0:
			for (int cnt = 0; cnt < sh.mTime_ani_; cnt++)
			{
				//cir[0].alpha = 40;


				draw = Mat::zeros(img.size(), img.type());
				warp = Mat::zeros(img.size(), img.type());

				blur(draw, draw,Size(blur_size,blur_size));

				moveWindow("draw", 2200, 10);
				showImg("draw", draw, scale_view);
				warpPerspective(draw, warp, h_T[frame1], img.size());


				subtract(frame_start, warp, result);
				setMouseCallback(SetWindow, onMouse);

				imwrite("result/result1_" + to_string(cnt) + "_appear.jpg", result);
				showImg("Result", result, scale_view);

				waitKey(1);
			}
			waitKey(0);
			break;

		case 1:	//1. Circle and Arrow 
			//1. Drawing Appear

			for (int i = 0; i < sh.mTime_ani_; i++)
			{
				cir1.Appear(sh.mTime_ani_);
				cir2.Appear(sh.mTime_ani_);
				arr1.Appear(sh.mTime_ani_);
				cir1.SpinOn();
				cir2.SpinOn();
				cir1.expandRad(i, sh.mAbs_val_());
				cir2.expandRad(i, sh.mAbs_val_());

				draw = Mat::zeros(img.size(), img.type());
				warp = Mat::zeros(img.size(), img.type());

				cir1.Draw(draw);
				cir2.Draw(draw);

				blur(draw, draw,Size(blur_size,blur_size));

				moveWindow("draw", 2200, 10);
				showImg("draw", draw, scale_view);
				warpPerspective(draw, warp, h_T[frame1], img.size());

				subtract(frame_start, warp, result);
				setMouseCallback(SetWindow, onMouse);

				imwrite("result/result1_" + to_string(i) + "_appear.jpg", result);
				showImg("Result", result, scale_view);

				waitKey(1);

			}

			waitKey(1);


			//2. Arrow Appear
			for (int i = 0; i < sh.mTime_ani_; i++)
			{

				cir1.SpinOn();
				cir2.SpinOn();
				setMouseCallback("Result", onMouse);

				draw = Mat::zeros(img.size(), img.type());
				warp = Mat::zeros(img.size(), img.type());

				arr1.Drawing(&ani, i);

				cir1.Draw(draw);
				cir2.Draw(draw);
				arr1.Draw(draw, ani);
				blur(draw, draw,Size(blur_size,blur_size));

				showImg("draw", draw, scale_view);
				warpPerspective(draw, warp, h_T[frame1], img.size());

				subtract(frame_start, warp, result);

				setMouseCallback(SetWindow, onMouse);

				imwrite("result/result2_" + to_string(i) + "_arrow.jpg", result);
				showImg("Result", result, scale_view);
				waitKey(1);

			}
			waitKey(1);

			// 3. frame swipe
			for (int cnt = 0; cnt < str.size() - 2; cnt++) {

				p_img = imread(str[cnt]);
				findHomo(cnt);



				draw = Mat::zeros(img.size(), img.type());
				warp = Mat::zeros(img.size(), img.type());

				cir1.SpinOn();
				cir2.SpinOn();
				cir1.Draw(draw);
				cir2.Draw(draw);
				arr1.Draw(draw, arr1.mEnd_);

				blur(draw, draw,Size(blur_size,blur_size));

				warpPerspective(draw, warp, h_T[cnt], img.size());


				subtract(p_img, warp, result);
				setMouseCallback(SetWindow, onMouse);

				imwrite("result/result3_" + to_string(cnt) + "_swipe.jpg", result);
				showImg("Result", result, scale_view);

				waitKey(1);

			}

			//4. Drawing Disappear


			findHomo(str.size() - 1);
			for (int i = 0; i < sh.mTime_ani_; i++)
			{
				cir1.Disppear(sh.mTime_ani_);
				cir2.Disppear(sh.mTime_ani_);
				arr1.Disppear(sh.mTime_ani_);
				cir1.contracRad(i, sh.mAbs_val_());
				cir2.contracRad(i, sh.mAbs_val_());

				setMouseCallback("Result", onMouse);

				draw = Mat::zeros(img.size(), img.type());
				warp = Mat::zeros(img.size(), img.type());

				cir1.Draw(draw);
				cir2.Draw(draw);
				arr1.Draw(draw, arr1.mEnd_);
				//arr1.Draw(draw, arr1.EndX, arr1.EndY);

				blur(draw, draw,Size(blur_size,blur_size));

				warpPerspective(draw, warp, h_T[frame2], img.size());


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
			for (int i = 0; i < sh.mTime_ani_; i++)
			{
				cir1.expandRad(i,sh.mAbs_val_());
				cir2.expandRad(i,sh.mAbs_val_());
				cir1.Appear(sh.mTime_ani_);
				cir2.Appear(sh.mTime_ani_);
				arr1.Appear(sh.mTime_ani_);
				cir1.SpinOn();
				cir2.SpinOn();


				draw = Mat::zeros(img.size(), img.type());
				warp = Mat::zeros(img.size(), img.type());

				cir1.Draw(draw);
				cir2.Draw(draw);
				//arr1.Draw(draw, ani);
				blur(draw, draw,Size(blur_size,blur_size));

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




			// 2. frame swipe
			for (int cnt = 0; cnt < str.size() - 2; cnt++) {

				p_img = imread(str[cnt]);
				findHomo(cnt);


				draw = Mat::zeros(img.size(), img.type());
				warp = Mat::zeros(img.size(), img.type());
				cir1.SpinOn();
				cir2.SpinOn();
				arr1.Drawing(&ani, cnt);

				cir1.Draw(draw);
				cir2.Draw(draw);
				arr1.Draw(draw, ani);
		

				blur(draw, draw,Size(blur_size,blur_size));
				showImg("draw", draw, scale_view);
				warpPerspective(draw, warp, h_T[cnt], img.size());


				subtract(p_img, warp, result);
				setMouseCallback(SetWindow, onMouse);

				imwrite("result/result2_" + to_string(cnt) + "_swipe.jpg", result);
				showImg("Result", result, scale_view);

				waitKey(1);

			}






			//3. Arrow2 Appear
			findHomo(str.size() - 1);

			for (int i = 0; i < sh.mTime_ani_; i++)
			{
				arr2.Appear(sh.mTime_ani_);

				cir1.SpinOn();
				cir2.SpinOn();
				setMouseCallback("Result", onMouse);

				draw = Mat::zeros(img.size(), img.type());
				warp = Mat::zeros(img.size(), img.type());


				cir1.Draw(draw);
				cir2.Draw(draw);
				arr1.Draw(draw, arr1.mEnd_);

				arr2.Drawing(&ani2, i);
				arr2.Draw(draw, ani2);


				blur(draw, draw,Size(blur_size,blur_size));

				showImg("draw", draw, scale_view);
				warpPerspective(draw, warp, h_T[str.size() - 1], img.size());


				subtract(frame_end, warp, result);


				setMouseCallback(SetWindow, onMouse);

				imwrite("result/result3_" + to_string(i) + "_arrow.jpg", result);
				showImg("Result", result, scale_view);
				waitKey(1);


			}


			//4. Drawing Disappear
	

			findHomo(str.size() - 1);
			for (int i = 0; i < sh.mTime_ani_; i++)
			{

				cir1.Disppear(sh.mTime_ani_);
				cir2.Disppear(sh.mTime_ani_);
				arr1.Disppear(sh.mTime_ani_);
				arr2.Disppear(sh.mTime_ani_);


				setMouseCallback("Result", onMouse);

				draw = Mat::zeros(img.size(), img.type());
				warp = Mat::zeros(img.size(), img.type());

				cir1.Draw(draw);
				cir2.Draw(draw);
				arr1.Draw(draw, arr1.mEnd_);
				arr2.Draw(draw, arr2.mEnd_);


				blur(draw, draw,Size(blur_size,blur_size));

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
			for (int i = 0; i < sh.mTime_ani_; i++)
			{
				cir1.expandRad(i,sh.mAbs_val_());
				cir2.expandRad(i,sh.mAbs_val_());
				cir1.Appear(sh.mTime_ani_);
				cir2.Appear(sh.mTime_ani_);

				cir1.SpinOn();
				cir2.SpinOn();
	

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
			for (int i = 0; i < sh.mTime_ani_; i++)
			{

				tri.Appear(sh.mTime_ani_);
				cir1.SpinOn();
				cir2.SpinOn();
				//spin += 360 / sh.time_ani;
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


			// 3. frame swipe
			for (int cnt = 0; cnt < str.size() - 2; cnt++) {

				//SpinOn();

				cir1.SpinOn();
				cir2.SpinOn();
				p_img = imread(str[cnt]);
				findHomo(cnt);


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
			for (int i = 0; i < sh.mTime_ani_; i++)
			{
				cir1.SpinOn();
				cir2.SpinOn();
				tri.Disppear(sh.mTime_ani_);
				cir1.Disppear(sh.mTime_ani_);
				cir2.Disppear(sh.mTime_ani_);

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

	parseJson parse;
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


void calcHomograpy(Point2d point, Point2d* h_point)
{
	double dx = (h11 * point.x + h12 * point.y + h13) / (h31 * point.x + h32 * point.y + 1);
	double dy = (h21 * point.x + h22 * point.y + h23) / (h31 * point.x + h32 * point.y + 1);

	*h_point = Point2d(dx, dy);
}

void calcHomograpy_T(Point2d point, Point2d* h_point)
{
	double dx = (th11 * point.x + th12 * point.y + th13) / (th31 * point.x + th32 * point.y + 1);
	double dy = (th21 * point.x + th22 * point.y + th23) / (th31 * point.x + th32 * point.y + 1);

	*h_point = Point2d(dx, dy);
}


void showImg(string filename, Mat img, float factor)
{
	namedWindow(filename, 0);
	imshow(filename, img);
	resizeWindow(filename, img.cols * factor, img.rows * factor);
}




void Circle::Draw(Mat plane)
{

	int beta = 15; //beta는 360의 약수, 점선의 간격을 설정

	SetColor(mColor_spin_, &spin_rgb);
	SetColor(mColor_, &rgb);


	mThickness_ = 4;
	double hole = mRadius_ * (10 - 4) / 10.0;

	Point center = mCenter_;
	int rad = mRadius_;
	int spin = mSpin_;


	switch (mCircle_type_)
	{
	case CIRCLE_SPIN:
		ellipse(plane, Point(center), Size(rad, rad), 0, 0, 360, rgb, -1, LINE_AA);				//테두리
		ellipse(plane, Point(center), Size(rad + 2, rad + 2), spin - 30, 0, 60, Scalar(0, 0, 0), -1, LINE_AA);	//피자모양으로 자르기
		ellipse(plane, Point(center), Size(rad, rad), spin - 30, 0, 60, spin_rgb, -1, LINE_AA);		// 자른 영역에 다른색으로 채우기
		ellipse(plane, Point(center), Size(hole, hole), 0, 0, 360, Scalar(0, 0, 0), -1, LINE_AA);	//가운데 뚫어주기
		break;

	case CIRCLE_RING:
		ellipse(plane, Point(center), Size(rad, rad), 0, 0, 360, rgb, -1, LINE_AA);
		ellipse(plane, Point(center), Size(hole, hole), 0, 0, 360, Scalar(0, 0, 0), -1, LINE_AA);
		break;

	case CIRCLE_DOTTED:
		ellipse(plane, Point(center), Size(rad, rad), 0, 0, 360, rgb, -1, LINE_AA);
		for (int i = 0; i < 360 / beta; i++)
		{
			if (i % 2 == 0)
			{
				ellipse(plane, Point(center), Size(rad + 2, rad + 2), spin + beta * i, 0, beta * 0.6, Scalar(0, 0, 0), -1, LINE_AA);
			}
		}
		ellipse(plane, Point(center), Size(rad * 0.7, rad * 0.7), 0, 0, 360, Scalar(0, 0, 0), -1, LINE_AA);
		break;

	case CIRCLE_FILL:
		ellipse(plane, Point(center), Size(rad, rad), 0, 0, 360, rgb, -1, LINE_AA);
		break;

	default:
		break;
	}
}


void Arrow::CalcSetPos(Point p, Point* calcP)
{
	double x, y;
	x = p.x + mAbs_val_() * cos(angle);
	y = p.x + mAbs_val_() * sin(angle);
	*calcP = Point(x, y);
}

void Arrow::Draw(Mat plane, Point2d e)
{
	// drawing과 삼각형위치 계산을 위한 Angle 계산
	if (mStart_.x == mEnd_.x)
	{
		//rx = 0;
		if (mEnd_.y > mStart_.y)
			angle = CV_PI / 2.0;
		//ry = rad;
		else
			angle = CV_PI * (3.0 / 2.0);
		//	ry = -rad;
	}
	else {
		angle = atan(double(abs(mEnd_.y - mStart_.y)) / double(abs(mEnd_.x - mStart_.x)));
		if (mEnd_.x > mStart_.x && mEnd_.y >= mStart_.y)
		{

		}
		else if (mStart_.x > mEnd_.x && mEnd_.y >= mStart_.y)
		{
			angle = CV_PI - angle;
		}
		else if (mStart_.x > mEnd_.x && mStart_.y > mEnd_.y)
		{
			angle = CV_PI + angle;
		}
		else if (mEnd_.x > mStart_.x && mStart_.y > mEnd_.y)
		{
			angle = 2 * CV_PI - angle;
		}
	}

	SetColor(mColor_, &rgb);



	Point start, end;

	double radius = sh.mAbs_val_();

	mSetStart_.x = mStart_.x + radius * cos(angle);
	mSetStart_.y = mStart_.y + radius * sin(angle);

	mSetEnd_.x = e.x - radius * cos(angle);
	mSetEnd_.y = e.y - radius * sin(angle);

	if (mSetStart_Flag_)
		start = mSetStart_;
	else
		start = mStart_;

	if (mSetEnd_Flag_)
		end = mSetEnd_;
	else
		end = e;

	Point pts[3];
	double len;
	len = sqrt(pow(end.x - (radius / 2.0) * cos(angle) - start.x, 2) + pow(end.y - (radius / 2.0) * sin(angle) - start.y, 2));



	triEx = end.x;
	triEy = end.y;
	trip1x = triEx - radius * cos(angle + 30 * M_PI / 180);
	trip1y = triEy - radius * sin(angle + 30 * M_PI / 180);
	trip2x = triEx - radius * cos(angle - 30 * M_PI / 180);
	trip2y = triEy - radius * sin(angle - 30 * M_PI / 180);
	pts[0] = end;
	pts[1] = Point(trip1x, trip1y);
	pts[2] = Point(trip2x, trip2y);

	const Point* ppt[1] = { pts };
	int npt[] = { 3 };


	static double p = 20.0;
	double move_end_X = (sh.mAbs_val_() / 1.5) * cos(angle);
	double move_end_Y = (sh.mAbs_val_() / 1.5) * sin(angle);


	switch (mArrType_)
	{
	case static_cast<int>(Arrow::arrType::SOLID):


		line(plane, Point(start.x, start.y), Point(round(end.x - move_end_X), round(end.y - move_end_Y)), rgb, mThick_, LINE_AA);
		fillPoly(plane, ppt, npt, 1, rgb, LINE_AA);
		//line(plane, Point(mStart_.x, mStart_.y), Point(round(e.x), round(e.y )), rgb, mThick_ / 3.0, LINE_AA);
		//circle(plane, Point(e.x, e.y), 3, rgb, -1, 8);

		//cout << "ex: " << Point(round(e.x - move_end_X), round(e.y - move_end_Y)) << endl;
		//cout << "mex: " << Point2d( move_end_X,  move_end_Y) << endl;
		//cout << "end point: " << Point2d(e.x - move_end_X, e.y - move_end_Y) << endl;
		break;

	case static_cast<int>(Arrow::arrType::DOTTED_LONG):
		fillPoly(plane, ppt, npt, 1, rgb, LINE_AA);
		for (int i = 1; i < len / p; i++)
		{

			line(plane, Point(start.x + (i - 1) * p * cos(angle) + p * cos(angle) / 1.5,
				start.y + ((i - 1) * p * sin(angle)) + p * sin(angle) / 1.5),
				Point(start.x + (i * p * cos(angle)) - (sh.mAbs_val_() / 2.0) * cos(angle),
					start.y + (i * p * sin(angle)) - (sh.mAbs_val_() / 2.0) * sin(angle)), rgb, mThick_ * 0.8, LINE_AA);

		}
		break;

	case static_cast<int>(Arrow::arrType::DOTTED_SHORT):
		fillPoly(plane, ppt, npt, 1, rgb, LINE_AA);
		for (int i = 1; i < len / p + 1; i++)
		{
			circle(plane, Point(start.x + (i - 1) * p * cos(angle) + p * cos(angle) / 4.0, start.y + ((i - 1) * p * sin(angle)) + p * sin(angle) / 4.0), sh.mAbs_val_() / 5.0, rgb, -1, LINE_AA);
		}
		break;

	default:
		break;
	}
}

//void Arrow::Draw(Mat plane, double dx, double dy)
//void Arrow::Draw(Mat plane, Point2d e)
//{
//	// drawing과 삼각형위치 계산을 위한 Angle 계산
//	if (mStart_.x == mEnd_.x)
//	{
//		//rx = 0;
//		if (mEnd_.y > mStart_.y)
//			angle = CV_PI / 2.0;
//		//ry = rad;
//		else
//			angle = CV_PI * (3.0 / 2.0);
//		//	ry = -rad;
//	}
//	else {
//		angle = atan(double(abs(mEnd_.y - mStart_.y)) / double(abs(mEnd_.x - mStart_.x)));
//		if (mEnd_.x > mStart_.x && mEnd_.y >= mStart_.y)
//		{
//
//		}
//		else if (mStart_.x > mEnd_.x && mEnd_.y >= mStart_.y)
//		{
//			angle = CV_PI - angle;
//		}
//		else if (mStart_.x > mEnd_.x && mStart_.y > mEnd_.y)
//		{
//			angle = CV_PI + angle;
//		}
//		else if (mEnd_.x > mStart_.x && mStart_.y > mEnd_.y)
//		{
//			angle = 2 * CV_PI - angle;
//		}
//	}
//
//	SetColor(mColor_, &rgb);
//	
//	
//
//
//
//	double radius = sh.mAbs_val_();
//
//	mSetStart_ = mStart_;
//	mSetEnd_ = mEnd_;
//
//	if (mSetStart_Flag_)
//	{
//		 
//		mStart_.x = radius * cos(angle);
//		mStart_.y = radius * sin(angle);
//	}
//	if (mSetEnd_Flag_)
//	{
//		mEnd_.x -= radius * cos(angle);
//		mEnd_.y -= radius * sin(angle);
//	}
//	Point pts[3];
//	double len;
//	len = sqrt(pow(e.x - (radius / 2.0) * cos(angle) - mStart_.x, 2) + pow(e.y - (radius / 2.0) * sin(angle) - mStart_.y, 2));
//
//
//
//	triEx = e.x;
//	triEy = e.y;
//	trip1x = triEx - radius * cos(angle + 30 * M_PI / 180);
//	trip1y = triEy - radius * sin(angle + 30 * M_PI / 180);
//	trip2x = triEx - radius * cos(angle - 30 * M_PI / 180);
//	trip2y = triEy - radius * sin(angle - 30 * M_PI / 180);
//	pts[0] = e;
//	pts[1] = Point(trip1x, trip1y);
//	pts[2] = Point(trip2x, trip2y);
//
//	const Point* ppt[1] = { pts };
//	int npt[] = { 3 };
//
//	
//	static double p = 20.0;
//	double move_end_X = (sh.mAbs_val_() / 1.5) * cos(angle);
//	double move_end_Y = (sh.mAbs_val_() / 1.5) * sin(angle);
//
//
//	double k = atan2(e.y-mStart_.y, e.x-mStart_.x);
//	double j = atan2(e.y, e.x );
//	
//	switch (mArrType_)
//	{
//	case static_cast<int>(Arrow::arrType::SOLID):
//		
//
//		line(plane, Point(mStart_.x, mStart_.y), Point(round(e.x - move_end_X), round(e.y - move_end_Y)), rgb, mThick_, LINE_AA);
//		fillPoly(plane, ppt, npt, 1, rgb, LINE_AA);
//		//line(plane, Point(mStart_.x, mStart_.y), Point(round(e.x), round(e.y )), rgb, mThick_ / 3.0, LINE_AA);
//		//circle(plane, Point(e.x, e.y), 3, rgb, -1, 8);
//		
//		//cout << "ex: " << Point(round(e.x - move_end_X), round(e.y - move_end_Y)) << endl;
//		//cout << "mex: " << Point2d( move_end_X,  move_end_Y) << endl;
//		//cout << "end point: " << Point2d(e.x - move_end_X, e.y - move_end_Y) << endl;
//		break;
//
//	case static_cast<int>(Arrow::arrType::DOTTED_LONG):
//		fillPoly(plane, ppt, npt, 1, rgb, LINE_AA);
//		for (int i = 1; i < len/p; i++)
//		{
//
//			line(plane, Point(mStart_.x + (i - 1) * p * cos(angle) + p * cos(angle) / 1.5,
//				mStart_.y + ((i - 1) * p * sin(angle)) + p * sin(angle) / 1.5),
//				Point(mStart_.x + (i * p * cos(angle)) - (sh.mAbs_val_() / 2.0) * cos(angle),
//					mStart_.y + (i * p * sin(angle)) - (sh.mAbs_val_() / 2.0) * sin(angle)), rgb, mThick_ * 0.8, LINE_AA);
//
//		}
//		break;
//
//	case static_cast<int>(Arrow::arrType::DOTTED_SHORT):
//		fillPoly(plane, ppt, npt, 1, rgb, LINE_AA);
//		for (int i = 1; i <len/p+1; i++)
//		{
//			circle(plane, Point(mStart_.x + (i - 1) * p * cos(angle) +p * cos(angle) / 4.0, mStart_.y + ((i - 1) * p * sin(angle)) + p * sin(angle) / 4.0), sh.mAbs_val_()/5.0, rgb, -1, LINE_AA);
//		}
//		break;
//
//	default:
//		break;
//	}
//}


//draw 3d calib ROI 
void Triangle::Draw(Mat dst)
{
	SetColor(mColor_, &rgb);

	Point pts[3];

	pts[0] = mWarpPoint_[0];
	pts[1] = mWarpPoint_[1];
	pts[2] = mWarpPoint_[2];

	const Point* ppt[1] = { pts };
	int npt[] = { 3 };

	fillPoly(dst, ppt, npt, 1, rgb, 8);

}


void Arrow::Drawing(Point2d* p, int i)
{
	double dx, dy;
	double time = sh.mTime_ani_;
	//dx = double(mStart_.x + (mEnd_.x - mStart_.x) / pow(time, 2) * pow(i, 2));
	//dy = double(mStart_.y + (mEnd_.y - mStart_.y) / pow(time, 2) * pow(i, 2));
	double d = 0.1;
	dy = double(mStart_.y + (mEnd_.y - mStart_.y) * ((1.0 - d) * pow((i+1) / time, 2) + d));
	dx = double(mStart_.x + (mEnd_.x - mStart_.x) * ((1.0 - d) * pow((i+1) / time, 2) + d));
	//dx = double(mStart_.x + (mEnd_.x - mStart_.x) * double((i+1) / time));
	//dy = double(mStart_.y + (mEnd_.y - mStart_.y) * double((i+1) / time));
	//cout << "i/t " << double(i / time) << endl;

	//dx = round(double(mStart_.x + (mEnd_.x - mStart_.x) / pow(sh.mTime_ani_/(i + 1), 1.0 / 2.5)));
	//dy = round(double(mStart_.y + (mEnd_.y - mStart_.y) / pow(sh.mTime_ani_/(i + 1), 1.0 / 2.5)));

	*p = Point2d(dx, dy);
}


void Eventclass::HandleEvent(int evt, int x, int y, int flags)
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
	Eventclass* p = (Eventclass*)param;
	p->HandleEvent(evt, x, y, flags);
}

