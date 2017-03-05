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
int colorFront='B';

Point2f drawCenterLine(Mat imageIn, int colorFront);
Mat getObjectOfColor(Mat image, int colorFront, int object);
Mat getLargestArea(Mat image, int object);
void goTowardsObject(int base_speed, Mat img, int colorFront, bool ArduinoConnected);
void statusBar(Point2f center);

Point2f findCenter(Mat image);
Mat thresholdImage(Mat image, int colorFront, bool calibration );
double fillRatio(vector<Point> contour, int object);
int getFPS();
int getDistance();
void drawStraightLine(Mat *img, Point2f p1, Point2f p2);
void calibrateThreshold(int color);
void dispImage(Mat image, String title, int loc);
void setColor(int colorFront);

time_t start;
Mat newFrame;
string positionText = "";
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


Mat thresholdImage(Mat image, int colorFront, bool calibration )
{
    
    if (!calibration)
        setColor(colorFront);
    
    Mat imageOUT=image.clone();
    cvtColor(imageOUT, imageOUT, COLOR_BGR2HSV);
    inRange(imageOUT, Scalar(iLowH, iLowS, iLowV),
        Scalar(iHighH, iHighS, iHighV), imageOUT);   
    erode(imageOUT, imageOUT, cv::Mat(), cv::Point(-1, -1), 2);
    medianBlur(imageOUT, imageOUT, 5);
    //erode(imageOUT, imageOUT, cv::Mat(), cv::Point(-1, -1), 3);
    //dilate(imageOUT, imageOUT, cv::Mat(), cv::Point(-1, -1),1);
    return imageOUT;
}

void calibrateThreshold(int color){
    setColor(color);
    char windowTitle[] = "Calibration";
    namedWindow(windowTitle, 1);
    moveWindow(windowTitle, 500,500);
    cvCreateTrackbar("dilate iteration", windowTitle, &filterRatio, 10);
    cvCreateTrackbar("LowHue", windowTitle, &iLowH, 179);
    cvCreateTrackbar("HighHue", windowTitle, &iHighH, 179);
    cvCreateTrackbar("LowSat", windowTitle, &iLowS, 255); 
    cvCreateTrackbar("HighSat", windowTitle, &iHighS, 255);
    cvCreateTrackbar("LowVal", windowTitle, &iLowV, 255);
    cvCreateTrackbar("HighVal", windowTitle, &iHighV, 255);
    Mat image;
    while (true) {
        camera >> newFrame; // get a new frame from camera
        resize(newFrame, newFrame, Size(), resizeRatio, resizeRatio,INTER_LINEAR);

        image = thresholdImage(newFrame, 'B', true);
        dispImage(image, "Calibration", 2);
        int c = waitKey(10);
        if ((char)c == 27) {
            cvDestroyWindow(windowTitle);
            exit(0);
        }

    }
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
        
        angle=atan((p1.y - p2.y) / (p1.x - p2.x))*180/PI;

}

int getFPS()
{
    time_t end;
    end = clock();
    double fps = CLOCKS_PER_SEC / (end - start);
    start = end;
    return fps;
}


void dispImage(Mat image, String title, int loc)
{
    // 0 2 4
    // 1 3 5
    namedWindow(title, 1);
    resize(image, image, Size(), 0.55 / resizeRatio, 0.55 / resizeRatio, INTER_AREA);
    imshow(title, image);
    int x = 50 + (loc/2)*380;
    int y = 30 + (loc%2)*370;
    moveWindow(title, x, y);
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
        iLowS = 15;
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

double fillRatio(vector<Point> contour, int object)
{
    Point_<float> circleCenter;
    RotatedRect boundRect;
    double areaCont = contourArea(contour, false); 
    double fillRatio=0;
    //area of rectange : r.width * r.height 

    if (object == 'R'){
    boundRect = minAreaRect(contour);
    double rectEnvelopeArea =boundRect.size.width * boundRect.size.height;
    fillRatio = abs(boundRect.angle);
    }
    else if (object == 'C'){
    minEnclosingCircle(contour, circleCenter, circleRadius);
    double circleEnvelopeArea = PI * circleRadius * circleRadius;
    fillRatio = areaCont / (0.9 * circleEnvelopeArea);
    //angle = acos(fillRatio) * 180.0 / 3.1412;
    }

    return fillRatio;
}


Point2f findCenter(Mat image)
{
    //returns the center of a binary image
    Moments oMoments = moments(image);
    double dM01 = oMoments.m01;
    double dM10 = oMoments.m10;
    double dArea = oMoments.m00;
    
    int posX = dM10 / dArea;
    int posY = dM01 / dArea;

    return Point(posX,posY);
}


void statusBar(Point2f center){

    ostringstream statusBar;
    string statusText = "";
    statusText.assign("");
    statusText.assign("FPS :");
    statusBar << int(getFPS());
    statusText.append(statusBar.str());

    statusBar.str(""); // clear string stream
    statusBar << int(angle);
    statusText.append("  Angle: ");
    statusText.append(statusBar.str());

    statusBar.str(""); // clear string stream
    statusBar << center.x;
    statusText.append("  Center: ");
    statusText.append(statusBar.str());

    displayStatusBar("Threshold", statusText, 0);

}