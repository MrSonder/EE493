void templateMatching(Mat img);
void rectangleMask(Mat image, RotatedRect rectangle);
void templateExtract(Mat imageIn, int color);

void templateExtract(Mat imageIn, int color)
{
    Mat image1,image2;
    image1 = getObjectOfColor(imageIn, 'Y', 'R');
    imageIn.copyTo(image2, image1);
    dispImage(image2, "Mask",4);
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

    Mat templ = imread("rsz_blue_crop.png", CV_LOAD_IMAGE_COLOR);   // Read the file

  /// Create the result matrix
  int result_cols =  img.cols - templ.cols + 1;
  int result_rows = img.rows - templ.rows + 1;

  //result.create( result_rows, result_cols, CV_32FC1 );

  /// Do the Matching and Normalize
  matchTemplate( img, templ, result, CV_TM_CCORR_NORMED );
  normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() );
  threshold(result, result, 0.8, 1, THRESH_TOZERO);
  dispImage(result, "matching", 3);
  
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