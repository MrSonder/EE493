#include "ee493_util.cpp"

void selectObjectLargestArea(Mat& dst); // filters out the closes object
void directionData(int& posX, int& posY); // send motor signals
Point2f findCenter(Mat image);
Mat drawCenterLine(Mat imageIn, int colorFront);
Mat detectObject(Mat image, int colorFront);
Mat selectObjectAllign(Mat image, int colorFront, int object);
Mat thresholdImage(Mat image, int colorFront, bool calibration);
double fillRatio(vector<Point> contour, int object);

void calibrateThreshold(int color);
int main(int argc, char* argv[])
{
    
    //calibrateThreshold('Y');
    while (true) {
        direction = 00;
        camera >> newFrame; // get a new frame from camera
        resize(newFrame, newFrame, Size(), resizeRatio, resizeRatio,INTER_LINEAR);


        //templateExtract(newFrame,colorFront);
        dst=drawCenterLine(newFrame,colorFront);
        trackObject(dst,arduinoConnected); // tx2Arduino() implemented inside track object
        displayRASP(dst);
        
        int c = waitKey(1);
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
        imshow(windowTitle, image);
        int c = waitKey(10);
        if ((char)c == 27) {
            cvDestroyWindow(windowTitle);
            exit(0);
        }

    }
}


Mat drawCenterLine(Mat imageIn, int colorFront) 
{
    // draws a line throught the object and returns angle of the line
    Mat image1,image2;
    Mat image3,image4;
    image1 = selectObjectAllign(imageIn, 'Y', 'R');
    Point2f point_mid = findCenter(image1);

    image2 = selectObjectAllign(imageIn, colorFront, 'C');
    Point2f point_front = findCenter(image2);

  
    drawStraightLine(&imageIn,point_mid,point_front);

    imageIn.copyTo(image3, image2);
    imshow("Test Window",image3);
    return image1;
}

Mat selectObjectAllign(Mat image, int colorFront, int object) 
{
    //selects the object with the smallest angle from a binary image
    Mat imageSelected = Mat::zeros(image.size(), CV_8UC1);
    vector<vector<Point> > contours; // Vector for storing contour
    vector<Vec4i> hierarchy;
    Point_<float> circleCenter; // bounding circle params
    //Rect boundRect;
    RotatedRect boundRect;
    int largest_area = 0;
    int largest_contour_index = 0;
    int largest_fillRatio = 0;

    Mat imageContours = thresholdImage(image, colorFront, false);
    findContours(imageContours, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE );

    for (int i = 0; i < contours.size(); i++) // iterate through each contour.
    {
        double b = fillRatio(contours[i], object);
        if (contourArea(contours[i], false)>2000){
        if (b > largest_fillRatio) {
            largest_fillRatio = b;
            largest_contour_index = i; // Store the index of largest contour
            minEnclosingCircle(contours[i], circleCenter, circleRadius);
            boundRect = minAreaRect(contours[i]);
        }
        }

    }   
    drawContours(imageSelected, contours, largest_contour_index, Scalar(255, 0, 0), CV_FILLED, 8, hierarchy);
        
    object_exist = true;
    return imageSelected;
}




/*
void selectObjectLargestArea(Mat& dst) //selects the object with largest area
{
    vector<vector<Point> > contours; // Vector for storing contour
    vector<Vec4i> hierarchy;
    Point_<float> circleCenter; // bounding circle params
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

*/


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

Mat thresholdImage(Mat image, int colorFront, bool calibration )
{
    Mat imageOUT=image.clone();
    if (!calibration)
    {
        setColor(colorFront);
    }
    //GaussianBlur(image, imageOUT, cv::Size(5, 5), 2*filterRatio+1, 2*filterRatio+1);
    cvtColor(imageOUT, imageOUT, COLOR_BGR2HSV);
    inRange(imageOUT, Scalar(iLowH, iLowS, iLowV),
        Scalar(iHighH, iHighS, iHighV), imageOUT);   
    erode(imageOUT, imageOUT, cv::Mat(), cv::Point(-1, -1), 6);
    dilate(imageOUT, imageOUT, cv::Mat(), cv::Point(-1, -1), 4);
    medianBlur(imageOUT, imageOUT, 5);
    return imageOUT;
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
