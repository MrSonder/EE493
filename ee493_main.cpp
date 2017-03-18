#include "ee493_opencv.cpp"
#include "ee493_future.cpp"
#include "ee493_arduino.cpp"
#include <cmath>
bool searchColor(Mat img, int color);
void goTowardsObjectMethod(int color);
void searchColorMethod(int color);

int cam_index_1 = 1;
int cam_index_2 = 1 - cam_index_1;

int main(int argc, char *argv[])
{
    time(&start);
    cout << "ArduinoConnected:" << ArduinoConnected << endl;
    int colorFront = 'B';
    int colorFlag = 'P';
    //calibrateThreshold('R');
    //calibrateThreshold('b');


    Mat image1, image2;
    Mat image3, image4;


    
    while(true){
        newFrame = imread("board2.png");
        newFrame = getObjectOfColor(newFrame, 'b', 'R');
        dispImage(newFrame, "im", 0);
        //templateExtract(newFrame, 'B');
}
        //templateMatching(newFrame);


/*
    searchColorMethod(colorFront);
    goTowardsObjectMethod(colorFront);

    //go to flag
    searchColorMethod(colorFlag);
    goTowardsObjectMethod(colorFlag);
*/



        
       
       
       
       
       
       
        /*int b = waitKey(1);	
        if ( b == 'q')
        {camera.release(); camera.open(0); while(!camera.read(newFrame) ) {continue;}}
    	if ( b == 'w')
        {camera.release(); camera.open(1); while(!camera.read(newFrame) ) {continue;}}
    	//cout << "asodjk" <<endl;
    	if ( b == 27) break;*/

    return 0;
}

void searchColorMethod(int color){
    //camera.release();
    camera.open(cam_index_2);
    while (!camera.read(newFrame))
    {
        continue;
    }

    object_exist = searchColor(newFrame, color);

    while (!object_exist)
    {
        camera >> newFrame;
        txArduino(driveMotor(70, -70));
        resize(newFrame, newFrame, Size(), resizeRatio, resizeRatio, INTER_LINEAR);
        object_exist = searchColor(newFrame, color);
    }
    txArduino(driveMotor(-70, 70));
    txArduino(driveMotor(0, 0));
}

void goTowardsObjectMethod(int color)
{
    
    camera.open(cam_index_1);
    while (!camera.read(newFrame))
    {
        continue;
    }
    object_exist = true;
    while (object_exist)
    {
        camera >> newFrame;
        resize(newFrame, newFrame, Size(), resizeRatio, resizeRatio, INTER_LINEAR);
        object_exist = goTowardsObject(150, newFrame, color, ArduinoConnected, 110, 5);
    }

    camera.release();

    camera.open(cam_index_2);
    while (!camera.read(newFrame))
    {
        continue;
    }

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

    cout << "\n"
         << center.y << "\n"
         << endl;
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

Mat getObjectOfColor(Mat image, int colorFront, int object)
{
    Mat imageContours = thresholdImage(image, colorFront, false);
    Mat imageSelected = getLargestArea(imageContours, object);
    return imageSelected;
}

Mat getLargestArea(Mat image, int object)
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

    findContours(image, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

    for (int i = 0; i < contours.size(); i++) // iterate through each contour.
    {
        double a = contourArea(contours[i], false);
        //if (contourArea(contours[i], false)>2000){
        if (a > largest_area)
        {
            largest_area = a;
            largest_contour_index = i; // Store the index of largest contour
            minEnclosingCircle(contours[i], circleCenter, circleRadius);
            boundRect = minAreaRect(contours[i]);
        }
    }

    if (object == 'R')
        rectangleMask(image, boundRect);

    drawContours(imageSelected, contours, largest_contour_index, Scalar(255, 0, 0), CV_FILLED, 8, hierarchy);

    return imageSelected;
}

Point2f drawCenterLine(Mat imageIn, int colorFront)
{
    // draws a line through the object and returns angle of the line
    Mat image1, image2;
    Mat image3, image4;

    image1 = getObjectOfColor(imageIn, 'Y', 'R');
    Point2f point_mid = findCenter(image1);

    //putText(imageIn, format("Angle : %d",angle), point_mid, FONT_HERSHEY_COMPLEX_SMALL, 0.5, Scalar(0, 0, 0));

    image2 = getObjectOfColor(imageIn, colorFront, 'C');
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