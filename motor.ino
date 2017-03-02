String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
int direction=00;
int counter=-1;
String left_speed ;
String left_direction ;
String right_speed ;
String right_direction ;
int left_1=5;
int left_2=6;

int right_1=9;
int right_2=10;

void setup() {
  // initialize serial:
  Serial.begin(9600);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
  pinMode(left_1, OUTPUT);  //left1;6
  pinMode(left_2, OUTPUT);  //left2;7
  pinMode(right_1, OUTPUT);  //right1; 9
  pinMode(right_2, OUTPUT);  //right2; 10
  analogWrite(right_2, 0);
  analogWrite(left_2, 0);
  analogWrite(right_1, 0);
  analogWrite(left_1, 0);
  stringComplete = false;
  //inputString = "102F102F";
}

void loop() {
  /*working variables*/
  if (stringComplete == true) {
     left_speed = inputString.substring(0,3) ;
     left_direction = inputString.substring(3,4) ;
     right_speed = inputString.substring(4,7) ;
     right_direction = inputString.substring(7,8) ;
     Serial.print("\nLeft wheel turning ");
     if (left_direction == "F") {analogWrite(left_1, left_speed.toInt());analogWrite(left_2, 0); 
                                  Serial.print("forward with "); Serial.print(left_speed); }
     else if (left_direction == "R"){analogWrite(left_2, left_speed.toInt());analogWrite(left_1, 0);  
                                  Serial.print("backward with "); Serial.print(left_speed); }
     Serial.print(".\nRight wheel turning ");
     if (right_direction == "F") {analogWrite(right_1, right_speed.toInt());analogWrite(right_2, 0); 
                                  Serial.print("forward with "); Serial.print(right_speed);Serial.print(".\n"); }
     else if (right_direction == "R"){analogWrite(right_2, right_speed.toInt());analogWrite(right_1, 0);  
                                  Serial.print("backward with "); Serial.print(right_speed);Serial.print(".\n"); }
	stringComplete = false;
       inputString="" ;

    }

  
}



/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // Expected format is X100F100R
    // add it to the inputString:
    inputString += inChar;
    if (inChar == 'X') {counter = -1; inputString="" ;}
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    counter++;
    if (counter == 9) {
     stringComplete = true;
     counter = -1;
     Serial.print(inputString);}
  }
}
