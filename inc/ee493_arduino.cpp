String driveMotor(int leftMotor_int, int RightMotor_int);
String int_to_XXX(int speedInt);
void turnRobot(int turn_angle);
void driveMotorForSeconds(int seconds, int L, int R);
void allignRobotAxial(int angle);
double calibrateTime(int L, int R);
double calibrateTurnTime(int L, int R, int number_of_turns);
void tx2Arduino(string data);
void txTerminal(string data);

int turn_PWM = 100;
double second_per_turn = 4.4;
double camera_center_to_object_duration = 5.5;

/*
local GRAY="\[\033[1;30m\]"
local LIGHT_GRAY="\[\033[0;37m\]"
local CYAN="\[\033[0;36m\]"
local LIGHT_CYAN="\[\033[1;36m\]"
*/

void txArduino(string data)
{
    if (ArduinoConnected){
    FILE* file;
    file = fopen("/dev/ttyUSB0", "w");
    fprintf(file, "%s\r", data.c_str()); // Writing to the file
    fclose(file);
    }
    
    txTerminal(data);
}

void txTerminal(string data)
{
    printf("\033[1;36m%s\r", data.c_str());
    fflush(stdout);
}

void drive_motor_terminal(){
    int L;
    int R;
    cin >> L;
    cin >> R;
    txArduino(driveMotor(L,R));
}

void allignRobotAxial(int angle){
    //assuming angle=90 || -90 when alligned
    //turn (90-angle degrees)

    if (angle>0){
        turnRobot(90-angle);
        }
    else if (angle<0){
        turnRobot(-90-angle);
        }

    driveMotorForSeconds(camera_center_to_object_duration, 250, 250);

}

double calibrateTurnTime(int L, int R, int number_of_turns){
    double second_per_turn = calibrateTime(L, R) /((double) number_of_turns);
    cout << "second_per_turn: " << second_per_turn;
    return second_per_turn;
}
void driveMotorForSeconds(int seconds, int L, int R){
    txArduino(driveMotor(L, R));
    usleep(seconds*1e6);
    txArduino(driveMotor(0, 0));
}

void turnRobot(int turn_angle){

    double second_per_degree =  second_per_turn / 360.0 ;
    double turn_time = second_per_degree * turn_angle;
    cout<<"\nTurning " << turn_angle << " degrees in " << turn_time << "seconds" <<endl;
    
    turn_time = turn_time + 0.5;
    if (turn_angle>0){
        driveMotorForSeconds(turn_time, -1*turn_PWM, turn_PWM );
    }
    else if (turn_angle<0){
        driveMotorForSeconds(turn_time, turn_PWM, -1*turn_PWM );
    }

    cout<<"Done!" <<endl;

}

double calibrateTime(int L, int R){
    //sets second_per_turn

    cout<<"Press Enter to start!\n";
    getchar();
    txArduino( driveMotor(L, R) );
    time_t start, end;
    time(&start);

    cout<<"Press Enter to stop!\n";
    getchar();

    time(&end);
    txArduino( driveMotor(0, 0) ); 
    int total_time = (end - start);
    cout<<"Total Time: "<< total_time<<" Seconds"<<endl;
    return total_time;
}


String driveMotor(int leftMotor_int, int RightMotor_int){
    string data = "X" + int_to_XXX( leftMotor_int ) + int_to_XXX( RightMotor_int );
    return data;
}

String int_to_XXX(int speedInt){
    ostringstream stringStream;
    stringStream<< abs(speedInt);
    string speedString = stringStream.str();
    string speedStringFormatted = "";

    while(speedString.length() != 3){
        speedString = speedString.append("0");
        speedStringFormatted = speedStringFormatted.append("0");
    }
    speedStringFormatted.append(stringStream.str());

    if(speedInt > 0){
        speedStringFormatted = speedStringFormatted.append("F");
    }
    else{
        speedStringFormatted = speedStringFormatted.append("R");
    }

    return speedStringFormatted;
}


