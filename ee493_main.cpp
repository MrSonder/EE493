#include "inc/ee493_headers.cpp"
#include "inc/ee493_opencv_lowlevel.cpp"
#include "inc/ee493_opencv.cpp"
#include "inc/ee493_future.cpp"
#include "inc/ee493_arduino.cpp"
#include "inc/ee493_controller.cpp"


void getBoardInfo();

int main(int argc, char *argv[])
{
    time(&start);
    cout << "\033[2J" << "\033[1;31mArduinoConnected: " << ArduinoConnected << endl;
    int colorFront = 'B';
    int colorFlag = 'P';
    
    //startCamera(0);
    //calibrateThreshold('P');
    
    //getFrameFromCamera(true, true);
    //calibrateDoubleCamera();
    //getPanaroma();

    searchColorMethod(colorFront);
    goTowardsObjectMethod(colorFront);

    //searchColorMethod(colorFlag);
    //goTowardsObjectMethod(colorFlag);
    return 0;
}



void getBoardInfo(){
    Mat image_temp, image;
    Mat image_temp2;
    Mat image_thresh;
    Mat temp2;
    Point2f point_cyc;
    int game[7][7];
    newFrame = imread("board3.png");
    
    Point2f point;
    int trig_x[7];
    int trig_avg=0;
    cout<<"triangles"<<endl;
    for(int i=0; i<=6; i++){
        point = getBoardSlot(newFrame, i, i);
        trig_x[i] = point.x;
        cout<<point.y<<endl;
        trig_avg = trig_avg + point.y/7;
    }
    cout<< trig_avg <<endl;

cout<<"cylnders"<<endl;

  
    image = templateMatching(templateExtract(newFrame, 'B'));
    Mat imageSelected = Mat::zeros(image.size(), CV_8UC1);
    vector<vector<Point> > contours; // Vector for storing contour
    vector<Vec4i> hierarchy;
    image.convertTo(image, CV_8UC1);
    cout<<image.type()<<endl;
    findContours(image, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    for (int i = 0; i < contours.size(); i++) // iterate through each contour.
    {
    imageSelected = Mat::zeros(image.size(), CV_8UC1);
    drawContours(imageSelected, contours, i, Scalar(255, 0, 0), CV_FILLED, 8, hierarchy);
    Point2f point_cyc = findCenter(imageSelected);
    cout<<point_cyc.y<<endl;
    }
    dispImage(newFrame, "matching", 3);

        //getBoardSlot(newFrame, 0);
        //
}