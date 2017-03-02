#include "ee493_util.cpp"
#include "ee493_future.cpp"

String driveMotor(int leftMotor_int, int RightMotor_int);
String int_to_XXX(int speedInt);
bool ArduinoConnected = true;

int main(int argc, char* argv[])
{
    //system("stty -F /dev/ttyUSB0 cs8 9600 ignbrk -brkint -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts");
        
    //calibrateThreshold('Y');
    while (true) {
        camera >> newFrame; // get a new frame from camera
        resize(newFrame, newFrame, Size(), resizeRatio, resizeRatio, INTER_LINEAR);
        goTowardsObject(newFrame, colorFront, ArduinoConnected);


        //txArduino("050F050R");
        /*
        int L;
        int R;
        cin >> L;
        cin >> R;
        txArduino(driveMotor(L,R));
        */
        //usleep( 100000 );

        if ( waitKey(10) == 27)
        break;  
    }
}


void allignRobotAxial(int angle){
    //assuming angle=90 || -90 when alligned


}

void turnRobot(int turn_angle){
    double time_full_cycle = 2000;
    double time_per_degree =  time_full_cycle / 360 ;
    int ms_us = 1000;
    double turn_time = time_per_degree * turn_angle * ms_us;

    if (turn_angle>0){
        
    }

    usleep(turn_time);
}


String driveMotor(int leftMotor_int, int RightMotor_int){
    string data = "X" + int_to_XXX( leftMotor_int ) + int_to_XXX( RightMotor_int );
    return data;
}

String int_to_XXX(int speedInt){
    ostringstream stringStream;
    stringStream<< abs(speedInt);
    string speedString = stringStream.str();
    string speedStringFormatted = "";

    while(speedString.length() != 3){
        speedString = speedString.append("0");
        speedStringFormatted = speedStringFormatted.append("0");
    }
    speedStringFormatted.append(stringStream.str());

    if(speedInt > 0){
        speedStringFormatted = speedStringFormatted.append("F");
    }
    else{
        speedStringFormatted = speedStringFormatted.append("R");
    }

    return speedStringFormatted;
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

    findContours(image, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE );

    for (int i = 0; i < contours.size(); i++) // iterate through each contour.
    {
        double a = contourArea(contours[i], false);
        //if (contourArea(contours[i], false)>2000){
        if (a > largest_area) {
            largest_area = a;
            largest_contour_index = i; // Store the index of largest contour
            minEnclosingCircle(contours[i], circleCenter, circleRadius);
            boundRect = minAreaRect(contours[i]);
        }

    }

    if (object=='R')
        rectangleMask(image, boundRect);
    
    drawContours(imageSelected, contours, largest_contour_index, Scalar(255, 0, 0), CV_FILLED, 8, hierarchy);

    object_exist = true;
    return imageSelected;
}


Point2f drawCenterLine(Mat imageIn, int colorFront) 
{
    // draws a line through the object and returns angle of the line
    Mat image1,image2;
    Mat image3,image4;

    image1 = getObjectOfColor(imageIn, 'Y', 'R');
    Point2f point_mid = findCenter(image1);

    //putText(imageIn, format("Angle : %d",angle), point_mid, FONT_HERSHEY_COMPLEX_SMALL, 0.5, Scalar(0, 0, 0));

    image2 = getObjectOfColor(imageIn, colorFront, 'C');
    Point2f point_front = findCenter(image2);

    image4 = Mat::zeros(imageIn.size(), CV_8UC3);
    drawStraightLine(&image4,point_mid,point_front);

    imageIn = imageIn + image4;
    if (!ArduinoConnected){
        dispImage(image2, "Threshold", 2);
        dispImage(imageIn, "Source", 0);
    }
    
    return point_front;
}

void goTowardsObject(Mat img, int colorFront, bool ArduinoConnected)
{
//tracks object with an image taken from camera
//uses global angle variable 

    Point2f center =  drawCenterLine(img, colorFront);

    int mid_y = img.rows/2;
    int mid_x = img.cols/2;
    Point2f test(mid_x , mid_y);
    center = center - test;
    string txString;
     int speed = (center.x)/2 + 20;
    
    if(abs(center.x) < 550){
        txString = "X" + int_to_XXX(speed)+int_to_XXX(-speed);

    }

     /*  
    else if ( abs(center.x) > 55 || abs(center.x) < 1000){
            speed = speed / 2;
            txString="X" + int_to_XXX(speed)+int_to_XXX(-speed);
    }*/
    else
        txString="X000F000F";
//printf("%s\n\r", txString.c_str());
    if (ArduinoConnected)
        txArduino(txString);
    else
        txTerminal(txString);
    
    //statusBar(center);*/
    
}
