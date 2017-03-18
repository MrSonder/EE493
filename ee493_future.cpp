void templateMatching(Mat img);
void rectangleMask(Mat image, RotatedRect rectangle);
void templateExtract(Mat imageIn, int color);


void templateExtract(Mat imageIn, int color)
{
    Mat image1,image2;
    image1 = getObjectOfColor(imageIn, color, 'R');
    imageIn.copyTo(image2, image1);
    dispImage(image2, "Mask", 4);
}

void rectangleMask(Mat image, RotatedRect rectangle)
{
    Mat imageRect = Mat::zeros( image.size(), CV_8UC1 );
    Point2f rect_points[4]; rectangle.points( rect_points );
    for( int j = 0; j < 4; j++ )
          line( imageRect, rect_points[j], rect_points[(j+1)%4], Scalar(255, 0, 0));

    Mat imageMask = getLargestArea(imageRect, 'N');

    Mat imageOut;

    newFrame.copyTo(imageOut, imageMask);
    //dispImage(imageOut, "masked" ,4);
}


void templateMatching(Mat img) //not working
{
    Mat result;

    Mat templ = imread("blue3.png", CV_LOAD_IMAGE_COLOR);   // Read the file

  /// Create the result matrix
  int result_cols =  img.cols - templ.cols + 1;
  int result_rows = img.rows - templ.rows + 1;

  //result.create( result_rows, result_cols, CV_32FC1 );

  /// Do the Matching and Normalize
  matchTemplate( img, templ, result, CV_TM_CCORR_NORMED );
  //normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() );
  threshold(result, result, 0.8, 1, THRESH_TOZERO);
  dispImage(result, "matching", 3);
  dispImage(img, "source", 0);
  //testWindow(result,"matching");
  /// Localizing the best match with minMaxLoc
  double minVal; double maxVal; Point minLoc; Point maxLoc;
  Point matchLoc;

  minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
  matchLoc = maxLoc;
 
  /// Show me what you got
  rectangle( newFrame, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0 );
  rectangle( result, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0 );

}



void calibrateThresholdBlackWhite(int color)
{
    setColor(color);
    char windowTitle[] = "Calibration";
    namedWindow(windowTitle, 1);
    moveWindow(windowTitle, 500, 500);
    cvCreateTrackbar("Threshold", windowTitle, &threshold_bw, 255);
    cvCreateTrackbar("Erode", windowTitle, &erode_val, 30);
    cvCreateTrackbar("Dilate", windowTitle, &dilate_val, 30);
    cvCreateTrackbar("Filter", windowTitle, &filter, 20);
    Mat image;
    while (true)
    {
        //camera >> newFrame; // get a new frame from camera
        newFrame = imread("board3.png");
        resize(newFrame, newFrame, Size(), 1, 1, INTER_LINEAR);
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


Mat thresholdImageBlackWhite(Mat image, int color, bool calibration)
{
    Mat imageOUT = image.clone();
    cvtColor(imageOUT, imageOUT, COLOR_BGR2GRAY);

    threshold(imageOUT, imageOUT, threshold_bw, 255, method);
    erode(imageOUT, imageOUT, cv::Mat(), cv::Point(-1, -1), erode_val);    
    medianBlur(imageOUT, imageOUT, 5);
    dilate(imageOUT, imageOUT, cv::Mat(), cv::Point(-1, -1), 3);
    return imageOUT ;
}




