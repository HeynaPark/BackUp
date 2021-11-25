#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <time.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>
#include <fstream>
#include "json/json.h"

#pragma comment(lib, "jsoncpp.lib")


#define FILE_NAME "1/000100.jpg"


using namespace cv;
using namespace std;

string pts_path = "1.pts";

double wpointX[4] = {};
double wpointY[4] = {};
double pointX[19][4] = { {},{},{},{} };
double pointY[19][4] = { {},{},{},{} };
Mat h[19] = {};				//Homograpy Matrix	(3d -> 2d)
Mat h_T[19] = {};			//Homograpy Matrix	(2d -> 3d)
Point p1, p2, p3, wp1, wp2, wp3;

//int num = 19;
//string dsc[19] = {};
String path("1/*.jpg");
vector<String> str;
//int size_i = str.size();



Mat flag = imread("korea.png");
Mat inner = imread("tae300.png");
Mat frame;
int fps = 30;
Mat warp;
//Mat img = imread("000126.jpg");

//Mat img;
VideoCapture cap(FILE_NAME);
//int fps = (int)cap.get(CAP_PROP_FPS);
String SetWindow = "Settings";

void showImg(string filename, Mat img, float factor);
void drawCircle(Mat plane, struct Circle2D c);
void drawLine(Mat plane, double x, double y, double dx, double dy, double factor);
void drawPlane(Mat dst);
void onMouse(int evt, int x, int y, int flags, void* param);
void calcHomograpy(double x, double y, double* dx, double* dy);
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


double h11, h12, h13, h21, h22, h23, h31, h32;
double th11, th12, th13, th21, th22, th23, th31, th32;

int fx, fy;
int fx_e, fy_e;
int fx2, fy2;
int fx2_e, fy2_e;
double hx1, hy1, hx2, hy2;

struct transP {
	double hfx;
	double hfy;
};

struct Circle2D {
	Scalar color;
	int x, y, ex, ey;	//start, end point
};

struct  warpPoint
{
	double x, y;
};
struct planePoint {
	Point p[3];

};

int rad = 27;		//radius
double radius;
int rad1, rad2;
int rad_a;
int alpha = 0;	
int max_a;			// transparency (range: 1~10)
int thick = 23;
int spin = 0;		//spin circle parameter
double angle;

//color
int r = 123, g = 220, b = 220;
int color_c1 = 0;
int color_c2 = 3;
int color_l = 4;
Scalar rgb1;
Scalar rgb2;
Scalar rgb;


//Scalar red(200, 200, 20);
//Scalar orange(alpha, 50, 20);
//Scalar yellow(255, 0, 0);
//Scalar green(alpha, 40, alpha);
//Scalar blue(30, 90, alpha);
//Scalar white(30, 30, 30);
//Scalar black(100, 100, 100);

double scale = 1;			//roi size
float scale_view = 1;		//image scale

//line type
int line_off = 1;
int arrow_tip = 7;			//if tip = 0 , result is line
int circle_type = 3;

double lineEndx, lineEndy;

class eventclass
{
public:
	void HandleEvent(int evt, int x, int y, int flags);
};


//double* pointX = (double*)malloc(sizeof(int*) * size_i);
void parsing_json() {

	/*double** pointX;
	pointX = new double* [size_i];
	for (int i = 0; i < size_i; i++)
	{
		pointX[i] = new double[4];
	}
	for (int i = 0; i < size_i; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			pointX[i][j] = { };
		}
	}*/

	ifstream json_file;
	json_file.open(pts_path);

	Json::Value root;
	json_file >> root;


	//cout<<root["points"].asString()<<endl;
   // cout << root["world_coords"] << endl;

	Json::Value world_pts = root["world_coords"];
	wpointX[0] = world_pts["X1"].asDouble();
	wpointX[1] = world_pts["X2"].asDouble();
	wpointX[2] = world_pts["X3"].asDouble();
	wpointX[3] = world_pts["X4"].asDouble();
	wpointY[0] = world_pts["Y1"].asDouble();
	wpointY[1] = world_pts["Y2"].asDouble();
	wpointY[2] = world_pts["Y3"].asDouble();
	wpointY[3] = world_pts["Y4"].asDouble();

	/*for (int i = 0; i < 4; i++)
	{
		cout << "world X" << i + 1 << " " << wpointX[i] << endl;
		cout << "world Y" << i + 1 << " " << wpointY[i] << endl;
	}*/


	Json::Value pts = root["points"];
	for (int i = 0; i < pts.size(); i++)
	{
		// dsc[i] = pts[i]["dsc_id"].asString();
		// string strDsc = pts[i]["dsc_id"].asString();
		// dsc[i] = pts[i]["dsc_id"].asString();
		pointX[i][0] = pts[i]["pts_3d"]["X1"].asDouble();
		pointX[i][1] = pts[i]["pts_3d"]["X2"].asDouble();
		pointX[i][2] = pts[i]["pts_3d"]["X3"].asDouble();
		pointX[i][3] = pts[i]["pts_3d"]["X4"].asDouble();

		pointY[i][0] = pts[i]["pts_3d"]["Y1"].asDouble();
		pointY[i][1] = pts[i]["pts_3d"]["Y2"].asDouble();
		pointY[i][2] = pts[i]["pts_3d"]["Y3"].asDouble();
		pointY[i][3] = pts[i]["pts_3d"]["Y4"].asDouble();

	}
}

int main(int ac, char** av)
{
	//Mat img = imread("1/000100.jpg");
	Mat img = imread("1/000136.jpg");
	Mat p_img;

	//int index = 0;
	//char buf[256];

	glob(path, str, false);

	if (str.size() == 0)
		cout << "image non exist. \n" << endl;

	parsing_json();

	if (!cap.isOpened())
	{
		printf("Can't open the camera");
		return -1;
	}
	//cap >> img;

	clock_t start, end, start_1, end_1, start_2, end_2, start_3, end_3;

	struct Circle2D c1, c2, c1_d, c2_d, c_b;
	struct transP f1, f2, e, e2;

	double wid = (abs(wpointX[1] - wpointX[0]) + abs(wpointX[2] - wpointX[3])) * 0.5;
	double hei = (abs(wpointY[2] - wpointY[1]) + abs(wpointY[3] - wpointY[0])) * 0.5;
	rad1 = (500 * wid) / 13500;
	rad2 = (500 * hei) / 3300;
	rad_a = rad1 + rad2;
	//cout << "rad1 : " << rad1 << " ,rad2 : " << rad2 << endl;

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
	createTrackbar("Thickness", SetWindow, &thick, 50);
	setTrackbarMin("Thickness", SetWindow, 1);
	createTrackbar("Line/Arrow/null", SetWindow, &line_off, 1);
	createTrackbar("Arrow tip", SetWindow, &arrow_tip, 10);
	createTrackbar("Circle/two color/Fill/null", SetWindow, &circle_type, 4);
	//setWindowProperty(SetWindow, WND_PROP_TOPMOST, 1);

	Mat plane(img.size(), img.type(), Scalar(50, 130, 55));
	//Mat plane(hei * 2, wid * 2, img.type(), Scalar(50, 130, 55));	//ROI
	Mat draw = Mat::zeros(plane.size(), plane.type());		//2d draw
	Mat drawimg = Mat::zeros(img.size(), img.type());
	warp = Mat::zeros(img.size(), img.type());
	Mat result = Mat::zeros(img.size(), img.type());
	Mat roi;
	Mat flag_mini, flag_warp;
	Mat img_warp;
	Mat inner_mini, inner_warp;

	//Mat draw_res, draw_out, output;
	//Mat flag_t;

	

	int t = 1; //time second 

	//draw image(flag)
	
	resize(flag, flag_mini, flag.size() / 4, 0, 0, cv::INTER_AREA);
	//cvtColor(inner, inner, COLOR_BGR2RGB);
	resize(inner, inner_mini,Size(2*rad-thick,2*rad-thick), 0, 0, cv::INTER_AREA);
	
	//imshow("korea test", flag_mini);
	//warpPerspective(img, img_warp, h[0], img.size());
	//imshow("warp img", img_warp);


	p1 = Point(674,415), p2=Point(1002,800), p3=Point(1042,800);
	struct warpPoint warp1, warp2, warp3;
	findHomo(str.size()-1);
	calcHomograpy(p1.x, p1.y, &warp1.x, &warp1.y);
	calcHomograpy(p2.x, p2.y, &warp2.x, &warp2.y);
	calcHomograpy(p3.x, p3.y, &warp3.x, &warp3.y);
	wp1.x = warp1.x;
	wp1.y = warp1.y;
	wp2.x = warp2.x;
	wp2.y = warp2.y;
	wp3.x = warp3.x;
	wp3.y = warp3.y;
	cout << "select p1 " << p1.x << endl;
	cout << "warp p1 " << warp1.x << endl;
	



	while (1) {

		//warpPerspective(flag_black, flag_t, h[0],img.size());

		/*if (img.empty())
		{
			printf("empty image");
			return 0;
		}*/
		//for (int i = 0; i < 30 ; i++)

		////click two points
		showImg("Click two Points", img, scale_view);
		moveWindow("Click two Points", 1800, 10);
		setMouseCallback("Click two Points", onMouse);
		//setWindowProperty(SetWindow, WND_PROP_TOPMOST, 1);

		//if (waitKey(0) == 32);
		findHomo(0);

		/*fx = 1230;
		fy = 532;*/
		fx = 625;
		fy = 798;
		fx2 = 510;
		fy2 = 606;
		calcHomograpy(fx, fy, &f1.hfx, &f1.hfy);
		calcHomograpy(fx2, fy2, &f2.hfx, &f2.hfy);


		//warpAffine(flag_warp, flag_warp, translation, flag_warp.size());
		//resize(flag_warp, flag_warp, flag_warp.size());
	
		max_a = 90;
		Mat frame_start = imread(str[0]);
		
		alpha = 0;
		spin = 0;
		calcHomograpy(fx_e, fy_e, &e.hfx, &e.hfy);
		calcHomograpy(fx2_e, fy2_e, &e2.hfx, &e2.hfy);

		c1.x = f1.hfx;
		c1.y = f1.hfy;
		c2.x = f2.hfx;
		c2.y = f2.hfy;

		c1.ex = e.hfx;
		c1.ey = e.hfy;
		c2.ex = e2.hfx;
		c2.ey = e2.hfy;

		angle = atan(double(c2.y - c1.y) / double(c2.x - c1.x));
		double rx, ry;
		rx = rad * cos(angle);
		ry = rad * sin(angle);
		double arrStartX, arrStartY, arrEndX, arrEndY;
		arrStartX = c1.x + rx;
		arrStartY = c1.y + ry;
		arrEndX = c2.x - rx;
		arrEndY = c2.y - ry;
		lineEndx = arrEndX - rx * 0.85;
		lineEndy = arrEndY - ry * 0.85;

		//1. Drawing Appear
		for (int i = 0; i < fps/2; i++)
		{	

			start = clock();
			warpPerspective(frame_start, drawimg, h[0], drawimg.size());	
			

			alpha += max_a/fps*2;
			spin += 360 / fps*2;
			
			//radius = (rad / (fps/2)) * i +  rad/2;
			radius = ((-192.0 * rad) / (245 * pow((fps/2 ), 2)) )* pow(  i - (7 / 8) * (fps/2 ), 2) + (11 / 10) * rad;
			//cout << "rad: " << rad << endl;
			setMouseCallback("Result", onMouse);

			draw = Mat::zeros(img.size(), img.type());
			warp = Mat::zeros(img.size(), img.type());

			c1.color = Scalar(b, g, r);
			c2.color = rgb2;

			drawPlane(draw);
			drawCircle(draw, c1);
			drawCircle(draw, c2);
			

			moveWindow("draw", 2200, 10);
			showImg("draw", draw, scale_view);
			warpPerspective(draw, warp, h_T[0], img.size());

			subtract(frame_start, warp, result);
			setMouseCallback(SetWindow, onMouse);

			calcHomograpy_T(c1.x, c1.y, &hx1, &hy1);
			calcHomograpy_T(c2.x, c2.y, &hx2, &hy2);

			
			imwrite("result/result1_"+to_string(i)+"_appear.jpg", result);
			showImg("Result", result, scale_view);
			
			waitKey(1);

			end = clock();
			double delay = (double)(end - start);
			cout << "result: " << ((delay) / CLOCKS_PER_SEC) << "seconds" << endl;	
		}

		waitKey(1);

		//2. Arrow Appear
		for (int i = 0; i < fps; i++)
		{
			start_1 = clock();
			//warpPerspective(frame_start, drawimg, h[0], drawimg.size());

			spin += 360 / fps*2;
			//alpha = max_a;
			setMouseCallback("Result", onMouse);

			draw = Mat::zeros(img.size(), img.type());
			warp = Mat::zeros(img.size(), img.type());

		/*	calcHomograpy(fx_e, fy_e, &e.hfx, &e.hfy);
			calcHomograpy(fx2_e, fy2_e, &e2.hfx, &e2.hfy);

			c1.x = f1.hfx;
			c1.y = f1.hfy;
			c2.x = f2.hfx;
			c2.y = f2.hfy;

			c1.ex = e.hfx;
			c1.ey = e.hfy;
			c2.ex = e2.hfx;
			c2.ey = e2.hfy;*/

			//c1.color = Scalar(b, g, r);
			//c2.color = rgb2; 
			drawPlane(draw);
			
			drawCircle(draw, c2);

			double dx, dy;
		/*	dx = double(c1.x + (c2.x - c1.x) * i / fps*2);
			dy = double(c1.y + (c2.y - c1.y) * i / fps*2);*/

			/*dx = double(c1.x + (c2.x - c1.x) / pow(fps, 2) * pow(i, 2));
			dy = double(c1.y + (c2.y - c1.y) / pow(fps, 2) * pow(i, 2));*/
			
	/*		dx = double(c1.x+rx + (c2.x - c1.x-rx) / pow(fps, 1.0 / 2.5) * pow(i, 1.0 / 2.5));
			dy = double(c1.y+ry + (c2.y - c1.y-ry) / pow(fps, 1.0 / 2.5) * pow(i, 1.0 / 2.5));*/

			/*dx = double(c1.x + (c2.x - c1.x) / sqrt(fps) * sqrt(i));
			dy = double(c1.y + (c2.y - c1.y) / sqrt(fps) * sqrt(i));*/
			
			
			

			dx = double(arrStartX  + (lineEndx -arrStartX) / pow(fps, 1.0 / 2.5) * pow(i, 1.0 / 2.5));
			dy = double(arrStartY + (lineEndy - arrStartY) / pow(fps, 1.0 / 2.5) * pow(i, 1.0 / 2.5));

			drawLine(draw, arrStartX,arrStartY,dx,dy, 1);
			drawCircle(draw, c1);

			showImg("draw", draw, scale_view);
			
			warpPerspective(draw, warp, h_T[0], img.size());

			subtract(frame_start, warp, result);
			setMouseCallback(SetWindow, onMouse);


			calcHomograpy_T(c1.x, c1.y, &hx1, &hy1);
			calcHomograpy_T(c2.x, c2.y, &hx2, &hy2);

			imwrite("result/result2_" + to_string(i) + "_arrow.jpg", result);
			showImg("Result", result, scale_view);
			waitKey(1);

			end_1 = clock();
			double delay_1 = (double)(end_1 - start_1);
			cout << "result 2 : " << ((delay_1) / CLOCKS_PER_SEC) << "seconds" << endl;

		}

		spin = 0;
		// 3. frame swipe
		for (int cnt = 0; cnt < str.size()-2 ; cnt++) {
			

			p_img = imread(str[cnt]);
			findHomo(cnt);

			//warpPerspective(img, roi, h[cnt], img.size());
			warpPerspective(img, drawimg, h[cnt], drawimg.size());	//
			//showImg("warp test", roi, scale_view);
			//showImg("drawimg", drawimg, scale_view);	//

			//resize(roi, roi, img.size()); 
			//resize(roi, roi, Size(wid * scale, hei * scale));
			//cap >> img;
			
			
			spin += 5;

			setMouseCallback("Result", onMouse);

			draw = Mat::zeros(img.size(), img.type());
			warp = Mat::zeros(img.size(), img.type());

			/*calcHomograpy(fx_e, fy_e, &e.hfx, &e.hfy);
			calcHomograpy(fx2_e, fy2_e, &e2.hfx, &e2.hfy);*/

			


			//add flag
			//Mat flag_black = Mat::zeros(img.size(), img.type());
			//if (fx != 0)
			//{
			//	Mat mini_roi = flag_black(Rect(f1.hfx, f1.hfy, flag_mini.cols, flag_mini.rows));
			//	add(flag_mini, mini_roi, mini_roi);
			//}
			//warpPerspective(flag_black, flag_warp, h_T[cnt], img.size());
			//Mat dst = Mat::zeros(img.size(), img.type());
			//dst = p_img + flag_warp*0.1;
			////imshow("dst", dst);
			//setMouseCallback("dst", onMouse);


		/*	c1.x = f1.hfx;
			c1.y = f1.hfy;
			c2.x = f2.hfx;
			c2.y = f2.hfy;

			c1.ex = e.hfx;
			c1.ey = e.hfy;
			c2.ex = e2.hfx;
			c2.ey = e2.hfy;*/
		
			c1.color = Scalar(b, g, r);
			c2.color = rgb2;

			drawPlane(draw);

			
			drawCircle(draw, c2);

			//inter image
			Mat inner_black = Mat::zeros(img.size(), img.type());
			
			if (fx != 0)
			{
				Mat inner_roi = inner_black(Rect(f1.hfx - (rad - thick / 2), f1.hfy - (rad - thick / 2), inner_mini.cols, inner_mini.rows));
				//Mat inner_roi = draw(Rect(0,0, inner_mini.cols, inner_mini.rows));
				add(inner_mini , inner_roi, inner_roi);
			}
			warpPerspective(inner_black, inner_warp, h_T[cnt], img.size());

			//blur(warp, warp, Size(5, 5));
			drawLine(draw, arrStartX, arrStartY, lineEndx, lineEndy, 1);
			drawCircle(draw, c1);
			/*dx = f1.hfx + (e.hfx - f1.hfx) * i / float(fps * t);
			dy = f1.hfy + (e.hfy - f1.hfy) * i / float(fps * t);*/
			//drawLine(draw, f1.hfx, f1.hfy, dx, dy, 1);		// end x = x+(ex-x)/(fps*time)*i
		//	showImg("draw", draw, scale_view);
			//resize(draw, draw, img.size());
			warpPerspective(draw, warp, h_T[cnt], img.size());
			//subtract(roi, draw, drawimg);
			//showImg("draw", drawimg, scale_view);
			//subtract(img, warp, result);
			subtract(p_img, warp, result);
			
			//imshow("inner", inner_warp);
			Mat inner_mask,gray_mask;
			cvtColor(inner_warp, gray_mask, COLOR_BGR2GRAY);
			threshold(gray_mask, inner_mask, 1, 255, THRESH_BINARY);
			blur(inner_mask, inner_mask, Size(5, 5));
			//imshow("mask", inner_mask);
			Mat p_img2=Mat::zeros(img.size(), img.type());
			//cout << "p_img : " << p_img.size() << "inner_mask : " << inner_mask.size() << endl;
			cvtColor(inner_mask, inner_mask, COLOR_GRAY2BGR);
			subtract(p_img, inner_mask*0.5, p_img2);
			
			Mat result2;
			result2 = p_img2 + inner_warp*0.5;
			//imshow("result2", result2);

			//bitwise_and(gray_mask, inner_mask, inner_mask);
			//imshow("mask", inner_mask);
			//result = result + inner_warp*0.2 ;
			setMouseCallback(SetWindow, onMouse);

			double hx1, hy1, hx2, hy2;
			calcHomograpy_T(c1.x, c1.y, &hx1, &hy1);
			calcHomograpy_T(c2.x, c2.y, &hx2, &hy2);
			//ellipse(result, Point(hx1,hy1), Size(10, 10), 0, 360, 360, Scalar(0,0,255), 3, 16);
			//ellipse(result, Point(hx2, hy2), Size(10, 10), 0, 360, 360, Scalar(0, 0, 255), 3, 16);
			//cout << "\ndraw center" << cnt << " " << f1.hfx << "," << f1.hfy << endl;
			//cout << "red center" << cnt << " " << hx1 << "," << hy1 << endl;

			imwrite("result/result3_" + to_string(cnt) + "_swipe.jpg", result);
			showImg("Result", result, scale_view);
			
			//add(draw_out, draw_color, draw_color);
			//imshow("black add", draw_color);
			//add(draw_out, draw, draw);		//(roi - draw) + draw 차영상에 draw이미지를 합성
			//add(roi * (10.0 - alpha) * 0.1, draw * alpha * 0.1, draw);	//alpha blending
			//vconcat(roi, draw_res, output);
			//vconcat(output, draw, output);
			//imshow("draw result", output);
			//imshow(SetWindow, draw);
			//setMouseCallback(SetWindow, onMouse);


			//add(dst * (10.0 - alpha) * 0.1, dst2 * alpha * 0.1, dst2);

			//setMouseCallback("Image", onMouse);

			//warpPerspective(draw_res, warp, h_T, img.size());
			////blur(warp, warp, Size(15, 15));
			//showImg("Warp draw", warp, scale_view);

			////sharp
			///*Mat sharp;
			//float data[9] = { -1,-1,-1,-1,3,-1,-1,-1,-1 };
			//Mat kernel(3, 3, CV_8UC1, data);
			//filter2D(warp, sharp, -1, kernel, Point(-1, -1), 0);*/
			////addWeighted(warp, 0.6, sharp, 0.4,0,warp);

			//subtract(img, warp, result);
			//add(result, warp, warp);
			//add(img * (10.0 - alpha) * 0.1, warp * alpha * 0.1, warp);
			//add(dst2 * (10.0 - alpha) * 0.1, warp * alpha * 0.1, dst2);
			//showImg("Image", dst2, scale_view);
			//showImg("Result", warp, scale_view);
			

			/*if (!cap.isOpened())
			{
				printf("Can't open the camera");
				return -1;
			}
			cap >> frame;

			while (1)
			{
				cap >> frame;
				if (frame.empty())
				{
					printf("empty image");
					return 0;
				}

				imshow("frame", img);

				if (waitKey(1000/fps) >= 0)
					break;
			}


			cap.release();*/
			waitKey(1);

			//if (waitKey((1000)) == 32);
		}

		//4. Drawing Disappear
		Mat frame_end = imread(str[str.size()-1]);
		//calcHomograpy(fx, fy, &f1.hfx, &f1.hfy);
		//calcHomograpy(fx2, fy2, &f2.hfx, &f2.hfy);
		findHomo(str.size()-1);
		for (int i = 0; i < fps/2; i++)
		{
			start_3 = clock();
			//warpPerspective(frame_start, drawimg, h[0], drawimg.size());

			alpha -= max_a / fps*2;
			setMouseCallback("Result", onMouse);

			draw = Mat::zeros(img.size(), img.type());
			warp = Mat::zeros(img.size(), img.type());

			c1.color = Scalar(b, g, r);
			c2.color = rgb2;

			drawPlane(draw);
			
			drawCircle(draw, c2);
			drawLine(draw, arrStartX, arrStartY, lineEndx, lineEndy, 1);
			drawCircle(draw, c1);
			//showImg("draw", draw, scale_view);
			warpPerspective(draw, warp, h_T[str.size() - 1], img.size());

			subtract(frame_end, warp, result);
			setMouseCallback(SetWindow, onMouse);

			calcHomograpy_T(c1.x, c1.y, &hx1, &hy1);
			calcHomograpy_T(c2.x, c2.y, &hx2, &hy2);

			imwrite("result/result4_" + to_string(i) + "_disappear.jpg", result);
			showImg("Result", result, scale_view);
			waitKey(1);

			end_3 = clock();
			double delay_3 = (double)(end_3 - start_3);
			cout << "result 3: " << ((delay_3) / CLOCKS_PER_SEC) << "seconds" << endl;
		}
		waitKey(0);
	}


	return 0;
}

void findHomo(int n)
{
	//double newpointX[4] = { 0,wid,wid,0 };
	//double newpointY[4] = { 0,0,hei,hei };

	vector<Point2d> pts_src, pts_dst;

	//for (size_t i(0); i < 4; i++) {
	//	pts_src.push_back(Point2d(pointX[i], pointY[i]));
	//	pts_dst.push_back(Point2d(wpointX[i], wpointY[i]));
	//	//pts_dst.push_back(Point2d(newpointX[i], newpointY[i]));
	//}
	for (size_t i(0); i < 4; i++) {
		pts_src.push_back(Point2d(pointX[n][i], pointY[n][i]));
		pts_dst.push_back(Point2d(wpointX[i], wpointY[i]));
	}


	//img to plane
	h[n] = findHomography(pts_src, pts_dst, RANSAC);
	//cout << "\n" << "h (3d->2d):" << h << "\n" << endl;
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
	//cout << "h_T (2d->3d):" << h_T << "\n" << endl;
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

void drawCircle(Mat plane, struct Circle2D c)
{
	double theta = atan2(c.y - c.ey, c.x - c.ex) * 180.0 / CV_PI;
	int beta = 20; //beta는 360의 약수


	switch (color_c1)
	{
	case 0:
		rgb1 = Scalar((255 - b)*alpha/max_a, (255 - g)*alpha/max_a, (255 - r)*alpha/max_a);				//custom
		break;
	case 1:
		rgb1 = Scalar(alpha, alpha, 0);		//red
		break;
	case 2:
		rgb1 = Scalar(alpha, alpha / 2, 0);		//orange
		break;
	case 3:
		rgb1 = Scalar(alpha, 0, 0);			//yellow
		break;
	case 4:
		rgb1 = Scalar(alpha, 0, alpha);		//green
		break;
	case 5:
		rgb1 = Scalar(0, alpha, alpha);		//blue
		break;
	case 6:
		rgb1 = Scalar(0, 0, alpha);			//cyan
		break;
	case 7:
		rgb1 = Scalar(alpha, alpha, alpha);	//black
		break;
	}

	switch (circle_type)
	{
	case 0:
		//ellipse(plane, Point(x, y), Size(rad * factor+4, rad * factor+4), theta, 30, 330, green, (thick + 1) / 5, 16);
		//ellipse(plane, Point(c.x, c.y), Size(rad, rad), spin, 30, 330, rgb1, -1, 16);
		ellipse(plane, Point(c.x, c.y), Size(rad, rad), 0,0,360, rgb1, -1, 16);
		ellipse(plane, Point(c.x, c.y), Size(rad+2, rad+2), spin-30, 0, 60, Scalar(0, 0, 0), -1, 16);
		ellipse(plane, Point(c.x, c.y), Size(rad, rad), spin - 30, 0, 60, Scalar(0, alpha, alpha), -1, 16);
		ellipse(plane, Point(c.x, c.y), Size(rad*0.6, rad*0.6), 0,0,360, Scalar(0,0,0), -1, 16);
		
		
		
		break;
	case 1:
		//ellipse(plane, Point(c.x, c.y), Size(rad, rad), 0, 0, 360, rgb1, (thick + 1) / 2, 16);
		ellipse(plane, Point(c.x, c.y), Size(rad, rad), 0, 0, 360, rgb1,-1, 16);
		ellipse(plane, Point(c.x, c.y), Size(rad - thick/2, rad - thick/2), 0, 0, 360, Scalar(0,0,0), -1, 16);

		//	ellipse(plane, Point(c.x, c.y), Size(rad, rad), spin -30, 0, 60, Scalar(20,20,20), (thick + 1) / 2, 16);
		break;
	case 2:

		ellipse(plane, Point(c.x, c.y), Size(rad, rad), 0, 0, 360, rgb1, -1, 16);
		for (int i = 0; i < 360 / beta; i++)
		{
			if (i % 2 == 0)
			{
				//ellipse(plane, Point(c.x, c.y), Size(rad, rad), beta * i, 0, beta * 0.6, rgb1, (thick + 1) / 4, 16);
				
				ellipse(plane, Point(c.x, c.y), Size(rad+2, rad+2), spin+beta * i, 0, beta * 0.6, Scalar(0,0,0), -1, 16);
				
			}
		}
		ellipse(plane, Point(c.x, c.y), Size(rad * 0.8, rad * 0.8), 0, 0, 360, Scalar(0,0,0), -1, 16);
		break;
	case 3:
		//ellipse(plane, Point(c.x+3, c.y+3), Size(rad, rad), 0, 0, 360, Scalar(alpha, alpha, alpha), -1, 16);
		//ellipse(plane, Point(c.x, c.y), Size(rad1*2, rad2*2), 0, 0, 360, rgb1, -1, 16);
		ellipse(plane, Point(c.x, c.y), Size(rad,rad), 0, 0, 360, rgb1, -1, 16);
		break;
	case 4: 
		ellipse(plane, Point(c.x, c.y), Size(rad1 * 2, rad2 * 2), 0, 0, 360, rgb1, -1, 16);
		break;
	}
}

void drawLine(Mat plane, double x, double y, double dx, double dy, double factor)
{
	int k = 1;
	//
	Point pts[1][3];

	double triEx, triEy;
	triEx = dx + rad * cos(angle);
	triEy = dy + rad * sin(angle);
	pts[0][0] = Point(triEx,triEy);
	double trip1x, trip1y, trip2x, trip2y;
	trip1x = triEx - rad * cos(angle + 30*M_PI/180);
	trip1y = triEy - rad * sin(angle + 30*M_PI/180);
	trip2x = triEx - rad * cos(angle - 30*M_PI/180);
	trip2y = triEy - rad * sin(angle - 30*M_PI/180);

	pts[0][1] = Point(trip1x,trip1y);
	pts[0][2] = Point(trip2x,trip2y);

	const Point* ppt[1] = { pts[0] };
	int npt[] = { 3 };



	switch (color_l)
	{
	case 0:
		rgb = Scalar(0, alpha, 0);
	case 1:
		rgb = Scalar(alpha, alpha, 0);
		break;
	case 2:
		rgb = Scalar(alpha, alpha / 2, 0);
		break;
	case 3:
		rgb = Scalar(alpha, 0, 0);
		break;
	case 4:
		rgb = Scalar(alpha, 0, alpha);
		break;
	case 5:
		rgb = Scalar(0, alpha, alpha);
		break;
	case 6:
		rgb = Scalar(0, 0, alpha);
		break;
	case 7:
		rgb = Scalar(alpha, alpha, alpha);
		break;
	}
	switch (line_off)
	{
	case 0:
		
		//arrowedLine(plane, Point(x+k, y+k), Point(dx+k, dy+k), Scalar(alpha,alpha,alpha), thick / 5 + 4, 16, 0, arrow_tip * 0.1 * 0.2);
		arrowedLine(plane, Point(x, y), Point(dx, dy), rgb, thick / 5 + 4, 16, 0, arrow_tip * 0.1 * 0.2);
		break;
	case 1:
		fillPoly(plane, ppt, npt, 1, rgb, 16);
		//line(plane, Point(x, y), Point(dx - (rad-1) * cos(angle), dy-(rad-1)*sin(angle)), rgb, thick / 5 + 4, 16);
		line(plane, Point(x, y), Point(dx+3*cos(angle),dy+3*sin(angle)), rgb, thick / 5 + 4, 16);
		break;
	}
}


//draw 3d calib ROI 
void drawPlane(Mat dst)
{
	//Point pts[1][4];
	//pts[0][0] = p1;
	//pts[0][1] = p2;
	//pts[0][2] = p3;
	//pts[0][3] = p4;
	Point pts[1][3];
	
	pts[0][0] = wp1;
	pts[0][1] = wp2;
	pts[0][2] = wp3;
	
	const Point* ppt[1] = { pts[0] };
	int npt[] = { 3 };


	switch (color_c2)
	{
	case 0:
		rgb2 = Scalar((255 - b) * alpha / max_a, (255 - g) * alpha / max_a, (255 - r) * alpha / max_a);				//custom
		break;
	case 1:
		rgb2 = Scalar(alpha, alpha, 0);		//red
		break;
	case 2:
		rgb2 = Scalar(alpha, alpha / 2, 0);		//orange
		break;
	case 3:
		rgb2 = Scalar(alpha, 0, 0);			//yellow
		break;
	case 4:
		rgb2 = Scalar(alpha, 0, alpha);		//green
		break;
	case 5:
		rgb2 = Scalar(0, alpha, alpha);		//blue
		break;
	case 6:
		rgb2 = Scalar(0, 0, alpha);			//cyan
		break;
	case 7:
		rgb2 = Scalar(alpha, alpha, alpha);	//black
		break;
	}


	fillPoly(dst, ppt, npt, 1, rgb2, 8);


	//const Point* ppt[1] = { pts[0] };
	//int npt[] = { 4 };

	//fillPoly(dst, ppt, npt, 1, Scalar(224, 4, 23), 8);

	//circle(dst, p1, 10, Scalar(55, 0, 255), 5, 8, 0);
	//circle(dst, p2, 10, Scalar(5, 255, 255), 5, 8, 0);
	//circle(dst, p3, 10, Scalar(0, 255, 33), 5, 8, 0);
	//circle(dst, p4, 10, Scalar(255, 0, 33), 5, 8, 0);

}


void eventclass::HandleEvent(int evt, int x, int y, int flags)
{
	if (evt == EVENT_LBUTTONDOWN)
	{
		fx = x;
		fy = y;
		alpha = 0;
		cout << "\nx,y:" << x << "," << y << endl;
	}

	if (evt == EVENT_LBUTTONUP)
	{
		fx_e = x;
		fy_e = y;
		cout << "\nex,y:" << x << "," << y << endl;
	}

	if (evt == EVENT_RBUTTONDOWN)
	{
		fx2 = x;
		fy2 = y;
		cout << "\n2x,2y:" << x << "," << y << endl;
	}

	if (evt == EVENT_RBUTTONUP)
	{
		fx2_e = x;
		fy2_e = y;
	}
}


void onMouse(int evt, int x, int y, int flags, void* param)
{
	eventclass* p = (eventclass*)param;
	p->HandleEvent(evt, x, y, flags);
}

