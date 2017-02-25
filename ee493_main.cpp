#include "ee493_util.cpp"
#include "ee493_future.cpp"
#include <unistd.h>
Point2f findCenter(Mat image);
Point2f drawCenterLine(Mat imageIn, int colorFront);
Mat selectObjectAllign(Mat image, int colorFront, int object);
double fillRatio(vector<Point> contour, int object);
Mat selectObject(Mat image, int colorFront, int object);
Mat largestArea(Mat image, int object);
void rectangleMask(Mat image, RotatedRect rectangle);
void templateExtract(Mat imageIn, int color);
void trackObject2(Mat img, int colorFront);  
void statusBar(Point2f center);

int main(int argc, char* argv[])
{
    
    //calibrateThreshold('B');
    while (true) {

        camera >> newFrame; // get a new frame from camera
        resize(newFrame, newFrame, Size(), resizeRatio, resizeRatio, INTER_LINEAR);

        //templateExtract(newFrame, colorFront);
        //templateMatching(newFrame);

        trackObject2(newFrame, colorFront);
        usleep( 100000 );
        if ( waitKey(100) == 27)
        break;  
    }
}


void templateExtract(Mat imageIn, int color)
{
    Mat image1,image2;
    image1 = selectObject(imageIn, 'Y', 'R');
    imageIn.copyTo(image2, image1);
    dispImage(image2, "Mask",4);
}

void rectangleMask(Mat image, RotatedRect rectangle)
{
    Mat imageRect = Mat::zeros( image.size(), CV_8UC1 );
    Point2f rect_points[4]; rectangle.points( rect_points );
    for( int j = 0; j < 4; j++ )
          line( imageRect, rect_points[j], rect_points[(j+1)%4], Scalar(255, 0, 0));

    Mat imageMask = largestArea(imageRect, 'N');

    Mat imageOut;

    newFrame.copyTo(imageOut, imageMask);
    //dispImage(imageOut, "masked" ,4);
}

Mat selectObject(Mat image, int colorFront, int object) 
{
    Mat imageContours = thresholdImage(image, colorFront, false);
    Mat imageSelected = largestArea(imageContours, object);
    return imageSelected;
}


Mat largestArea(Mat image, int object)
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

Point2f drawCenterLine(Mat imageIn, int colorFront) 
{
    // draws a line throught the object and returns angle of the line
    Mat image1,image2;
    Mat image3,image4;

    image1 = selectObject(imageIn, 'Y', 'R');
    Point2f point_mid = findCenter(image1);

    //putText(imageIn, format("Angle : %d",angle), point_mid, FONT_HERSHEY_COMPLEX_SMALL, 0.5, Scalar(0, 0, 0));

    image2 = selectObject(imageIn, colorFront, 'C');
    Point2f point_front = findCenter(image2);

    image4 = Mat::zeros(imageIn.size(), CV_8UC3);

    drawStraightLine(&image4,point_mid,point_front);

    imageIn = imageIn + image4;
    dispImage(image2, "Threshold", 2);
    dispImage(imageIn, "Source", 0);
    
    return point_front;
}

void trackObject2(Mat img, int colorFront)
{
//tracks object with an image taken from camera
//uses global angle variable 

    Point2f center =  drawCenterLine(img, colorFront);

    int mid_y = img.rows/2;
    int mid_x = img.cols/2;
    Point2f test(mid_x , mid_y);
    center = center - test;

    int speed = (int) abs(center.x) + 20;


    ostringstream speedString;
    speedString<<speed;

    string speedSTR = speedString.str();
    string leftWheel;
    string rightWheel;

    if (speedSTR.length() == 2) {
        leftWheel = leftWheel.append("0");
        rightWheel = rightWheel.append("0");

    } 

    leftWheel.append( speedString.str() );
    rightWheel.append( speedString.str() );




    string txString="";
    if(center.x > 0){
        leftWheel.append("F");
        rightWheel.append("R");
    }

    if(center.x <= 0){
        leftWheel.append("R");
        rightWheel.append("F");
    }

   
    txString = leftWheel + rightWheel ;
    //printf("%s\n\r", txString.c_str());
 
    //txArduino(txString);


    speedString << int(getFPS());
    txString.append(speedString.str());

    txTerminal(txString);
    //statusBar(center);

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
