void getPanaroma();
void getTriangleArray(Mat image);
void calibrateDoubleCamera();
Mat getTriangleContours(Mat image, int trig_index_1, int trig_index_2);
Point2f getBoardSlot(Mat img, int trig_index_1, int trig_intex_2);
int getFPS();

Point2f drawCenterLine(Mat imageIn, int colorFront);
Mat getObjectOfColor(Mat image, int colorFront);
Mat getLargestArea(Mat image, int object);
double fillRatio(vector<Point> contour, int object);


int getFPS()
{
    time(&end);
    double fps = FPSCounter / (end - start);
    FPSCounter++;
    return fps;
}

double fillRatio(vector<Point> contour, int object)
{
    Point_<float> circleCenter;
    RotatedRect boundRect;
    double areaCont = contourArea(contour, false);
    double fillRatio = 0;
    //area of rectange : r.width * r.height

    if (object == 'R')
    {
        boundRect = minAreaRect(contour);
        double rectEnvelopeArea = boundRect.size.width * boundRect.size.height;
        fillRatio = abs(boundRect.angle);
    }
    else if (object == 'C')
    {
        minEnclosingCircle(contour, circleCenter, circleRadius);
        double circleEnvelopeArea = PI * circleRadius * circleRadius;
        fillRatio = areaCont / (0.9 * circleEnvelopeArea);
        //angle = acos(fillRatio) * 180.0 / 3.1412;
    }

    return fillRatio;
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

    return point_front;
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
        
        area_threshold = 0;
    }
    if(colorFront == 'P')
    {
        
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


    drawContours(imageSelected, contours, largest_contour_index, Scalar(255, 0, 0), CV_FILLED, 8, hierarchy);

    return imageSelected;
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
