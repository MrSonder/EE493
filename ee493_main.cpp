#include "ee493_opencv.cpp"
#include "ee493_future.cpp"
#include "ee493_arduino.cpp"
#include <cmath>
#include <iostream>
#include <fstream>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/stitching.hpp"
//#include "opencv2/stitching/stitcher.hpp"
void getPanaroma();
bool searchColor(Mat img, int color);
void goTowardsObjectMethod(int color);
void searchColorMethod(int color);
void switchToCamera(int index);
void startCamera(int index);
void getTriangleArray(Mat image);
void calibrateDoubleCamera();
Mat getTriangleContours(Mat image, int trig_index_1, int trig_index_2);
Point2f getBoardSlot(Mat img, int trig_index_1, int trig_intex_2);
int cam_index_1 = 1;
int cam_index_2 = 1 - cam_index_1;

void goTowardsSlot(int base_speed, Mat img, int trig_index, bool ArduinoConnected, int y_threshold, int turn_rate_divider);

int main(int argc, char *argv[])
{
    time(&start);
    cout << "ArduinoConnected:" << ArduinoConnected << endl;
    int colorFront = 'B';
    int colorFlag = 'P';
    
    startCamera(0);
    while(true){
    camera >> newFrame;
    resize(newFrame, newFrame, Size(), 0.6, 0.6, INTER_LINEAR);
    //getObjectOfColor(newFrame, 'P');
    dispImage(newFrame, "xx", 0);
    }
    //calibrateThreshold('P');
    //calibrateDoubleCamera();
/*
    Mat image_temp, image;
    Mat image_temp2;
    Mat image_thresh;
    Mat temp2;
    Point2f point_cyc;
    int game[7][7];
      newFrame = imread("board3.png");
    
    Point2f point;
    int trig_x[7];
    int trig_avg=0;
    cout<<"triangles"<<endl;
    for(int i=0; i<=6; i++){
        point = getBoardSlot(newFrame, i, i);
        trig_x[i] = point.x;
        cout<<point.y<<endl;
        trig_avg = trig_avg + point.y/7;
    }
    cout<< trig_avg <<endl;

cout<<"cylnders"<<endl;

  
    image = templateMatching(templateExtract(newFrame, 'B'));
    Mat imageSelected = Mat::zeros(image.size(), CV_8UC1);
    vector<vector<Point> > contours; // Vector for storing contour
    vector<Vec4i> hierarchy;
    image.convertTo(image, CV_8UC1);
    cout<<image.type()<<endl;
    findContours(image, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    for (int i = 0; i < contours.size(); i++) // iterate through each contour.
    {
    imageSelected = Mat::zeros(image.size(), CV_8UC1);
    drawContours(imageSelected, contours, i, Scalar(255, 0, 0), CV_FILLED, 8, hierarchy);
    Point2f point_cyc = findCenter(imageSelected);
    cout<<point_cyc.y<<endl;
    }
    dispImage(newFrame, "matching", 3);

        //getBoardSlot(newFrame, 0);
        //

*/

    //getPanaroma();



    searchColorMethod(colorFront);
    goTowardsObjectMethod(colorFront);

    //go to flag
    searchColorMethod(colorFlag);
    goTowardsObjectMethod(colorFlag);


    return 0;
}

void getPanaroma()
{
    startCamera(0);

    Mat pano;
    Stitcher stitcher = Stitcher::createDefault(true);
    int b = 0;

    while (true)
    {
        Mat pano;
        vector<Mat> imgs;
        int k = 0;
        while (b != 'd')
        {
            b = waitKey(100);
            camera.read(newFrame);
            resize(newFrame, newFrame, Size(), 0.6, 0.6, INTER_LINEAR);

            if (b == 'r')
            {
                b = 0;
                cout << "save" << endl;
                imgs.push_back(newFrame);

                Stitcher::Status status = stitcher.stitch(imgs, pano);

                if (status == Stitcher::OK)
                {
                    k++;
                    dispImage(pano, "pano", 0);
                    if (k == 5)
                    {
                        break;
                    }
                }
            }
            dispImage(newFrame, "frame", 5);
        }
        b = 0;
        cout << "pano" << endl;
    }
}

void calibrateDoubleCamera()
{
    startCamera(0);
    while (true)
    {
        camera.read(newFrame);
        dispImage(newFrame, "Source", 4);
        int b = waitKey(1);
        b = b - 48;
        if (b == 1)
        {
            cout << b << endl;
            switchToCamera(0);
        }
        if (b == 2)
        {
            cout << b << endl;
            switchToCamera(0);
        }
    }
}

Point2f getBoardSlot(Mat img, int trig_index_1, int trig_intex_2)
{
    Mat imageContours = thresholdImage(img, 'w', false);
    flip(imageContours, imageContours, 1);
    transpose(imageContours, imageContours);
    imageContours = getTriangleContours(imageContours, trig_index_1, trig_intex_2);
    flip(imageContours, imageContours, 0);
    transpose(imageContours, imageContours);
    Point2f point = findCenter(imageContours);
    dispImage(imageContours, "3", 4);
    return point;
}

Mat getTriangleContours(Mat image, int trig_index_1, int trig_index_2)
{
    Mat imageSelected = Mat::zeros(image.size(), CV_8UC1);
    vector<vector<Point> > contours; // Vector for storing contour
    vector<Vec4i> hierarchy;

    findContours(image, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

    for (int i = trig_index_1; i <= trig_index_2; i++) // iterate through each contour.
    {
    drawContours(imageSelected, contours, i, Scalar(255, 0, 0), CV_FILLED, 8, hierarchy);
    }
    return imageSelected;
}

void getFrameFromCamera(){
    
}


/*
void goTowardsSlot(int base_speed, Mat img, int trig_index, bool ArduinoConnected, int y_threshold, int turn_rate_divider)
{
    Point2f center = getBoardSlot(img, trig_index);

    int mid_y = img.rows / 2;
    int mid_x = img.cols / 2;
    Point2f test(mid_x, mid_y);
    center = center - test;
    string txString;
    int speed = (center.x) / turn_rate_divider;
    if (abs(center.x) < 550)
    {
        txArduino(driveMotor(base_speed + speed, base_speed - speed));
    }
    else
    {
        txArduino(driveMotor(0, 0));
    }

    if (center.y > y_threshold)
    {
        //driveMotorForSeconds(1.5, 100, 100);
        txArduino(driveMotor(0, 0));
    }
}
*/
void switchToCamera(int index)
{
    camera.release();
    startCamera(index);
}

void startCamera(int index)
{
    camera.open(index);
    while (!camera.read(newFrame))
    {
        continue;
    }
}
void searchColorMethod(int color)
{
    //camera.release();
    startCamera(cam_index_2);
    object_exist = searchColor(newFrame, color);

    while (!object_exist)
    {
        camera >> newFrame;
        txArduino(driveMotor(-70, 70));
        resize(newFrame, newFrame, Size(), resizeRatio, resizeRatio, INTER_LINEAR);
        object_exist = searchColor(newFrame, color);
        //dispImage(newFrame, "search", 0);
        
    }
    driveMotorForSeconds(1, 50, -50);
    txArduino(driveMotor(0, 0));
}

void goTowardsObjectMethod(int color)
{
    startCamera(cam_index_1);
    object_exist = true;
    while (object_exist)
    {
        camera >> newFrame;
        resize(newFrame, newFrame, Size(), resizeRatio, resizeRatio, INTER_LINEAR);
        object_exist = goTowardsObject(150, newFrame, color, ArduinoConnected, 110, 5);
    }
    

    camera.release();

    startCamera(cam_index_2);
    object_exist = true;
    while (object_exist)
    {
        camera >> newFrame;
        resize(newFrame, newFrame, Size(), resizeRatio, resizeRatio, INTER_LINEAR);
        object_exist = goTowardsObject(150, newFrame, color, ArduinoConnected, 105, 4);
    }
}

bool goTowardsObject(int base_speed, Mat img, int colorFront, bool ArduinoConnected, int y_threshold, int turn_rate_divider)
{
    //tracks object with an image taken from camera
    //uses global angle variable

    Point2f center = drawCenterLine(img, colorFront);
    bool object_exist = true;
    int mid_y = img.rows / 2;
    int mid_x = img.cols / 2;
    Point2f test(mid_x, mid_y);
    center = center - test;
    string txString;
    int speed = (center.x) / turn_rate_divider;
    if (abs(center.x) < 550)
    {
        txArduino(driveMotor(base_speed + speed, base_speed - speed));
    }
    else
    {
        txArduino(driveMotor(0, 0));
    }

    if (center.y > y_threshold)
    {
        //driveMotorForSeconds(1.5, 100, 100);
        txArduino(driveMotor(0, 0));
        object_exist = false;
    }
    return object_exist;
}

bool searchColor(Mat img, int color)
{
    //tracks object with an image taken from camera
    //uses global angle variable

    Point2f center = drawCenterLine(img, color);
    bool object_exist = false;
    int mid_y = img.rows / 2;
    int mid_x = img.cols / 2;
    Point2f test(mid_x, mid_y);
    center = center - test;

    if ((center.x > -100 and center.x < 100) and (center.y < 80 and center.y > -20))
    {
        object_exist = true;
    }
    return object_exist;
}

Mat getObjectOfColor(Mat image, int colorFront)
{
    Mat imageContours = thresholdImage(image, colorFront, false);
    Mat imageSelected = getLargestArea(imageContours, colorFront);
    return imageSelected;
}

Mat getLargestArea(Mat image, int colorFront)
{
    Mat imageSelected = Mat::zeros(image.size(), CV_8UC1);
    vector<vector<Point> > contours; // Vector for storing contour
    vector<Vec4i> hierarchy;
    Point_<float> circleCenter; // bounding circle params
    //Rect boundRect;
    RotatedRect boundRect;
    int largest_area = 0;
    int largest_contour_index = 0;
    int largest_fillRatio = 0;
    int area_threshold = 0;
    
    if(colorFront == 'R' || colorFront == 'B')
    {
        cout << 'C' <<endl;
        area_threshold = 0;
    }
    if(colorFront == 'P')
    {
        cout << 'P' <<endl;
        area_threshold = 500;
    }

    findContours(image, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    
    double a ;
    for (int i = 0; i < contours.size(); i++) // iterate through each contour.
    {
        double a = contourArea(contours[i], false);
        //if (contourArea(contours[i], false)>2000){
        if (a > largest_area && a > area_threshold)
        {
            largest_area = a;
            largest_contour_index = i; // Store the index of largest contour
            minEnclosingCircle(contours[i], circleCenter, circleRadius);
            boundRect = minAreaRect(contours[i]);
        }
    }
    cout<<endl<<largest_area<<endl;

    


    drawContours(imageSelected, contours, largest_contour_index, Scalar(255, 0, 0), CV_FILLED, 8, hierarchy);

    return imageSelected;
}

Point2f drawCenterLine(Mat imageIn, int colorFront)
{
    // draws a line through the object and returns angle of the line
    Mat image1, image2;
    Mat image3, image4;

    image1 = getObjectOfColor(imageIn, 'Y');
    Point2f point_mid = findCenter(image1);

    //putText(imageIn, format("Angle : %d",angle), point_mid, FONT_HERSHEY_COMPLEX_SMALL, 0.5, Scalar(0, 0, 0));

    image2 = getObjectOfColor(imageIn, colorFront);
    Point2f point_front = findCenter(image2);

    image4 = Mat::zeros(imageIn.size(), CV_8UC3);
    drawStraightLine(&image4, point_mid, point_front);

    imageIn = imageIn + image4;
    if (!ArduinoConnected)
    {
        dispImage(image2, "Threshold", 2);
        dispImage(imageIn, "Source", 0);
    }

    return point_front;
}
