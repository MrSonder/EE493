#include "ee493_util.cpp"

void selectObjectLargestArea(Mat& dst); // filters out the closes object
void trackObject(Mat dst, bool arduinoConnected);
void directionData(int& posX, int& posY); // send motor signals
void drawStraightLine(Mat *img, Point2f p1, Point2f p2);
void findCenter(Mat image, Point2f& point);
void selectObject2(Mat& imageIN);
Mat drawCenterLine2(Mat imageIn, int colorFront);
Mat detectObject(Mat image, int colorFront);


Mat newFrameThresholded;
Mat dst;
VideoCapture camera(0);


int main(int argc, char* argv[])
{
    loadWindows();

    while (true) {
        direction = 00;
        camera >> newFrame; // get a new frame from camera
        resize(newFrame, newFrame, Size(), resizeRatio, resizeRatio,INTER_LINEAR);

        dst = Mat::zeros(newFrame.size(), CV_8UC1);

        newFrameThresholded=drawCenterLine2(newFrame, colorFront);
        selectObjectLargestArea(dst);
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

Mat drawCenterLine2(Mat imageIn, int colorFront) //returns angle of the line
{
    Mat image1,image2;

    image1=thresholdImage(imageIn, 'G');
    Point2f point_mid;
    selectObject2(image1);
    findCenter(image1, point_mid);

    image2=thresholdImage(imageIn, colorFront);
    Point2f point_front;
    selectObject2(image2);
    findCenter(image2, point_front);

    drawStraightLine(&newFrame,point_mid, point_front);
    
    image1=image1+image2;
    resize(image1, image1, Size(), 0.5/resizeRatio, 0.5/resizeRatio, INTER_LINEAR);
    imshow("Test Window", image1);
    
    return image2;

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

void selectObject2(Mat& image)
{
    vector<vector<Point> > contours; // Vector for storing contour
    vector<Vec4i> hierarchy;

    largest_area = 0;
    int largest_contour_index = 0;
    findContours(image, contours, hierarchy, CV_RETR_CCOMP,
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
         image = Mat::zeros(image.size(), CV_8UC1);

        drawContours(
            image, contours, largest_contour_index, Scalar(255, 0, 0), CV_FILLED, 8,
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



int fillRatio()  //unused
{
    float circleEnvelopeArea = 3.1412 * circleRadius * circleRadius;
    float fillRatio = largest_area / (0.9 * circleEnvelopeArea);
    angle = acos(fillRatio) * 180.0 / 3.1412;
    if (fillRatio > 1.0)
        angle = 0;
    return fillRatio;
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
