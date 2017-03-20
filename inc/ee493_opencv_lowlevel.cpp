void switchToCamera(int index);
void startCamera(int index);
void dispImage(Mat image, String title, int loc);
void statusBar(Point2f center);
Point2f findCenter(Mat image);
void setColor(int colorFront);
void drawStraightLine(Mat *img, Point2f p1, Point2f p2);
Mat thresholdImageBlackWhite(Mat image, int color, bool calibration);
Mat thresholdImage(Mat image, int colorFront, bool calibration);

void calibrateThreshold(int color);
void calibrateThresholdBlackWhite(int color);

void getFrameFromCamera(double resize_ratio, bool disp, bool repeat);
void getFrameFromCamera();
void getFrameFromCamera(bool disp, bool repeat);


void getFrameFromCamera(){
    getFrameFromCamera(resizeRatio, false, false);
}


void getFrameFromCamera(bool disp, bool repeat){
    getFrameFromCamera(resizeRatio, disp, repeat);
}

void getFrameFromCamera(double resize_ratio, bool disp, bool repeat){
    
    do{
    camera >> newFrame;
    resize(newFrame, newFrame, Size(), resize_ratio, resize_ratio, INTER_LINEAR);
    if(disp) {
        dispImage(newFrame, "xx", 0);
    }
    }while(repeat);
}

void dispImage(Mat image, String title, int loc)
{
    // 0 2 4
    // 1 3 5
    waitKey(1);
    namedWindow(title, 1);
    resize(image, image, Size(), 0.55 / resizeRatio, 0.55 / resizeRatio, INTER_AREA);
    imshow(title, image);
    int x = 50 + (loc / 2) * 380;
    int y = 30 + (loc % 2) * 370;
    moveWindow(title, x, y);
}

void setColor(int colorFront)
{
    switch (colorFront)
    {
    case int('B'):
        iLowH = 50;
        iHighH = 105;
        iLowS = 25;
        iHighS = 255;
        iLowV = 25;
        iHighV = 255;
        break;

    case int('R'):
        iLowH = 155;
        iHighH = 179;
        iLowS = 55;
        iHighS = 255;
        iLowV = 25;
        iHighV = 255;
        break;

    case int('Y'):
        iLowH = 15;
        iHighH = 70;
        iLowS = 15;
        iHighS = 255;
        iLowV = 35;
        iHighV = 255;
        break;

    case int('G'):
        iLowH = 50;
        iHighH = 85;
        iLowS = 0;
        iHighS = 255;
        iLowV = 0;
        iHighV = 255;
        break;

    case int('P'):
        iLowH = 115;
        iHighH = 160;
        iLowS = 45;
        iHighS = 255;
        iLowV = 45;
        iHighV = 255;
        break;

    case int('w'):
        method = THRESH_BINARY;
        erode_val = 2;
        threshold_bw = 243;
        break;

    case int('b'):
        method = THRESH_BINARY_INV;
        erode_val = 7;
        threshold_bw = 72;
        break;
    }
}

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

    angle = atan((p1.y - p2.y) / (p1.x - p2.x)) * 180 / PI;
}

void statusBar(Point2f center)
{

    ostringstream statusBar;
    string statusText = "";
    statusText.assign("");
    statusText.assign("FPS :");
    //statusBar << int(getFPS());
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


Point2f findCenter(Mat image)
{
    //returns the center of a binary image
    Moments oMoments = moments(image);
    double dM01 = oMoments.m01;
    double dM10 = oMoments.m10;
    double dArea = oMoments.m00;

    int posX = dM10 / dArea;
    int posY = dM01 / dArea;

    return Point(posX, posY);
}

Mat templateExtract(Mat imageIn, int color)
{
    Mat image1, image2;
    image1 = thresholdImage(imageIn, color, false);

    imageIn.copyTo(image2, image1);
    
    dispImage(image2, "Mask", 4);
    return image2;
}


Mat thresholdImage(Mat image, int colorFront, bool calibration)
{
    if (!calibration)
        setColor(colorFront);

    Mat imageOUT = image.clone();

    if (colorFront == 'w' || colorFront == 'b')
    {
        imageOUT = thresholdImageBlackWhite(image, colorFront, calibration);
        return imageOUT;
    }

    cvtColor(imageOUT, imageOUT, COLOR_BGR2HSV);
    Mat temp = imageOUT.clone();

    inRange(imageOUT, Scalar(iLowH, iLowS, iLowV),
            Scalar(iHighH, iHighS, iHighV), imageOUT);
    if (colorFront == 82)
    {

        inRange(temp, Scalar(0, iLowS, iLowV),
                Scalar(28, iHighS, iHighV), temp);
        imageOUT = temp + imageOUT;
    }
    erode(imageOUT, imageOUT, cv::Mat(), cv::Point(-1, -1), 2);
    medianBlur(imageOUT, imageOUT, 5);
    //erode(imageOUT, imageOUT, cv::Mat(), cv::Point(-1, -1), 3);
    //dilate(imageOUT, imageOUT, cv::Mat(), cv::Point(-1, -1),1);
    return imageOUT;
}

void calibrateThreshold(int color)
{
    if (color == 'b' || color == 'w')
    {
        calibrateThresholdBlackWhite(color);
    }
    setColor(color);
    char windowTitle[] = "Calibration";
    namedWindow(windowTitle, 1);
    moveWindow(windowTitle, 500, 500);
    cvCreateTrackbar("dilate iteration", windowTitle, &filterRatio, 10);
    cvCreateTrackbar("LowHue", windowTitle, &iLowH, 179);
    cvCreateTrackbar("HighHue", windowTitle, &iHighH, 179);
    cvCreateTrackbar("LowSat", windowTitle, &iLowS, 255);
    cvCreateTrackbar("HighSat", windowTitle, &iHighS, 255);
    cvCreateTrackbar("LowVal", windowTitle, &iLowV, 255);
    cvCreateTrackbar("HighVal", windowTitle, &iHighV, 255);
    Mat image;
    while (true)
    {
        
        //newFrame = imread("board3.png");

        getFrameFromCamera();
        image = thresholdImage(newFrame, color, true);
        dispImage(image, "Calibration", 2);

        int c = waitKey(10);
        if ((char)c == 27)
        {
            cvDestroyWindow(windowTitle);
            exit(0);
        }
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