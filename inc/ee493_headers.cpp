#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include <unistd.h>
#include <iostream>

#define PI 3.14159
using namespace cv;
using namespace std;

float resizeRatio = 0.5;
bool ArduinoConnected = false;

int cam_index_1 = 1;
int cam_index_2 = 1 - cam_index_1;

time_t start, end_t;
Mat newFrame;
string positionText = "";

VideoCapture camera;
int offset = 0; // 24=-1
int iLowH, iHighH, iLowS, iHighS, iLowV, iHighV;
int filterRatio = 1;
int dilateIter = 2;
int direction;
bool object_exist = false;
int largest_area = 0;
float circleRadius;
int lockToleranceInt = 75;
float angle;
float slopeLine;
double FPSCounter = 1;
int method, erode_val, dilate_val, filter, threshold_bw;