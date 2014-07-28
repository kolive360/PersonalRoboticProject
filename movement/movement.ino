/*
* Autonomous Obstacle Avoidance Vehicle
*
* Description: Vehicle that avoids obstacles and can navigate across a flat surface
*
* Author: Amar Bhatt
*/


// Define Motor Pins
const int motorR1A = 22;
const int motorR1B = 23;
const int motorF1A = 24;
const int motorF1B = 25;

const int motorR2A = 26;
const int motorR2B = 27;
const int motorF2A = 28;
const int motorF2B = 29;

const int motorR1E = 2;
const int motorF1E = 3;
const int motorR2E = 4;
const int motorF2E = 5;

// Define Sensor Pins
const int trigPin = 7;
const int echoPin = 8;

// Define distance constants
const int TooClose = 2;
const int OK = 10;
const int Good = 15;

// Define speed constants
const int halfSpeed = 128;
const int threeQuartersSpeed = 188;
const int fullSpeed = 250;

// Direction variables
int curDir = -1;
int prevDir = -1;

// Direction of motion enumeration
enum motorDirection {
  FORWARD,
  REVERSE,
  RIGHT,
  LEFT
};

// Initialize pins  
void setup(){
  Serial.begin(9600);
  
  pinMode(motorR1A, OUTPUT);
  pinMode(motorR1B, OUTPUT);
  pinMode(motorF1A, OUTPUT);
  pinMode(motorF1B, OUTPUT);
  
  pinMode(motorR2A, OUTPUT);
  pinMode(motorR2B, OUTPUT);
  pinMode(motorF2A, OUTPUT);
  pinMode(motorF2B, OUTPUT);
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  Serial.println("Start Motors");
}

void loop(){
//  //Serial.println("Reset Motors");
//  brake();
//  delay(2000);
//  //Serial.println("Drive Forward");
//  drive(255, FORWARD);
//  delay(2000);
//  //Serial.println("Pivot Left");
//  pivot(255, LEFT);
//  delay(4000);
//  //Serial.println("Drive Reverse");
//  drive(255, REVERSE);
//  delay(2000);
//  //Serial.println("Pivot RIGHT");
//  pivot(255, RIGHT);
//  delay(4000);  

  // Poll Ultrasonic distance sensor
  long inches = getDistanceFromObstacle();

  // If TooClose reverse and turn
  if (inches <= TooClose){
    brake(); // Stop
    drive(halfSpeed, REVERSE); // Reverse
    delay(1000);
    // If the vehicle was in midst of turning
    if(prevDir != -1){
      // If the vehicle was not in the process of correcting a previous turn mistake
      if (curDir == -1){
        curDir = (prevDir == RIGHT) ? LEFT : RIGHT; // Turn in the opposite direction
        pivot(fullSpeed, curDir);
       delay(2000); 
      }else{
        pivot(fullSpeed, curDir); // Turn in the current direction
        delay(2000);
      }     
    }
  }
  // If OK start turning slightly away from obstacle
  else if (inches <= OK){
     // If the vehicle was not already turning
     if(prevDir == -1){
       pivot(fullSpeed, RIGHT); // Turn right by default
       prevDir = RIGHT;
       delay(2000);
     }else { // else turn in the current direction
       pivot(fullSpeed, curDir);
       prevDir = curDir; // set the previous direction equal to the current direction
       delay(2000);
     } 
  }
  // Keep driving forward!
  else {
    drive(fullSpeed, FORWARD);
    // Reset direction variables
    curDir = -1;
    prevDir = -1;
    delay(500);
  }
}

/*
* go(int, int, int, int, int)
*
* Description: Sets the passed in motor with passed in direction and speed 
*
* Parameters: motorPinA/motorPinB = the 2 pins the wheel motor is attached to
*             motorEnable = the enable pin for the wheel motor
*             motorSpeed = the speed the wheel motor needs to be set to
*             motorDirection = direction wheel motor needs to go in
*/
void go(int motorPinA, int motorPinB, int motorEnable, int motorSpeed, int motorDirection){
  analogWrite(motorEnable, motorSpeed);
  //Forward
  if (motorDirection == FORWARD){
    digitalWrite(motorPinA, HIGH);
    digitalWrite(motorPinB, LOW);
  }//Reverse
  else{
    digitalWrite(motorPinA, LOW);
    digitalWrite(motorPinB, HIGH);
  }
}

/*
* drive(int, int)
*
* Description: Sets up vehicle to travel either Forward or Reverse
*
* Parameters: driveSpeed = the speed the vehicle needs to go
*             driveDirection = direction (FORWARD or REVERSE)
*/
void drive(int driveSpeed, int driveDirection) {
  go(motorR1A, motorR1B, motorR1E, driveSpeed, driveDirection);
  go(motorF1A, motorF1B, motorF1E, driveSpeed, driveDirection);
  go(motorR2A, motorR2B, motorR2E, driveSpeed, driveDirection);
  go(motorF2A, motorF2B, motorF2E, driveSpeed, driveDirection);  
}

/*
* pivot(int, int)
*
* Description: Sets up vehicle to turn either LEFT or RIGHT
*
* Parameters: pivotSpeed = the speed the vehicle needs to turn at
*             pivotDirection = direction of turning (LEFT or RIGHT)
*/
void pivot (int pivotSpeed, int pivotDirection){
  //Pivot Left
  if (pivotDirection == RIGHT) {
    go(motorR1A, motorR1B, motorR1E, pivotSpeed, FORWARD);
    go(motorF1A, motorF1B, motorF1E, pivotSpeed, FORWARD);
    go(motorR2A, motorR2B, motorR2E, pivotSpeed, REVERSE);
    go(motorF2A, motorF2B, motorF2E, pivotSpeed, REVERSE);
  }
  //Pivot Right
  else{
    go(motorR1A, motorR1B, motorR1E, pivotSpeed, REVERSE);
    go(motorF1A, motorF1B, motorF1E, pivotSpeed, REVERSE);
    go(motorR2A, motorR2B, motorR2E, pivotSpeed, FORWARD);
    go(motorF2A, motorF2B, motorF2E, pivotSpeed, FORWARD);
  }
}

/*
* brake()
*
* Description: Stops vehicle and turns off motors
*
*/
void brake() {
  analogWrite(motorR1E, 0);
  analogWrite(motorF1E, 0);
  analogWrite(motorR2E, 0);
  analogWrite(motorF2E, 0);
  digitalWrite(motorR1A, LOW);
  digitalWrite(motorR1B, LOW);
  digitalWrite(motorF1A, LOW);
  digitalWrite(motorF1B, LOW);
  digitalWrite(motorR2A, LOW);
  digitalWrite(motorR2B, LOW);
  digitalWrite(motorF2A, LOW);
  digitalWrite(motorF2B, LOW);
}

/*
* getDistanceFromObstacle()
*
* Description: Uses distance sensor to get distance from nearest object
*
* Return: long = inches from object
*/
long getDistanceFromObstacle() {
  //Variables for inches and centimeter conversion
  long duration;
  //Ensure clean pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(trigPin, LOW);
  
  //Read Pulse Back
  duration = pulseIn(echoPin, HIGH);
  
  //convert to inches
  return microsecondsToInches(duration);
}

/*
* microsecondsToInches(long)
*
* Description: Converts time into inches
*
* Return: long = inches from object
*/
long microsecondsToInches(long microseconds){
  // According to Parallax's datasheet for the PING))), there are
  // 73.746 microseconds per inch (i.e. sound travels at 1130 feet per
  // second).  This gives the distance travelled by the ping, outbound
  // and return, so we divide by 2 to get the distance of the obstacle.
  return microseconds / 73.746 / 2;
}//end microsecondsToInches
  
