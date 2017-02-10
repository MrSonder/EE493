void templateMatching(Mat img);
void houghTransform(Mat src);


void templateMatching(Mat img) //not working
{
    Mat result;

    Mat templ = imread("blue.png", CV_LOAD_IMAGE_COLOR);   // Read the file



  /// Create the result matrix
  int result_cols =  img.cols - templ.cols + 1;
  int result_rows = img.rows - templ.rows + 1;

  //result.create( result_rows, result_cols, CV_32FC1 );

  /// Do the Matching and Normalize
  matchTemplate( img, templ, result, CV_TM_CCORR );

  normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() );
  //testWindow(result,"matching");
  /// Localizing the best match with minMaxLoc
  double minVal; double maxVal; Point minLoc; Point maxLoc;
  Point matchLoc;

  minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
  matchLoc = minLoc;
 
  /// Show me what you got
  rectangle( newFrame, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0 );
  rectangle( result, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0 );
}

void houghTransform(Mat src){
  vector<Vec3f> circles;
  Mat src_gray;
  cvtColor( src, src_gray, CV_BGR2GRAY );
  /// Apply the Hough Transform to find the circles
  HoughCircles( src_gray, circles, CV_HOUGH_GRADIENT, 1, src_gray.rows/8, 200, 100, 0, 0 );

  /// Draw the circles detected
  for( size_t i = 0; i < circles.size(); i++ )
  {
      Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
      int radius = cvRound(circles[i][2]);
      // circle center
      circle( src, center, 3, Scalar(0,255,0), -1, 8, 0 );
      // circle outline
      circle( src, center, radius, Scalar(0,0,255), 3, 8, 0 );
   }
   //testWindow(src, "hough");
   //testWindow(src_gray, "gray");

}

