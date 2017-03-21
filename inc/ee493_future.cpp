
void rectangleMask(Mat image, RotatedRect rectangle);


void rectangleMask(Mat image, RotatedRect rectangle)
{
    Mat imageRect = Mat::zeros(image.size(), CV_8UC1);
    Point2f rect_points[4];
    rectangle.points(rect_points);
    for (int j = 0; j < 4; j++)
        line(imageRect, rect_points[j], rect_points[(j + 1) % 4], Scalar(255, 0, 0));

    Mat imageMask = getLargestArea(imageRect, 'N');

    Mat imageOut;

    newFrame.copyTo(imageOut, imageMask);
    //dispImage(imageOut, "masked" ,4);
}


