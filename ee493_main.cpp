#include "ee493_opencv.cpp"
#include "ee493_future.cpp"
#include "ee493_arduino.cpp"

int main(int argc, char* argv[])
{
    cout<<"ArduinoConnected:"<< ArduinoConnected <<endl;
    
    //calibrateThreshold('Y');
    //turnTimer(1);
    //double time_1 = calibrateTurnTime(150, -150, 2);
    

    while (true) {     
        camera >> newFrame; // get a new frame from camera
        resize(newFrame, newFrame, Size(), resizeRatio, resizeRatio, INTER_LINEAR);
        goTowardsObject(150, newFrame, colorFront, ArduinoConnected);

        //txArduino("050F050R");
        //usleep( 100000 );
        if ( waitKey(10) == 27)
        break;  
    }
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

void goTowardsObject(int base_speed, Mat img, int colorFront, bool ArduinoConnected)
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
        txArduino(driveMotor(base_speed + speed, base_speed - speed));
    }

     /*  
    else if ( abs(center.x) > 55 || abs(center.x) < 1000){
            speed = speed / 2;
            txString="X" + int_to_XXX(speed)+int_to_XXX(-speed);
    }*/
    else
       txArduino(driveMotor(0, 0));
   
    //statusBar(center);
    
}
