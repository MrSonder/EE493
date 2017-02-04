#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <time.h>

using namespace cv;
using namespace std;

void loadWindows(); // loads control window
void directionData(int& posX, int& posY); // send motor signals
void displayRASP(Mat dst);
void setColor(int colorFront);
void tx2Arduino();
int getFPS();
int getDistance();


time_t start;
Mat newFrame;
string positionText = "WAITING FOR DATA";
FILE* file; // object to open device file

int offset = 25; // 24=-1
int iLowH, iHighH, iLowS, iHighS, iLowV, iHighV;
int filterRatio = 1;
int dilateIter = 1;
int direction;
bool object_exist = false;
int largest_area = 0;
float circleRadius;
int lockToleranceInt = 20;
float angle;

float resizeRatio = 0.4;
int colorFront='B';
bool arduinoConnected=false;


void tx2Arduino()
{
    file = fopen("/dev/ttyUSB4", "w");
    fprintf(file, "%d/r", direction); // Writing to the file
    fclose(file);
}

void directionData(int& posX, int& posY)
{
    cv::Size s = newFrame.size();
    float height = s.height;
    int mid = s.width / 2;
    float lockTolerance = lockToleranceInt / 100.0;
    int lowRange = mid * (1 - lockTolerance) + (offset - 25);
    int highRange = mid * (1 + lockTolerance) + (offset - 25);
    int stopRange = 0.8 * height; // adjust the constant for stop range

    line(newFrame, Point(lowRange, stopRange), Point(highRange, stopRange),
        Scalar(100, 100, 255), 2, 4);
    line(newFrame, Point(mid, height), Point(mid, stopRange),
        Scalar(100, 100, 255), 2, 4);

    ostringstream statusBar;
    positionText.assign("");
    if (posX < highRange && posX > lowRange) {
        positionText.assign("Mid    FPS :");
        statusBar << int(getFPS());
        positionText.append(statusBar.str());
        direction = 11;

        statusBar.str(""); // clear string stream
        statusBar << int(getDistance());
        positionText.append("  Dist.: ");
        positionText.append(statusBar.str());
    }
    else if (posX > highRange) {
        positionText.assign("Right  Offset:");
        statusBar << int(posX - mid);
        positionText.append(statusBar.str());
        direction = 10;
    }

    else if (posX < lowRange) {
        positionText.assign("Left   Offset:");
        statusBar << int(mid - posX);
        positionText.append(statusBar.str());
        direction = 01;
    }

    if (posY > stopRange) {
        positionText.assign("Stop!");
        direction = 00;
    }
}

int getFPS()
{
    time_t end;
    end = clock();
    double fps = CLOCKS_PER_SEC / (end - start);
    start = end;
    return fps;
}

void displayRASP(Mat dst)
{
    namedWindow("Denoised", 1);
    resize(newFrame, newFrame, Size(), 0.5 / resizeRatio, 0.5 / resizeRatio, INTER_LINEAR);
    displayStatusBar("Denoised", positionText, 0);
    imshow("Denoised", newFrame); // show camera input at thr. window in HSV
    imshow("Control Bar", dst); // show camera input at thr. window in HSV
    namedWindow("Test Window", 1);
}

void setColor(int colorFront)
{
    switch (colorFront) {
    case int('B'):
        iLowH = 90;
        iHighH = 110;
        iLowS = 120;
        iHighS = 255;
        iLowV = 150;
        iHighV = 255;
        break;
    case int('R'):
        iLowH = 144;
        iHighH = 179;
        iLowS = 72;
        iHighS = 255;
        iLowV = 35;
        iHighV = 255;
        break;
    case int('Y'):
        iLowH = 0;
        iHighH = 40;
        iLowS = 0;
        iHighS = 255;
        iLowV = 220;
        iHighV = 255;
        break;
    case int('G'):
        iLowH = 50;
        iHighH = 75;
        iLowS = 72;
        iHighS = 255;
        iLowV = 35;
        iHighV = 255;
        break;
    }
}

void loadWindows()
{
    char controlBar[] = "Control Bar";
    namedWindow(controlBar, WINDOW_NORMAL); // create a window called "Control"
    cvCreateTrackbar("dilate iteration", controlBar, &filterRatio, 10); //Hue (0 - 179)
    cvCreateTrackbar("LowHue", controlBar, &iLowH, 179); // Hue (0 - 179)
    cvCreateTrackbar("HighHue", controlBar, &iHighH, 179);
    cvCreateTrackbar("LowSat", controlBar, &iLowS, 255); //Saturation (0 - 255)
    cvCreateTrackbar("HighSat", controlBar, &iHighS, 255);
    cvCreateTrackbar("LowVal", controlBar, &iLowV, 255); //Value (0 - 255)
    cvCreateTrackbar("HighVal", controlBar, &iHighV, 255);
    cvCreateTrackbar("Margin", controlBar, &lockToleranceInt,
        50); // Value (0 - 255)
    cvCreateTrackbar("Offset", controlBar, &offset, 50); // Value (0 - 255)
}

int getDistance()
{
    float x = largest_area;
    float a = -0.00741;
    float b = 8637;
    float c = 37.38;
    float distance1 = (a * x + b / x + c);

    a = -0.98565;
    b = 204.304;
    float distance2 = a * x + b;

    int distance = (distance1 > 110) ? int(distance2) : int(distance1);

    return distance;
}