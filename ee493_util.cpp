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

#define PI 3.14159
using namespace cv;
using namespace std;

void loadWindows(); // loads control window
void directionData(int& posX, int& posY); // send motor signals
void displayRASP(Mat dst);
void setColor(int colorFront);
void tx2Arduino();
int getFPS();
int getDistance();
void drawStraightLine(Mat *img, Point2f p1, Point2f p2);
void trackObject(Mat dst, bool arduinoConnected);
//void testWindow(Mat image);

void testWindow(Mat image, String title);
time_t start;
Mat newFrame;
string positionText = "WAITING FOR DATA";
FILE* file; // object to open device file
Mat newFrameThresholded;
Mat dst;
VideoCapture camera(0);

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
float resizeRatio = 0.5;
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
   // if (posX < highRange && posX > lowRange) {
        positionText.assign("Mid    FPS :");
        statusBar << int(getFPS());
        positionText.append(statusBar.str());
        direction = 11;

        statusBar.str(""); // clear string stream
        //statusBar << int(getAngle());
        statusBar << int(angle);
        positionText.append("  Angle: ");
        positionText.append(statusBar.str());
    //}
/*    else if (posX > highRange) {
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
    }*/
}

void drawStraightLine(Mat *img, Point2f p1, Point2f p2)
{
        Point2f p, q;
        // Check if the line is a vertical line because vertical lines don't have slope
        if (p1.x != p2.x)
        {
                p.x = 0;
                q.x = img->cols;
                // Slope equation (y1 - y2) / (x1 - x2)
                float m = (p1.y - p2.y) / (p1.x - p2.x);
                // Line equation:  y = mx + b
                float b = p1.y - (m * p1.x);
                p.y = m * p.x + b;
                q.y = m * q.x + b;
        }
        else
        {
                p.x = q.x = p2.x;
                p.y = 0;
                q.y = img->rows;
        }

        line(*img, p, q, Scalar(255, 100, 100), 1);
        
        //angle=atan((p1.y - p2.y) / (p1.x - p2.x))*180/3.1415;

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
    resize(newFrame, newFrame, Size(), 0.6 / resizeRatio, 0.6 / resizeRatio, INTER_LINEAR);
    displayStatusBar("Denoised", positionText, 0);
    imshow("Denoised", newFrame); // show camera input at thr. window in HSV
    imshow("Control Bar", dst); // show camera input at thr. window in HSV

    moveWindow("Denoised", 50, 250);
    moveWindow("Control Bar", 450, 250);
}

void testWindow(Mat image, String title)
{
    //char title[] = "Test";
    namedWindow(title, 1);
    moveWindow(title, 850, 250);
    resize(image, image, Size(), 0.6 / resizeRatio, 0.6 / resizeRatio, INTER_AREA);
    imshow(title, image); 
}

void setColor(int colorFront)
{
    switch (colorFront) {
    case int('B'):
        iLowH = 70;
        iHighH = 110;
        iLowS = 50;
        iHighS = 255;
        iLowV = 50;
        iHighV = 255;
        break;

    case int('R'):
        iLowH = 110;
        iHighH = 179;
        iLowS = 15;
        iHighS = 255;
        iLowV = 35;
        iHighV = 255;
        break;

    case int('Y'):
        iLowH = 15;
        iHighH = 70;
        iLowS = 50;
        iHighS = 255;
        iLowV = 35;
        iHighV = 255;
        break;

    case int('G'):
        iLowH = 50;
        iHighH = 85;
        iLowS = 0;
        iHighS = 255;
        iLowV = 0;
        iHighV = 255;
        break;
    }
}

void loadWindows()
{
    char controlBar[] = "Control Bar";
    namedWindow(controlBar, 1); // create a window called "Control"
    /*cvCreateTrackbar("dilate iteration", controlBar, &filterRatio, 10); //Hue (0 - 179)
    cvCreateTrackbar("LowHue", controlBar, &iLowH, 179); // Hue (0 - 179)
    cvCreateTrackbar("HighHue", controlBar, &iHighH, 179);
    cvCreateTrackbar("LowSat", controlBar, &iLowS, 255); //Saturation (0 - 255)
    cvCreateTrackbar("HighSat", controlBar, &iHighS, 255);
    cvCreateTrackbar("LowVal", controlBar, &iLowV, 255); //Value (0 - 255)
    cvCreateTrackbar("HighVal", controlBar, &iHighV, 255);
    cvCreateTrackbar("Margin", controlBar, &lockToleranceInt,50); // Value (0 - 255)
    cvCreateTrackbar("Offset", controlBar, &offset, 50); // Value (0 - 255)*/
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

    return distance;  //modified to test slopeLine
}


void trackObject(Mat dst, bool arduinoConnected)
{
    if (!object_exist) {
        positionText.assign("Searching!");
        return;
    }

    Moments oMoments = moments(dst);
    double dM01 = oMoments.m01;
    double dM10 = oMoments.m10;
    double dArea = oMoments.m00;
    int posX = dM10 / dArea;
    int posY = dM01 / dArea;
    directionData(posX, posY); // enable motor output

    if (arduinoConnected)
        tx2Arduino();
}