bool searchColor(Mat img, int color);
void goTowardsObjectMethod(int color);
void searchColorMethod(int color);
void goTowardsSlot(int base_speed, Mat img, int trig_index, bool ArduinoConnected, int y_threshold, int turn_rate_divider);
bool goTowardsObject(int base_speed, Mat img, int colorFront, bool ArduinoConnected, int y_threshold, int turn_rate_divider);

void searchColorMethod(int color)
{
    startCamera(cam_index_2);
    object_exist = searchColor(newFrame, color);

    while (!object_exist)
    {
        camera >> newFrame;
        resize(newFrame, newFrame, Size(), resizeRatio, resizeRatio, INTER_LINEAR);
        txArduino(driveMotor(-70, 70));
        object_exist = searchColor(newFrame, color);
        //dispImage(newFrame, "search", 0);
    }
    driveMotorForSeconds(1, 50, -50);
    txArduino(driveMotor(0, 0));
}

void goTowardsObjectMethod(int color)
{
    /*
    startCamera(cam_index_1);
    object_exist = true;
    while (object_exist)
    {
        getFrameFromCamera();
        object_exist = goTowardsObject(120, newFrame, color, ArduinoConnected, 105, 4);
    }
    

    camera.release();
*/
    startCamera(cam_index_2);
    object_exist = true;
    while (object_exist)
    {
        camera >> newFrame;
        resize(newFrame, newFrame, Size(), resizeRatio, resizeRatio, INTER_LINEAR);
       
        object_exist = goTowardsObject(90, newFrame, color, ArduinoConnected, 95, 4);
    }
}


/*
void goTowardsSlot(int base_speed, Mat img, int trig_index, bool ArduinoConnected, int y_threshold, int turn_rate_divider)
{
    Point2f center = getBoardSlot(img, trig_index);

    int mid_y = img.rows / 2;
    int mid_x = img.cols / 2;
    Point2f test(mid_x, mid_y);
    center = center - test;
    string txString;
    int speed = (center.x) / turn_rate_divider;
    if (abs(center.x) < 550)
    {
        txArduino(driveMotor(base_speed + speed, base_speed - speed));
    }
    else
    {
        txArduino(driveMotor(0, 0));
    }

    if (center.y > y_threshold)
    {
        //driveMotorForSeconds(1.5, 100, 100);
        txArduino(driveMotor(0, 0));
    }
}
*/

bool searchColor(Mat img, int color)
{
    //tracks object with an image taken from camera
    //uses global angle variable

    Point2f center = drawCenterLine(img, color);
    bool object_exist = false;
    int mid_y = img.rows / 2;
    int mid_x = img.cols / 2;
    Point2f test(mid_x, mid_y);
    center = center - test;

    if ((center.x > -100 and center.x < 100) and (center.y < 80 and center.y > -20))
    {
        object_exist = true;
    }
    return object_exist;
}

bool goTowardsObject(int base_speed, Mat img, int colorFront, bool ArduinoConnected, int y_threshold, int turn_rate_divider)
{
    //tracks object with an image taken from camera
    //uses global angle variable

    Point2f center = drawCenterLine(img, colorFront);
    bool object_exist = true;
    int mid_y = img.rows / 2;
    int mid_x = img.cols / 2;
    Point2f test(mid_x, mid_y);
    center = center - test;
    string txString;
    int speed = (center.x) / turn_rate_divider;
    if (abs(center.x) < 550)
    {
        txArduino(driveMotor(base_speed + speed, base_speed - speed));
    }
    else
    {
        txArduino(driveMotor(0, 0));
    }

    if (center.y > y_threshold)
    {
        //driveMotorForSeconds(1.5, 100, 100);
        txArduino(driveMotor(0, 0));
        object_exist = false;
    }
    return object_exist;
}