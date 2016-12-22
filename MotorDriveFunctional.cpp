//to run on raspberry
//comment out: motorGPIO(), motorDrive(),displayCOMP() functions

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <wiringPi.h>
#include <fstream>      
#include <string>

using namespace cv;
using namespace std;

void loadWindows(); //loads control window
void detectObject(); //selects the target objects
void selectObject(Mat& dst, Mat& thr); //filters out the closes object
void trackObject(Moments& oMoments); //tracks the object
void driverOutput(int& posX, int& posY); //send motor signals
void displayObject(Mat dst); //send camera data to screen
void displayRASP(Mat dst);
void displayCOMP(Mat dst);
void motorGPIO();
void motorDrive();
void setBlue();
void setRed();
void setYellow();
void tx2Arduino();

int iLowH, iHighH, iLowS, iHighS, iLowV , iHighV;
int filterRatio = 1;
int iLastX = 0; //line param
int iLastY = 0;
int lastContourIndex = -1;
int direction;

vector<vector<Point> > contours; // Vector for storing contour
vector<Vec4i> hierarchy;
Rect bounding_rect;
Point_<float> circleCenter; //bounding circle params
float circleRadius;

Mat src;
Mat temp;
Mat frame;
Mat newFrame;
Mat newFramePreScale;
Mat newFrameFilter;
Mat newFrameHSV;
Mat newFrameThresholded;
Mat newFrameDenoised;
Mat newFrameDetected;
Mat imageResizeIN;
Mat imageResizeOUT;

VideoCapture camera(0);

FILE *file; //Opening device file

int main(int, char**)
{
    setBlue();
    //setRed();
    //setYellow();

    //motorGPIO();
    loadWindows();
    
    Mat imgTmpPreScale;
    camera.read(imgTmpPreScale);
    resize(imgTmpPreScale, imageResizeOUT, Size(), 0.2, 0.2, INTER_LINEAR);
    Mat imgLines = Mat::zeros(imageResizeOUT.size(), CV_8UC3);
    Mat circleEnvelope = Mat::zeros(imageResizeOUT.size(), CV_8UC3);
    
    
    while (true) {
    	direction = 00;
        Mat thr(imageResizeOUT.rows, imageResizeOUT.cols, CV_8UC1);
        Mat dst(imageResizeOUT.rows, imageResizeOUT.cols, CV_8UC1, Scalar::all(0));

        camera >> newFramePreScale; // get a new frame from camera
        resize(newFramePreScale, newFrame, Size(), 0.2, 0.2, INTER_LINEAR);

        detectObject();
        selectObject(dst, thr);
        Moments oMoments = moments(dst);
        trackObject(oMoments);

        displayRASP(dst);
        
        //displayCOMP(Mat dst);
        //motorDrive();
        //tx2Arduino();
        
        int c = waitKey(10);
        if ((char)c == 27) {
        	while(1){
        	direction=00;
        	tx2Arduino();
            // digitalWrite(25, LOW);
            // digitalWrite(29, LOW);
        }
            break;
        }
    }
    return 0;
}

void tx2Arduino(){      
          file = fopen("/dev/ttyUSB0", "w");        
          //cout << "tx:" << endl;        
          fprintf(file, "%d/r", direction);//Writing to the file     
          fclose(file);     
            
    }       


void loadWindows()
{
    char controlBar[] = "Control Bar";
    namedWindow(controlBar, CV_WINDOW_AUTOSIZE); //create a window called "Control"

    cvCreateTrackbar("filter", controlBar, &filterRatio, 21); //Hue (0 - 179)
    cvCreateTrackbar("LowHue", controlBar, &iLowH, 179); //Hue (0 - 179)
    cvCreateTrackbar("HighHue", controlBar, &iHighH, 179);
    cvCreateTrackbar("LowSat", controlBar, &iLowS, 255); //Saturation (0 - 255)
    cvCreateTrackbar("HighSat", controlBar, &iHighS, 255);
    cvCreateTrackbar("LowVal", controlBar, &iLowV, 255); //Value (0 - 255)
    cvCreateTrackbar("HighVal", controlBar, &iHighV, 255);
}

void detectObject()
{
    medianBlur(newFrame, newFrameFilter, 2 * filterRatio + 1); //filters out noise, filterratio must be odd
    cvtColor(newFrameFilter, newFrameHSV, COLOR_BGR2HSV);
    inRange(newFrameHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), newFrameThresholded);

    vector<Mat> channels;
}

void selectObject(Mat& dst, Mat& thr)
{
    int largest_area = 0;
    int largest_contour_index = 0;
    findContours(newFrameThresholded, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE); // Find the contours in the image
    Scalar color(255, 255, 255);

    for (int i = 0; i < contours.size(); i++) // iterate through each contour.
    {
        double a = contourArea(contours[i], false); //  Find the area of contour
        if (a > largest_area) {
            largest_area = a;
            largest_contour_index = i; //Store the index of largest contour
            bounding_rect = boundingRect(contours[i]); // Find the bounding rectangle for biggest contour
            minEnclosingCircle(contours[i], circleCenter, circleRadius);
        }
    }

    drawContours(dst, contours, largest_contour_index, color, CV_FILLED, 8, hierarchy); // Draw the largest contour using previously stored index.
    circle(newFrame, circleCenter, circleRadius, Scalar(255, 0, 0), 2, 8, 0);

    float circleEnvelopeArea = 3.1412 * circleRadius * circleRadius;
    float fillRatio=largest_area / circleEnvelopeArea;
    cout << fillRatio << endl;
    //cout << circleRadius << endl;
    if (0.82 < fillRatio) { 
         cout << "ALIGN" << endl;
         direction = 11;
         cout <<fillRatio<< endl;
    }
}

void trackObject(Moments& oMoments)
{

    double dM01 = oMoments.m01;
    double dM10 = oMoments.m10;
    double dArea = oMoments.m00;
    int posX = dM10 / dArea;
    int posY = dM01 / dArea;

    driverOutput(posX, posY); //enable motor output

    iLastX = posX; //return position
    iLastY = posY;
}

void driverOutput(int& posX, int& posY)
{
    double lockTolerance = 0.05;
    cv::Size s = newFrame.size();
    int height = s.height;
    int mid = s.width / 2;
    int offset = 0 ;

    int lowRange = mid * (1 - lockTolerance)+offset;
    int HighRange = mid * (1 + lockTolerance)+offset;
    int stopRange = 0.8 * height; //adjust the constant for stop range
	if (direction==11){
}
else if (posX > HighRange) {
        cout << "RIGHT" << endl;
        //cout << posX << endl;
        direction = 10;
    }

    else if (posX < lowRange) {
        cout << "LEFT" << endl;
        direction = 01;
    }

    //else{
    //    direction=00;
    //}
    //if (posY > stopRange)
    //    cout << "STOP" << endl;
}

void motorGPIO()
{
    wiringPiSetup();
    pinMode(23, OUTPUT);
    pinMode(24, OUTPUT);
    pinMode(25, OUTPUT);
    pinMode(27, OUTPUT);
    pinMode(28, OUTPUT);
    pinMode(29, OUTPUT);
    digitalWrite(25, HIGH);
    digitalWrite(29, HIGH);
    digitalWrite(24, LOW);
    digitalWrite(28, LOW);
    digitalWrite(23, LOW);
    digitalWrite(27, LOW);
}

void motorDrive()
{  
    switch (direction) {
     case 11:{
         digitalWrite(23, HIGH);
         digitalWrite(27, HIGH);
         break;
    }
    case 10:{
        digitalWrite(23, LOW);
        digitalWrite(27, HIGH);
        break;
    }
    case 01:{
        digitalWrite(23, HIGH);
        digitalWrite(27, LOW);
        break;
    }
    case 00:{
        digitalWrite(23, LOW);
        digitalWrite(27, LOW);
        break;
    }
    default:{
        digitalWrite(23, LOW);
        digitalWrite(27, LOW);
        break;
    }
    }
}
void displayCOMP(Mat dst){
    namedWindow("Denoised", 1);
    namedWindow("Thresholded", 1);
    resize(newFrame, newFrame, Size(), 2, 2, INTER_LINEAR);
    resize(newFrameThresholded, newFrameThresholded, Size(),1, 1, INTER_LINEAR);
    imshow("Denoised", newFrame); //show camera input at thr. window in HSV
    imshow("Control Bar", dst); //show camera input at thr. window in HSV
}
void displayRASP(Mat dst)
{
    namedWindow("Denoised", 1);
    //namedWindow("Thresholded", 1);
    resize(newFrame, newFrame, Size(), 2, 2, INTER_LINEAR);
    //resize(newFrameThresholded, newFrameThresholded, Size(),1, 1, INTER_LINEAR);
    imshow("Denoised", newFrame); //show camera input at thr. window in HSV
    imshow("Control Bar", dst); //show camera input at thr. window in HSV
}

void setBlue(){
    iLowH = 41;
    iHighH = 108;
    iLowS = 98;
    iHighS = 255;
    iLowV = 99;
    iHighV = 255;
}
void setRed(){
    iLowH = 35;
    iHighH = 179;
    iLowS = 100;
    iHighS = 255;
    iLowV = 100;
    iHighV = 255;
}
void setYellow(){
    iLowH = 0;
    iHighH = 40;
    iLowS = 0;
    iHighS = 255;
    iLowV = 220;
    iHighV = 255;
}