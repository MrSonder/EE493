String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
int direction=00;
int counter=-1;
String left_speed ;
String left_direction ;
String right_speed ;
String right_direction ;
String step_or_dc ;
int left_1=5;
int left_2=6;

int right_1=9;
int right_2=10;
// Stepper
int pole_1 = 2;
int pole_2 = 3;
int pole_3 = 4;
int pole_4 = 7;

int step_interval = 5;
String step_level;
String up_or_down;


void setup() {
  // initialize serial:
  Serial.begin(9600);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
  pinMode(left_1, OUTPUT);  //left1;5
  pinMode(left_2, OUTPUT);  //left2;6
  pinMode(right_1, OUTPUT);  //right1; 9
  pinMode(right_2, OUTPUT);  //right2; 10
  pinMode(pole_1, OUTPUT);
  pinMode(pole_2, OUTPUT);
  pinMode(pole_3, OUTPUT);
  pinMode(pole_4, OUTPUT);
  digitalWrite(pole_1, HIGH);
  digitalWrite(pole_2, HIGH); 
  digitalWrite(pole_3, LOW);
  digitalWrite(pole_4, LOW);
  
  
  analogWrite(right_2, 0);
  analogWrite(left_2, 0);
  analogWrite(right_1, 0);
  analogWrite(left_1, 0);
  
  stringComplete = false;
  //inputString = "X102F102F";
}

void loop() {
  /*working variables*/
  if (stringComplete == true) {
     step_or_dc = inputString.substring(7,8) ;
     Serial.print("\n");Serial.print(step_or_dc);
     if (step_or_dc == "F" or step_or_dc == "R" ){
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
     else if (step_or_dc == "0") {
       step_level = inputString.substring(0,3) ;
       up_or_down = inputString.substring(3,4) ;
       Serial.print("\n");Serial.print(step_level);
       Serial.print("\n");Serial.print(up_or_down);
       if (up_or_down == "U") {step_up(step_level.toInt());}
       else if (up_or_down == "D") {step_down(step_level.toInt());}
       stringComplete = false;
       inputString="" ;       
       
     }
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
    if (counter == 8) {
     stringComplete = true;
     counter = -1;
     Serial.print(inputString);}
  }
}

void step_up(int level){
 int counter1=0;
 while (counter1<level) {
  digitalWrite(pole_1, HIGH);
  digitalWrite(pole_2, HIGH); 
  digitalWrite(pole_3, LOW);
  digitalWrite(pole_4, LOW);
  delay(step_interval);
  digitalWrite(pole_1, LOW);
  digitalWrite(pole_2, HIGH); 
  digitalWrite(pole_3, HIGH);
  digitalWrite(pole_4, LOW);
  delay(step_interval);
  digitalWrite(pole_1, LOW);
  digitalWrite(pole_2, LOW); 
  digitalWrite(pole_3, HIGH);
  digitalWrite(pole_4, HIGH);
  delay(step_interval);
  digitalWrite(pole_1, HIGH);
  digitalWrite(pole_2, LOW); 
  digitalWrite(pole_3, LOW);
  digitalWrite(pole_4, HIGH);
  delay(step_interval);
  digitalWrite(pole_1, HIGH);
  digitalWrite(pole_2, HIGH); 
  digitalWrite(pole_3, LOW);
  digitalWrite(pole_4, LOW);
  delay(step_interval);
  counter1=counter1+1;
  } 
}


void step_down(int level){
 int counter2=0;
 while (counter2<level) {
  digitalWrite(pole_1, HIGH);
  digitalWrite(pole_2, HIGH); 
  digitalWrite(pole_3, LOW);
  digitalWrite(pole_4, LOW);
  delay(step_interval);
  digitalWrite(pole_1, HIGH);
  digitalWrite(pole_2, LOW); 
  digitalWrite(pole_3, LOW);
  digitalWrite(pole_4, HIGH);
  delay(step_interval);
  digitalWrite(pole_1, LOW);
  digitalWrite(pole_2, LOW); 
  digitalWrite(pole_3, HIGH);
  digitalWrite(pole_4, HIGH);
  delay(step_interval);
  digitalWrite(pole_1, LOW);
  digitalWrite(pole_2, HIGH); 
  digitalWrite(pole_3, HIGH);
  digitalWrite(pole_4, LOW);
  delay(step_interval);
  digitalWrite(pole_1, HIGH);
  digitalWrite(pole_2, HIGH); 
  digitalWrite(pole_3, LOW);
  digitalWrite(pole_4, LOW);
  delay(step_interval);
  counter2=counter2+1;
  } 
}
