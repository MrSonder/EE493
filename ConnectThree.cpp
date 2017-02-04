#include "ee493_util.cpp"

void selectObjectLargestArea(Mat& dst); // filters out the closes object
void trackObject(Mat dst, bool arduinoConnected);
void directionData(int& posX, int& posY); // send motor signals
Point2f findCenter(Mat image);
Mat drawCenterLine(Mat imageIn, int colorFront);
Mat detectObject(Mat image, int colorFront);
Mat templateExtract(Mat image, int colorFront);
Mat selectObjectAllign(Mat image, int colorFront);

double fillRatio(vector<Point> contour);
Mat newFrameThresholded;
Mat dst;
VideoCapture camera(0);

double thresh=10;

int main(int argc, char* argv[])
{
    loadWindows();

    while (true) {
        direction = 00;
        camera >> newFrame; // get a new frame from camera
        resize(newFrame, newFrame, Size(), resizeRatio, resizeRatio,INTER_LINEAR);

        templateExtract(newFrame,colorFront);
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

Mat templateExtract(Mat image, int colorFront)
{
    Mat imageOUT;
    double maxval=0;
    setColor(colorFront);
    threshold(image, imageOUT, thresh, maxval, THRESH_TOZERO);
    return imageOUT;
}


Mat thresholdImage(Mat image, int colorFront)
{
    Mat imageOUT;
    setColor(colorFront);
    GaussianBlur(image, imageOUT, cv::Size(5, 5), 2*filterRatio+1, 2*filterRatio+1);
    cvtColor(imageOUT, imageOUT, COLOR_BGR2HSV);
    inRange(imageOUT, Scalar(iLowH, iLowS, iLowV),
        Scalar(iHighH, iHighS, iHighV), imageOUT);   
    erode(imageOUT, imageOUT, cv::Mat(), cv::Point(-1, -1), 1);
    dilate(imageOUT, imageOUT, cv::Mat(), cv::Point(-1, -1), dilateIter);
    return imageOUT;
}


Mat drawCenterLine(Mat imageIn, int colorFront) //returns angle of the line
{
    Mat image1,image2;

    image1 = selectObjectAllign(imageIn, 'G');
    Point2f point_mid = findCenter(image1);

    image2 = selectObjectAllign(imageIn, colorFront);
    Point2f point_front = findCenter(image2);

    drawStraightLine(&imageIn,point_mid,point_front);

    image1=image1+image2;

    return image1;
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

Mat selectObjectAllign(Mat image, int colorFront) 
{
    //selects the object with smallest angle from a binary image
    Mat imageSelected = Mat::zeros(newFrame.size(), CV_8UC1);
    vector<vector<Point> > contours; // Vector for storing contour
    vector<Vec4i> hierarchy;
    Point_<float> circleCenter; // bounding circle params
    int largest_area = 0;
    int largest_contour_index = 0;

    int largest_fillRatio = 0;
    Mat imageContours = thresholdImage(image, colorFront);
    imshow("Test Window",imageContours);
    findContours(imageContours, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

    for (int i = 0; i < contours.size(); i++) // iterate through each contour.
    {
        double a = contourArea(contours[i], false); //  Find the area of contour
        double b = fillRatio(contours[i]);
        /*if (a > largest_area) {
            largest_area = a;
            largest_contour_index = i; // Store the index of largest contour
            minEnclosingCircle(contours[i], circleCenter, circleRadius);
        }*/
        cout<<a<<"\n";
        if (contourArea(contours[i], false)>2000){
        if (b > largest_fillRatio) {
            largest_fillRatio = b;
            largest_contour_index = i; // Store the index of largest contour
            minEnclosingCircle(contours[i], circleCenter, circleRadius);
        }
        }

    }   
   // if (largest_area > 25) {
        drawContours(imageSelected, contours, largest_contour_index, Scalar(255, 0, 0), CV_FILLED, 8, hierarchy); 
        //circle(newFrame, circleCenter, circleRadius, Scalar(255, 0, 0), 2, 8, 0);
        object_exist = true;
   /* }
    else {
        object_exist = false;
    }*/


    return imageSelected;
}


double fillRatio(vector<Point> contour)  //unused
{
    Point_<float> circleCenter;
    double areaCont = contourArea(contour, false); 
    minEnclosingCircle(contour, circleCenter, circleRadius);
    double circleEnvelopeArea = PI * circleRadius * circleRadius;
    double fillRatio = areaCont / (0.9 * circleEnvelopeArea);
    angle = acos(fillRatio) * 180.0 / 3.1412;
    if (fillRatio > 1.0)
        angle = 0;
    return fillRatio;
}


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
