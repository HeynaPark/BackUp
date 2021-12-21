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
//string pts_path = "1.pts";		//pts file
//String path("1/*.jpg");
string pts_path = "2.pts";		//pts file
String path("2/*.jpg");
vector<String> str;


// H Matrix 
int frame_num = str.size();
int num = 20;
Mat h[20] = {};					//Homograpy Matrix	(3d -> 2d)		
Mat h_T[20] = {};				//Homograpy Matrix	(2d -> 3d)
double wX[4] = {};
double wY[4] = {};
double pX[20][4] = { {},{},{},{} };		// dsc 개수만큼 배열 생성
double pY[20][4] = { {},{},{},{} };
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

	int line_type = 1;			// 1.solid	2.dotted(1)	3.dotted(2)
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
	int mAbs_val_(int cm);
	int mAlpha_;
	int mAlpha_max_;
	int r, g, b;


	int mTime_ani_ = 30;
	enum COLOR { CUSTOM, RED, ORANGE, YELLOW, GREEN, BLUE, CYAN, BLACK,
		GOLD, OLIVE, AQUAMARIN, SKYBLUE, VYOLET, PINK, MINT, BRIKRED, BROUN
	};
	/*int mColor_;
	int mColor_spin;*/
	COLOR mColor_;
	COLOR mColor_spin_;

	void SetColor(COLOR c, Scalar* color);
	void Appear(int t) { mAlpha_ += (mAlpha_max_ * 255.0 / 100.0) / t; } //percent로 변환
	void Appear() { mAlpha_ += mAlpha_max_; }
	void Disppear(int t) { mAlpha_ -= (mAlpha_max_ * 255.0/100.0) / t; }


};




int Shape::mAbs_val_(int cm)
{
	parseJson parse;
	parse.parsing_json();



	for (int i = 0; i < 4; i++) {
		wX[i] = parse.wpointX[i];
		wY[i] = parse.wpointY[i];
	}
	double widPixel = (abs(wX[1] - wX[0]) + abs(wX[2] - wX[3])) * 0.5;
	double heiPixel = (abs(wY[2] - wY[1]) + abs(wY[3] - wY[0])) * 0.5;

	
	const double widReal = 13500;	//	(millimeter)
	const double heiReal = 3300;
	const double radiusReal = cm*10.0; // 0.5M

	int radius1, radius2;
	radius1 = double(radiusReal * widPixel) / widReal;
	radius2 = double(radiusReal * heiPixel) / heiReal;


	return radius1*2;
	//return  radius1 + radius2;
	
}
 

class Circle :public Shape {


	Scalar spin_rgb;
public:
	Point mCenter_;

	int mSpin_;
	double mRadius_;
	double mRadius_start;
	int mRadius_end;
	
	int mThickness_;

	bool mCir_Bigger_Flag_;
	bool mCir_Smaller_Flag_;

	enum cirType { CIRCLE_FILL, CIRCLE_RING, CIRCLE_DOTTED, CIRCLE_SPIN };
	cirType mCircle_type_;

	void Draw(Mat dst);
	
	void expandRad(int t, int r) { mRadius_ = ((1.0 - mRadius_start) * t / mTime_ani_ + mRadius_start) * r; }
	void contracRad(int t, int r) { mRadius_ = ((mRadius_end - 1.0) * t / mTime_ani_ + 1.0) * r; }

};



class Arrow :public Shape
{

private:
	
	double triEx, triEy;					//삼각형 끝 포인트
	double trip1x, trip1y, trip2x, trip2y;	//삼각형 나머지 두 포인트
	Point tip_start, tip_end;
	Point line_start, line_end;
public:
	double angle;

	Point mStart_, mEnd_;
	Point mSetStart_, mSetEnd_;
	Point mAniEnd_;
	bool mSetStart_Flag_, mSetEnd_Flag_;
	bool mLinedraw_Flag_;
	

	int mThick_;
	enum class arrType { SOLID, DOTTED_LONG, DOTTED_SHORT };
	int mArrType_;
	void Draw(Mat plane, int t);
	void CalcSetPos(Point p, Point* calcP);
	//void Drawline(int t);
};



class Triangle :public Shape {

public:

	vector<Point> mPoint_;
	vector<Point> mWarpPoint_;

	void Draw(Mat plane);

};


class Animation {

};



void showImg(string filename, Mat img, float factor);
void onMouse(int evt, int x, int y, int flags, void* param);
void calcHomograpy(Point2d point, Point2d* h_point);
void calcHomograpy_T(Point2d point, Point2d* h_point);
void findHomo(int n);


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




void initialize()
{


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
		r = 255, g = 50, b = 55;
		break;
	case ORANGE:
		r = 255, g = 150, b = 50;
		break;
	case YELLOW:
		r = 255, g = 250, b = 50;
		break;
	case GREEN:
		r = 55, g = 250, b = 50;
		break;
	case BLUE:
		r = 55, g = 50, b = 250;
		break;
	case CYAN:
		r = 55, g = 250, b = 250;
		break;
	case BLACK:
		r = 55, g = 55, b = 55;
		break;
	case AQUAMARIN:
		r = 127, g = 255, b = 212;
		break;
	case GOLD:
		r = 255, g = 215, b = 0;
		break;
	case OLIVE:
		r = 85, g = 107, b = 47;
		break;
	case SKYBLUE:
		r = 135, g = 206, b = 250;
		break;
	case MINT:
		r = 0, g = 206, b = 209;
		break;
	case VYOLET:
		r = 152, g = 85, b = 211;
		break;
	case PINK:
		r = 255, g = 182, b = 203;
		break;
	}
	//case CUSTOM:
	//	//rgb = Scalar((255 - b) * alpha / alpha_max, (255 - g) * alpha / alpha_max, (255 - r) * alpha / alpha_max);				//custom
	//	rgb = Scalar((255 - b) / 255.0 * alpha, (255 - g) / 255.0 * alpha, (255 - r) / 255.0 * alpha);
	//	break;
	//case RED:
	//	rgb = Scalar(alpha, alpha, 0);		//red
	//	break;
	//case ORANGE:
	//	rgb = Scalar(alpha, alpha / 2, 0);		//orange
	//	break;
	//case YELLOW:
	//	rgb = Scalar(alpha, 0, 0);			//yellow
	//	break;
	//case GREEN:
	//	rgb = Scalar(alpha, 0, alpha);		//green
	//	break;
	//case BLUE:
	//	rgb = Scalar(0, alpha, alpha);		//blue
	//	break;
	//case CYAN:
	//	rgb = Scalar(0, 0, alpha);			//cyan
	//	break;
	//case BLACK:
	//	rgb = Scalar(alpha, alpha, alpha);	//black
	//	break;

	//case AQUAMARIN:
	//	r = 127, g =255, b=212;
	//	rgb = Scalar((255 - b) / 255.0 * alpha, (255 - g) / 255.0 * alpha, (255 - r) / 255.0 * alpha);
	//	break;
	//case GOLD:
	//	r = 255,g = 215, b=0;
	//	rgb = Scalar((255 - b) / 255.0 * alpha, (255 - g) / 255.0 * alpha, (255 - r) / 255.0 * alpha);
	//	break;
	//case OLIVE:
	//	r = 85, g =107 , b =47;
	//	rgb = Scalar((255 - b) / 255.0 * alpha, (255 - g) / 255.0 * alpha, (255 - r) / 255.0 * alpha);
	//	break;
	//case SKYBLUE:
	//	r = 135, g = 206, b = 250;
	//	rgb = Scalar((255 - b) / 255.0 * alpha, (255 - g) / 255.0 * alpha, (255 - r) / 255.0 * alpha);
	//	break;
	//case MINT:
	//	r = 0, g = 206, b = 209;
	//	rgb = Scalar((255 - b) / 255.0 * alpha, (255 - g) / 255.0 * alpha, (255 - r) / 255.0 * alpha);
	//	break;
	//case VYOLET:
	//	r = 152, g = 85, b = 211;
	//	rgb = Scalar((255 - b) / 255.0 * alpha, (255 - g) / 255.0 * alpha, (255 - r) / 255.0 * alpha);
	//	break;
	//case PINK:
	//	r = 255, g = 182 ,b = 203;
	//	rgb = Scalar((255 - b) / 255.0 * alpha, (255 - g) / 255.0 * alpha, (255 - r) / 255.0 * alpha);
	//	break;
	//}
	rgb = Scalar((255 - b) / 255.0 * alpha, (255 - g) / 255.0 * alpha, (255 - r) / 255.0 * alpha);
	*color = rgb;
}




int main(int ac, char** av)
{


	glob(path, str, false);

	selTime[0] = 0; // 첫 프레임
	selTime[1] = str.size() - 1;	// 마지막 프레임

	int frame1 = 0;
	int frame2 = selTime[1];
	Mat roi, img_warp, frame_end, frame_start;

	//frame for draw
	frame_start = imread(str[frame1]);
	frame_end = imread(str[frame2]);

	if (str.size() == 0)
		cout << "image non exist. \n" << endl;




	Mat img = imread(str[frame1]);
	Mat plane(img.size(), img.type(), Scalar(50, 130, 55));
	Mat draw = Mat::zeros(plane.size(), plane.type());		//2d draw
	Mat draw_n;
	Mat drawimg = Mat::zeros(img.size(), img.type());
	warp = Mat::zeros(img.size(), img.type());
	Mat result = Mat::zeros(img.size(), img.type());
	
	int blur_size = 2;
	//Mat img = imread("1/000136.jpg");
	Mat p_img;
	clock_t start, end, start_1, end_1, start_2, end_2, start_3, end_3;

	

	struct CirclePARAM c1;
	//c1 = getParameter();


	
	//frame_end = imread(str[frame2]);
	//frame_end = imread(str[str.size() - 2]);

	//selPoint[0] = Point(624, 812);	//cir1
	//selPoint[1] = Point(405, 613);	//cir2
		
	//selPoint[0] = Point(2428, 885);	//cir1
	//selPoint[1] = Point(1256, 1546);	//cir2

	selPoint[0] = Point(1113, 479);	//cir1
	selPoint[1] = Point(579, 367);	//cir2

	selPoint[2] = Point(720.0, 782.0);	//arr1.s 
	selPoint[3] = Point(520.0, 596.0);	//arr1.e
					   
	selPoint[4] = Point(519, 593);	//arr2.s
	selPoint[5] = Point(785, 644);	//arr2.e
	//selPoint[5] = Point(1137, 536);	//arr2.e
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
	mode = 2;




	while (1) {

		findHomo(0);

		////////////////////////////////////////
		/*	 Object Initialize	*/


		double abs_value = sh.mAbs_val_(50);
		double abs_arr = sh.mAbs_val_(50);
		/*	Circle1	*/
		cir1.mCenter_ = wPoint[0];
		cir1.mColor_ = Circle::VYOLET;
		//cir1.rgb = Scalar(23, 71, 211);
		cir1.mColor_spin_ = Circle::SKYBLUE;
		cir1.mCircle_type_ = Circle::CIRCLE_SPIN;
		cir1.mRadius_ = abs_value;
		cout << "radius " << cir1.mRadius_ << endl;
		cir1.mRadius_start = 0.5;
		cir1.mRadius_end = 0.7;
		cir1.mThickness_ = cir1.mRadius_ * 0.5;
		cir1.mAlpha_max_ = 100;

		/*	Circle2	*/
		cir2.mCenter_ = wPoint[1];
		cir2.mColor_ = Circle::CYAN;
		cir2.mColor_spin_ = Circle::AQUAMARIN;
		cir2.mCircle_type_ = Circle::CIRCLE_SPIN;
		cir2.mRadius_ = abs_value;
		cir2.mRadius_start = 0.5;
		cir1.mRadius_end = 0.5;
		cir2.mThickness_ = cir2.mRadius_ * 0.5;
		cir2.mAlpha_max_ = 100;


		/*	Triangle	*/
		tri.mWarpPoint_ = { Point(240, 413), Point(362, 708), Point(425, 709) };
		tri.mAlpha_max_ = 5;
		tri.mColor_ = Triangle::YELLOW;
		//tri.mThick_ = 3;


		/*	Arrow1	*/
		double line_thick = 0.4;
		/*arr1.mStart_ = wPoint[0];
		arr1.mEnd_ = wPoint[1];*/
		arr1.mStart_ = wPoint[0];
		arr1.mEnd_ = wPoint[1];
		if (cir1.mCenter_ == arr1.mStart_)
			arr1.mSetStart_Flag_ = true;
		if (cir2.mCenter_ == arr1.mEnd_)
			arr1.mSetEnd_Flag_ = true;

		arr1.mArrType_ = 0;
		arr1.mThick_ = abs_arr * line_thick;
		arr1.mColor_ = Arrow::GOLD;
		arr1.mAlpha_max_ = 100;
	

		/*	Arrow2	*/

		arr2.mStart_ = wPoint[1];
		if (cir2.mCenter_ == arr2.mStart_)
			arr2.mSetStart_Flag_ = true;
		arr2.mEnd_ = wPoint[6];
		arr2.mArrType_ = 2;
		arr2.mThick_ = cir1.mRadius_; 
		arr2.mColor_ = Arrow::OLIVE;
		arr2.mAlpha_max_ = 60;

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
		int frame_input;
		Point2d warpclick,pos_warp;
	
	/*	while (1)
		{
			showImg("Click two Points", frame_start, scale_view);
			waitKey(0);
			setMouseCallback("Click two Points", onMouse);
			cout << "Frame number is " << frame1 << endl;
			cout << "click pos is (" << fx << "," << fy << ")" << endl;
			cout << "This pos warp to frame : " << endl;
			cin >> frame_input;
			findHomo(frame1);
			calcHomograpy(Point(fx, fy), &warpclick);
			findHomo(frame_input);
			calcHomograpy_T(warpclick, &pos_warp);
			cout << "Warp pos is (" << round(pos_warp.x) << "," << round(pos_warp.y) << ")" << "in " << frame_input << endl;
			waitKey(0);
		}*/
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
			
				cir1.mCir_Bigger_Flag_ = true;
				cir2.mCir_Bigger_Flag_ = true;
		
				//cir1.expandRad(i, sh.mAbs_val_());
				//cir2.expandRad(i, sh.mAbs_val_());

				draw = Mat::zeros(img.size(), img.type());
				warp = Mat::zeros(img.size(), img.type());


				//arr1.Drawline(i);
				cir1.Draw(draw);
				cir2.Draw(draw);
				arr1.Draw(draw, i);

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


			////2. Arrow Appear
			//for (int i = 0; i < sh.mTime_ani_; i++)
			//{

			//	cir1.SpinOn();
			//	cir2.SpinOn();
			//	cir1.mCir_Bigger_Flag_ = false;
			//	cir2.mCir_Bigger_Flag_ = false;
			//	arr1.mLinedraw_Flag_=true;
			//	

			//	setMouseCallback("Result", onMouse);

			//	draw = Mat::zeros(img.size(), img.type());
			//	warp = Mat::zeros(img.size(), img.type());

			//	//arr1.Drawing(&ani, i);


			//	cir1.Draw(draw,i);
			//	cir2.Draw(draw,i);
			//	arr1.Draw(draw, i);
			//	blur(draw, draw,Size(blur_size,blur_size));

			//	showImg("draw", draw, scale_view);
			//	warpPerspective(draw, warp, h_T[frame1], img.size());

			//	subtract(frame_start, warp, result);

			//	setMouseCallback(SetWindow, onMouse);

			//	imwrite("result/result2_" + to_string(i) + "_arrow.jpg", result);
			//	showImg("Result", result, scale_view);
			//	waitKey(1);

			//}
			//waitKey(1);

			// 3. frame swipe
			for (int cnt = 1; cnt < str.size() - 2; cnt++) {

				p_img = imread(str[cnt]);
				findHomo(cnt);
				arr1.mLinedraw_Flag_ = false;
				cir1.mCir_Bigger_Flag_ = false;
				cir2.mCir_Bigger_Flag_ = false;


				draw = Mat::zeros(img.size(), img.type());
				warp = Mat::zeros(img.size(), img.type());

	
				cir1.Draw(draw);
				cir2.Draw(draw);
				arr1.Draw(draw,cnt);

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
			/*	cir1.contracRad(i, sh.mAbs_val_());
				cir2.contracRad(i, sh.mAbs_val_());*/

				setMouseCallback("Result", onMouse);

				draw = Mat::zeros(img.size(), img.type());
				warp = Mat::zeros(img.size(), img.type());

				cir1.Draw(draw);
				cir2.Draw(draw);
				arr1.Draw(draw, i);
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
				cir1.expandRad(i, abs_value);
				//cir2.expandRad(i, abs_value);
				cir1.Appear(sh.mTime_ani_);
				//cir2.Appear(sh.mTime_ani_);
				cir2.Appear();
				arr1.Appear(sh.mTime_ani_);
				arr1.mLinedraw_Flag_ = true;


				draw = Mat::zeros(img.size(), img.type());
				warp = Mat::zeros(img.size(), img.type());

				cir1.Draw(draw);
				cir2.Draw(draw);
				arr1.Draw(draw,i);
				blur(draw, draw,Size(blur_size,blur_size));

				//moveWindow("draw", 2200, 10);
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
			for (int cnt = 2; cnt < str.size() - 2; cnt++) {


				p_img = imread(str[cnt]);
				findHomo(cnt);
				arr1.mLinedraw_Flag_ = false;

				draw = Mat::zeros(img.size(), img.type());
				warp = Mat::zeros(img.size(), img.type());
		
				cir1.Draw(draw);
				cir2.Draw(draw);
				arr1.Draw(draw,8);
		

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
				arr1.mLinedraw_Flag_ = false;

				setMouseCallback("Result", onMouse);

				draw = Mat::zeros(img.size(), img.type());
				warp = Mat::zeros(img.size(), img.type());


				cir1.Draw(draw);
				cir2.Draw(draw);
				arr1.Draw(draw,i);

				arr2.mLinedraw_Flag_ = true;
				
				arr2.Draw(draw,i);


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
				//cir1.mCir_Smaller_Flag_ = true;
				arr1.mLinedraw_Flag_ = false;
				arr2.mLinedraw_Flag_ = false;

				cir1.Disppear(sh.mTime_ani_);
				cir2.Disppear(sh.mTime_ani_);
				arr1.Disppear(sh.mTime_ani_);
				arr2.Disppear(sh.mTime_ani_);
				

				setMouseCallback("Result", onMouse);

				draw = Mat::zeros(img.size(), img.type());
				warp = Mat::zeros(img.size(), img.type());

				cir1.Draw(draw);
				cir2.Draw(draw);
				arr1.Draw(draw,i);
				arr2.Draw(draw,i);


				blur(draw, draw,Size(blur_size,blur_size));
				showImg("draw", draw, scale_view);
				warpPerspective(draw, warp, h_T[str.size() - 1], img.size());


				subtract(frame_end, warp, result);
				setMouseCallback(SetWindow, onMouse);

				imwrite("result/result4_" + to_string(i) + "_disappear.jpg", result);
				showImg("Result", result, scale_view);
				waitKey(1);

			}

			cir1.mCir_Smaller_Flag_ = false;

			break;

		case 3:	// 3. 2 Circle , 1 Triangle (input 5 points)
				//1. Drawing Appear
			for (int i = 0; i < sh.mTime_ani_; i++)
			{
				cir1.expandRad(i,cir1.mRadius_);
				cir2.expandRad(i,cir1.mRadius_);
				cir1.Appear(sh.mTime_ani_);
				cir2.Appear(sh.mTime_ani_);


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


	//mThickness_ = 4;
	

	Point center = mCenter_;
	double rad;
	int spin = mSpin_;
	mSpin_ += 360 / mTime_ani_;

	//if (mCir_Bigger_Flag_)
	//	rad = mRadius_ * (0.5*t/mTime_ani_ +0.5);
	//	//rad = double((1.0 - mRadius_start) * (t / mTime_ani_) + mRadius_start) * mRadius_;
	//else if (mCir_Smaller_Flag_)
	//	rad = ((mRadius_end - 1.0) * t / mTime_ani_ + 1.0) * mRadius_;
	//else
	rad = mRadius_;
	
	cout << "mrad " <<mRadius_<< endl;

	double hole = rad * (10 - 4) / 10.0;

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


void Arrow::Draw(Mat plane, int t)
{
	double time = sh.mTime_ani_;
	double d = 0;

	Point aniPoint;
	Point tip_start, tip_end;
	Point line_start, line_end;

	double radius = mAbs_val_(50);
	static double p = 25.0;
	static double dot = 20.0;
	double thick = mThick_ * 0.4;

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



	if (mSetStart_Flag_)
	{
		line_start.x = mStart_.x + radius *1.2* cos(angle);
		line_start.y = mStart_.y + radius *1.2* sin(angle);
		tip_start.x = line_start.x + radius * (sqrt(3) / 2.0) * cos(angle) ;
		tip_start.y = line_start.y + radius * (sqrt(3) / 2.0) * sin(angle) ;
	}
	else
	{
		line_start.x = mStart_.x;
		line_start.y = mStart_.y;
		tip_start.x = line_start.x + radius * (sqrt(3) / 2.0) * cos(angle);
		tip_start.y = line_start.y + radius * (sqrt(3) / 2.0) * sin(angle);
	}

	if (mSetEnd_Flag_)
	{
		if (mLinedraw_Flag_ == true)
		{
			tip_end.x = mEnd_.x - radius * cos(angle);
			tip_end.y = mEnd_.y - radius * sin(angle);
			aniPoint.x = double((tip_start.x + (tip_end.x - tip_start.x) * ((1.0 - d) * pow((t + 1) / time, 2) + d)));
			aniPoint.y = double((tip_start.y + (tip_end.y - tip_start.y) * ((1.0 - d) * pow((t + 1) / time, 2) + d)));
			tip_end = aniPoint;
		}
		else
		{
			tip_end.x = mEnd_.x - radius * cos(angle);
			tip_end.y = mEnd_.y - radius * sin(angle);

		}
		line_end.x = tip_end.x - radius * (sqrt(3) / 2.0) * cos(angle);
		line_end.y = tip_end.y - radius * (sqrt(3) / 2.0) * sin(angle);
	}
	else
	{
		if (mLinedraw_Flag_ == true)
		{
			aniPoint.x = double((tip_start.x + (mEnd_.x - tip_start.x) * ((1.0 - d) * pow((t + 1) / time, 2) + d)));
			aniPoint.y = double((tip_start.y + (mEnd_.y - tip_start.y) * ((1.0 - d) * pow((t + 1) / time, 2) + d)));
			tip_end = aniPoint;
		}
		else
		{
			tip_end.x = mEnd_.x;
			tip_end.y = mEnd_.y;

		}
		line_end.x = tip_end.x - radius * (sqrt(3) / 2.0) * cos(angle);
		line_end.y = tip_end.y - radius * (sqrt(3) / 2.0) * sin(angle);
	}



	Point pts[3];
	double len;
	len = sqrt(pow(line_end.x - line_start.x, 2) + pow(line_end.y - line_start.y, 2));

	int tip_angle = 30;
	//arrow tip position
	triEx = tip_end.x;
	triEy = tip_end.y;
	trip1x = triEx - radius * cos(angle + tip_angle * M_PI / 180);
	trip1y = triEy - radius * sin(angle + tip_angle * M_PI / 180);
	trip2x = triEx - radius * cos(angle - tip_angle * M_PI / 180);
	trip2y = triEy - radius * sin(angle - tip_angle * M_PI / 180);
	pts[0] = Point(triEx, triEy);
	pts[1] = Point(trip1x, trip1y);
	pts[2] = Point(trip2x, trip2y);
	const Point* ppt[1] = { pts };
	int npt[] = { 3 };
	double move_end_X = (sh.mAbs_val_(50) / 1.5) * cos(angle);
	double move_end_Y = (sh.mAbs_val_(50) / 1.5) * sin(angle);



	switch (mArrType_)
	{
	case static_cast<int>(Arrow::arrType::SOLID):


		line(plane, line_start, line_end, rgb, mThick_, LINE_AA);
		fillPoly(plane, ppt, npt, 1, rgb, LINE_AA);
		break;

	case static_cast<int>(Arrow::arrType::DOTTED_LONG):
		fillPoly(plane, ppt, npt, 1, rgb, LINE_AA);
		for (int i = 1; i < len / p; i++)
		{
			Point dotLong_start, dotLong_end;
			dotLong_start.x = line_start.x + i * p * cos(angle);
			dotLong_start.y = line_start.y + i * p * sin(angle);
			dotLong_end.x = line_start.x + i * p * cos(angle) - (radius / 2.0) * cos(angle) * 15.0 / p;
			dotLong_end.y = line_start.y + i * p * sin(angle) - (radius / 2.0) * sin(angle) * 15.0 / p;
			line(plane, dotLong_start, dotLong_end, rgb, thick * 0.8, LINE_AA);

			/*	line(plane, Point(start.x + (i - 1) * p * cos(angle) + p * cos(angle) / 1.5,
					start.y + ((i - 1) * p * sin(angle)) + p * sin(angle) / 1.5),
					Point(start.x + (i * p * cos(angle)) - (sh.mAbs_val_() / 2.0) * cos(angle),
						start.y + (i * p * sin(angle)) - (sh.mAbs_val_() / 2.0) * sin(angle)), rgb, mThick_ * 0.8, LINE_AA);*/

		}
		break;

	case static_cast<int>(Arrow::arrType::DOTTED_SHORT):
		fillPoly(plane, ppt, npt, 1, rgb, LINE_AA);
		for (int i = 1; i < len / dot ; i++)
		{
			circle(plane, Point(line_start.x + (i * dot * cos(angle)), line_start.y + (i * dot * sin(angle))), radius / 5.0, rgb, -1, LINE_AA);
		}
		break;

	default:
		break;
	}
}

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

