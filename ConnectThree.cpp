#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <stdlib.h>
using namespace cv;
using namespace std;

void loadWindows(); // loads control window
void detectObject(int color);
void selectObject(Mat& dst); // filters out the closes object
void trackObject(Mat dst);
void directionData(int& posX, int& posY); // send motor signals
void displayRASP(Mat dst);
void setColor(int color);
void tx2Arduino();
void drawStraightLine(Mat *img, Point2f p1, Point2f p2);
void drawCenterLine(Mat imageBody);
void findCenter(Mat image, Point2f& point);
void selectObject2(Mat& imageIN, Mat& imageOUT);


int offset = 25; // 24=-1
int iLowH, iHighH, iLowS, iHighS, iLowV, iHighV;
int filterRatio = 0;
int lastContourIndex = -1;
int direction;
bool object_exist = false;
int largest_area = 0;
float circleRadius;
int lockToleranceInt = 20;
float angle;

vector<vector<Point> > contours; // Vector for storing contour
vector<Vec4i> hierarchy;
Rect bounding_rect;
Point_<float> circleCenter; // bounding circle params
string positionText = "WAITING FOR DATA";
Mat frame;
Mat newFrame;
Mat newFramePreScale;
Mat newFrameFilter;
Mat newFrameThresholded;
Mat imageResizeIN;
Mat imageResizeOUT;
VideoCapture camera(0);

FILE* file; // object to open device file

int main(int argc, char* argv[])
{
    int color='B';
    setColor(color);
    loadWindows();
    Mat imgTmpPreScale;
    camera.read(imgTmpPreScale);

    float resizeRatio = 0.2;
    resize(imgTmpPreScale, imageResizeOUT, Size(), resizeRatio, resizeRatio,
        INTER_LINEAR);
    Mat imgLines = Mat::zeros(imageResizeOUT.size(), CV_8UC3);
    Mat circleEnvelope = Mat::zeros(imageResizeOUT.size(), CV_8UC3);

    while (true) {
        direction = 00;
        Mat dst(imageResizeOUT.rows, imageResizeOUT.cols, CV_8UC1, Scalar::all(0));

        camera >> newFramePreScale; // get a new frame from camera
        resize(newFramePreScale, newFrame, Size(), resizeRatio, resizeRatio,
            INTER_LINEAR);

        detectObject(color);
        drawCenterLine(newFrameFilter);
        selectObject(dst);
        trackObject(dst);
        
        displayRASP(dst);
        // tx2Arduino();

        int c = waitKey(10);
        if ((char)c == 27) {
            break;
            while (1) {
                direction = 00;
                //tx2Arduino();

            }
            break;
        }
    }
    return 0;
}

void tx2Arduino()
{
    file = fopen("/dev/ttyUSB4", "w");
    fprintf(file, "%d/r", direction); // Writing to the file
    fclose(file);
}


void detectObject(int color)
{
    
    setColor(color);
    medianBlur(newFrame, newFrameFilter,
        2 * filterRatio + 1); // filters out noise, filterratio must be odd
    cvtColor(newFrameFilter, newFrameFilter, COLOR_BGR2HSV);
    inRange(newFrameFilter, Scalar(iLowH, iLowS, iLowV),
        Scalar(iHighH, iHighS, iHighV), newFrameThresholded);   
}


void drawCenterLine(Mat imageBody) //returns angle of the line
{
    Mat imageOUT1(imageResizeOUT.rows, imageResizeOUT.cols, CV_8UC1, Scalar::all(0));
    Mat imageOUT2(imageResizeOUT.rows, imageResizeOUT.cols, CV_8UC1, Scalar::all(0));
    Mat imageOUT3(imageResizeOUT.rows, imageResizeOUT.cols, CV_8UC1, Scalar::all(0));
       
    setColor('G');
    inRange(newFrameFilter, Scalar(iLowH, iLowS, iLowV),
        Scalar(iHighH, iHighS, iHighV), imageBody);
    Point2f point_mid;
    selectObject2(imageBody,imageOUT1);
    findCenter(imageOUT1, point_mid);
    
    setColor('B');
    inRange(newFrameFilter, Scalar(iLowH, iLowS, iLowV),
        Scalar(iHighH, iHighS, iHighV), imageBody);
    Point2f point_front;
    selectObject2(imageBody,imageOUT2);
    findCenter(imageOUT2, point_front);
 
    drawStraightLine(&newFrame,point_mid, point_front);
    imageOUT3=imageOUT1+imageOUT2;
    resize(imageOUT3, imageOUT3, Size(), 2, 2, INTER_LINEAR);
    imshow("Test Window", imageOUT3);
}

void findCenter(Mat image, Point2f& point)
{
    Moments oMoments = moments(image);
    double dM01 = oMoments.m01;
    double dM10 = oMoments.m10;
    double dArea = oMoments.m00;
    int posX = dM10 / dArea;
    int posY = dM01 / dArea;
    point=Point(posX,posY);
}

void selectObject2(Mat& imageIN, Mat& imageOUT)
{
    largest_area = 0;
    int largest_contour_index = 0;
    findContours(imageIN, contours, hierarchy, CV_RETR_CCOMP,
        CV_CHAIN_APPROX_SIMPLE); // Find the contours in the image
    for (int i = 0; i < contours.size(); i++) // iterate through each contour.
    {
        double a = contourArea(contours[i], false); //  Find the area of contour
        if (a > largest_area) {
            largest_area = a;
            largest_contour_index = i; // Store the index of largest contour
        }
    }
     if (largest_area > 25) {
        drawContours(
            imageOUT, contours, largest_contour_index, Scalar(255, 0, 0), CV_FILLED, 8,
            hierarchy); // Draw the largest contour using previously stored index.
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

        angle =(atan((p1.y - p2.y) / (p1.x - p2.x)) * 180 / 3.1415);
}


void selectObject(Mat& dst)
{
    largest_area = 0;
    int largest_contour_index = 0;
    findContours(newFrameThresholded, contours, hierarchy, CV_RETR_CCOMP,
        CV_CHAIN_APPROX_SIMPLE); // Find the contours in the image
    for (int i = 0; i < contours.size(); i++) // iterate through each contour.
    {
        double a = contourArea(contours[i], false); //  Find the area of contour
        if (a > largest_area) {
            largest_area = a;
            largest_contour_index = i; // Store the index of largest contour
            minEnclosingCircle(contours[i], circleCenter, circleRadius);
        }
    }   
    if (largest_area > 25) {
        drawContours(
            dst, contours, largest_contour_index, Scalar(255, 0, 0), CV_FILLED, 8,
            hierarchy); // Draw the largest contour using previously stored index.
        circle(newFrame, circleCenter, circleRadius, Scalar(255, 0, 0), 2, 8, 0);
        object_exist = true;
        // cout << largest_area << endl;
    }
    else {
        object_exist = false;
    }
}


int getAngle()
{
    /*float circleEnvelopeArea = 3.1412 * circleRadius * circleRadius;
    float fillRatio = largest_area / (0.9 * circleEnvelopeArea);
    angle = acos(fillRatio) * 180.0 / 3.1412;
    if (fillRatio > 1.0)
        angle = 0;*/
    return angle;
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

void trackObject(Mat dst)
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
        positionText.assign("Mid    Angle :");
        statusBar << int(getAngle());
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

void displayRASP(Mat dst)
{
    namedWindow("Denoised", 1);
    resize(newFrame, newFrame, Size(), 2, 2, INTER_LINEAR);
    displayStatusBar("Denoised", positionText, 0);
    imshow("Denoised", newFrame); // show camera input at thr. window in HSV
    imshow("Control Bar", dst); // show camera input at thr. window in HSV
    namedWindow("Test Window",1);
 
}


void setColor(int color)
{
    switch (color) {
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
    cvCreateTrackbar("filter", controlBar, &filterRatio, 9); //Hue (0 - 179)
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