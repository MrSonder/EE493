#include "inc/ee493_headers.cpp"
#include "inc/ee493_opencv_lowlevel.cpp"
#include "inc/ee493_opencv.cpp"
#include "inc/ee493_future.cpp"
#include "inc/ee493_arduino.cpp"
#include "inc/ee493_controller.cpp"
#include "inc/ee493_board.cpp"

void getBoardInfo();

int main(int argc, char *argv[])
{
    time(&start);
    cout << "\033[2J"
         << "\033[1;31mArduinoConnected: " << ArduinoConnected << endl;
    int colorFront = 'B';
    int colorFlag = 'P';

    //startCamera(0);
    //calibrateThreshold('P');

    //getFrameFromCamera(true, true);
    //calibrateDoubleCamera();
    //getPanaroma();

    getBoardInfo();

    //searchColorMethod(colorFront);
    //goTowardsObjectMethod(colorFront);

    //searchColorMethod(colorFlag);
    //goTowardsObjectMethod(colorFlag);
    return 0;
}