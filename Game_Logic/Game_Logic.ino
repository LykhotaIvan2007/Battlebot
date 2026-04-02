#include <Adafruit_NeoPixel.h>

// SPEED SETTINGS 
const int SPEED = 160;   // Base speed of motors
const int SPEED_DIFFERENCE = 20; // Compensation for motor imbalance
const int SPEED_INCREMENTATION_MOVE_AND_CORRECT = 20; // Speed increase when correcting
const int SPEED_DECREMENTATION_MOVE_AND_CORRECT = 10; // Speed decrease when correcting
const int WALL_ALIGNING_DISTANCE = 9; // Distance to maintain from wall

const int TURN_90_TIME = 600; 
const int TURN_START_TIME = 750;

const int EMPTY_WAY_DISTANCE = 30; // Distance considered as open path
const int STOP_DISTANCE = 15; // Distance to stop before obstacle

const int TIME_WHEN_WAY_DETECTED = 650; // Time delay when new path detected
const int TIME_FOR_NEW_PATH = 1000; // Time to move into a new path

// MOTOR PINS 
const int LEFTMOTORBACK = 6;
const int LEFTMOTORSTRAIGHT = 9;
const int RIGHTMOTORBACK = 10;
const int RIGHTMOTORSTRAIGHT = 11;

// ULTRASONIC SENSOR PINS 
const int ECHOPIN_RIGHT = 2;
const int TRIGPIN_RIGHT = 3;
const int ECHOPIN = 4;
const int TRIGPIN = 5;
const int ECHOPIN_LEFT = 7;
const int TRIGPIN_LEFT = 8;

// GRIPPER
const int SERVO = 12;
const int GRIPPER_OPEN = 1600;  // Pulse for open position
const int GRIPPER_CLOSE = 990;  // Pulse for closed position

// LINE SENSOR PINS 
const int LINE_SENSOR1 = A0;
const int LINE_SENSOR2 = A1;
const int LINE_SENSOR3 = A2;
const int LINE_SENSOR4 = A3;
const int LINE_SENSOR5 = A4;
const int LINE_SENSOR6 = A5;
const int LINE_SENSOR7 = A6;
const int LINE_SENSOR8 = A7;

// Array of line sensor pins
const int sensorPins[8] = {
  LINE_SENSOR1, LINE_SENSOR2, LINE_SENSOR3, LINE_SENSOR4,
  LINE_SENSOR5, LINE_SENSOR6, LINE_SENSOR7, LINE_SENSOR8
};

int sensorValues[8]; // Stores values from sensors

const int NUM_SENSORS = 8; // Total number of line sensors

int blackDetection = 900; // Threshold value to detect black line

// DISTANCE VARIABLES 
long distanceFront, distanceRight, distanceLeft;

// LED bulbs
Adafruit_NeoPixel pixels(4, 13, NEO_GRB + NEO_KHZ800);

void setup(){
  // Set motor pins as output
  Serial.begin(9600);
  pinMode(LEFTMOTORBACK, OUTPUT);
  pinMode(LEFTMOTORSTRAIGHT, OUTPUT);
  pinMode(RIGHTMOTORBACK, OUTPUT);
  pinMode(RIGHTMOTORSTRAIGHT, OUTPUT);

  // Set ultrasonic sensor pins
  pinMode(TRIGPIN_RIGHT, OUTPUT);
  pinMode(ECHOPIN_RIGHT, INPUT);
  pinMode(TRIGPIN, OUTPUT);
  pinMode(ECHOPIN, INPUT);
  pinMode(TRIGPIN_LEFT, OUTPUT);
  pinMode(ECHOPIN_LEFT, INPUT);

  // Gripper setup
  pinMode(SERVO, OUTPUT);
  holdGripper(GRIPPER_OPEN, 1000);
  
  // Line sensor pins setup
  for (int i = 14; i < 22; i++){
    pinMode(i, INPUT);
  }

  // Initialize LED bulbs
  pixels.begin();      
  pixels.clear();      
  pixels.show();       
}

// MODES 
boolean parkingMode = true; // Initial parking routine active
boolean mode = true; // true = maze mode, false = line following mode

void loop(){
  gripper(0); // Maintain gripper signal

  // Read distances from sensors
  distanceLeft = readDistance(TRIGPIN_LEFT, ECHOPIN_LEFT);
  distanceRight = readDistance(TRIGPIN_RIGHT, ECHOPIN_RIGHT);
  distanceFront = readDistance(TRIGPIN, ECHOPIN);
  Serial.println(distanceFront);

  start(); // Execute starting sequence

  if (!parkingMode){
    if (checkLine()){
      lineFollowing(); // Follow line if detected
    }else{
      maze(); // Otherwise use maze navigation
    }
  }
  
}

// START LOGIC 
void start(){
  // If cone detected in front during parking mode
  if(distanceFront < 30 && parkingMode == true){
    delay(3000);

    // Move forward until left side is clear
    while (distanceLeft < 25){
      moveForward(SPEED, SPEED);
      distanceLeft = readDistance(TRIGPIN_LEFT, ECHOPIN_LEFT);
    }

    delay (950);
    stopMoving();

    holdGripper(GRIPPER_CLOSE, 1000); // Close gripper (grab object)

    //turnLeft();
    turnLeft(TURN_START_TIME);
    
    moveForward(SPEED, SPEED);
    delay(2000);

    moveAndCorrect(3000); // Move forward with wall correction

    parkingMode = false; // Exit parking mode
  }
}

// GRIPPER CONTROL 
void holdGripper(int pulse,int timeMs){
  unsigned long start = millis();
  while(millis() - start < timeMs){
    gripper(pulse);
    delay(1);
  }
}

void gripper(int newPulse){
  static unsigned long timer;
  static int pulse;

  if(millis() > timer){
    if(newPulse > 0){  
      pulse = newPulse;  
    }  

    digitalWrite(SERVO, HIGH);  
    delayMicroseconds(pulse);  
    digitalWrite(SERVO, LOW);  

    timer = millis() + 20; // Repeat every 20ms
  }
}

// MOVE WITH WALL CORRECTION
void moveAndCorrect(int duration){
  unsigned long startTime = millis();

  while (millis() - startTime < duration){
    long currentLeft = readDistance(TRIGPIN_LEFT, ECHOPIN_LEFT);
    long currentRight = readDistance(TRIGPIN_RIGHT, ECHOPIN_RIGHT);

    // Stop if obstacle detected ahead
    if (readDistance(TRIGPIN, ECHOPIN) < STOP_DISTANCE){
      stopMoving();
      break;
    }

    // Adjust direction based on wall closeness
    if (currentLeft < WALL_ALIGNING_DISTANCE){
      moveForward(SPEED + SPEED_INCREMENTATION_MOVE_AND_CORRECT, SPEED - SPEED_DECREMENTATION_MOVE_AND_CORRECT);
    }
    else if (currentRight < WALL_ALIGNING_DISTANCE){
      moveForward(SPEED - SPEED_DECREMENTATION_MOVE_AND_CORRECT, SPEED + SPEED_INCREMENTATION_MOVE_AND_CORRECT);
    }
    else{
      moveForward(SPEED, SPEED);
    }

    delay(20);
  }

  stopMoving();
}

// BASIC MOVEMENT 
void moveForward(int leftSpeed, int rightSpeed){
  pixels.setPixelColor(0, pixels.Color(0, 0, 0));   
  pixels.setPixelColor(1, pixels.Color(0, 0, 0)); 
  pixels.setPixelColor(2, pixels.Color(0, 0, 255));   
  pixels.setPixelColor(3, pixels.Color(0, 0, 255));
  pixels.show();

  leftSpeed = constrain(leftSpeed, 0, 255);
  rightSpeed = constrain(rightSpeed, 0, 255);

  analogWrite(LEFTMOTORSTRAIGHT, leftSpeed);
  analogWrite(LEFTMOTORBACK, 0);
  analogWrite(RIGHTMOTORSTRAIGHT, rightSpeed);
  analogWrite(RIGHTMOTORBACK, 0);
}

// Stop all motors
void stopMoving(){
  analogWrite(RIGHTMOTORSTRAIGHT, 0);
  analogWrite(LEFTMOTORSTRAIGHT, 0);
  analogWrite(RIGHTMOTORBACK, 0);
  analogWrite(LEFTMOTORBACK, 0);
}

// TURNING 
void turnLeft(int timeForTurn){
  pixels.setPixelColor(0, pixels.Color(255, 255, 0));
  pixels.setPixelColor(1, pixels.Color(0, 0, 0));
  pixels.setPixelColor(2, pixels.Color(0, 0, 0));
  pixels.setPixelColor(3, pixels.Color(255, 255, 0)); 
  pixels.show();

  analogWrite(LEFTMOTORSTRAIGHT, 0);
  analogWrite(LEFTMOTORBACK, SPEED + SPEED_DIFFERENCE);
  analogWrite(RIGHTMOTORSTRAIGHT, SPEED);
  analogWrite(RIGHTMOTORBACK, 0);

  delay(timeForTurn);
  stopMoving();
}

void turnRight(){
  pixels.setPixelColor(0, pixels.Color(0, 0, 0));   
  pixels.setPixelColor(1, pixels.Color(255, 255, 0)); 
  pixels.setPixelColor(2, pixels.Color(255, 255, 0));   
  pixels.setPixelColor(3, pixels.Color(0, 0, 0));  
  pixels.show();

  analogWrite(LEFTMOTORSTRAIGHT, SPEED);
  analogWrite(LEFTMOTORBACK, 0);
  analogWrite(RIGHTMOTORSTRAIGHT, 0);
  analogWrite(RIGHTMOTORBACK, SPEED + 10);

  delay(TURN_90_TIME);
  stopMoving();
}

void turnAround(){
  moveBack();
  delay(500);

  analogWrite(LEFTMOTORSTRAIGHT, SPEED + SPEED_DIFFERENCE);
  analogWrite(LEFTMOTORBACK, 0);
  analogWrite(RIGHTMOTORSTRAIGHT, 0);
  analogWrite(RIGHTMOTORBACK, SPEED + SPEED_DIFFERENCE);

  delay(700);
  stopMoving();
}

// Move backward slightly
void moveBack(){
  pixels.setPixelColor(0, pixels.Color(0, 255, 0));   
  pixels.setPixelColor(1, pixels.Color(0, 255, 0)); 
  pixels.setPixelColor(2, pixels.Color(0, 0, 0));   
  pixels.setPixelColor(3, pixels.Color(0, 0, 0));  
  pixels.show();

  analogWrite(LEFTMOTORSTRAIGHT, 0);
  analogWrite(LEFTMOTORBACK, SPEED);
  analogWrite(RIGHTMOTORSTRAIGHT, 0);
  analogWrite(RIGHTMOTORBACK, SPEED - SPEED_DIFFERENCE);

  delay(350);
  stopMoving();
}

// ULTRASONIC SENSOR 
long readDistance(int trig, int echo){
  digitalWrite(trig, LOW);
  delayMicroseconds(2);

  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  long duration = pulseIn(echo, HIGH, 6000);

  if (duration == 0){
    return 1000; // No signal = far distance
  }

  return duration * 0.0343 / 2; // Convert to cm
}

// MAZE NAVIGATION 
void maze(){
  stopMoving();

  if (distanceFront < WALL_ALIGNING_DISTANCE){
    moveBack();
    return;
  }

  if (distanceLeft > EMPTY_WAY_DISTANCE){  // left hand rule
    moveAndCorrect(TIME_WHEN_WAY_DETECTED);
    stopMoving();

    turnLeft(TURN_90_TIME);
    moveAndCorrect(TIME_FOR_NEW_PATH);
  }
  else if (distanceFront > STOP_DISTANCE){
    moveAndCorrect(300);
  }
  else if (distanceRight > EMPTY_WAY_DISTANCE){
    moveAndCorrect(TIME_WHEN_WAY_DETECTED);
    stopMoving();

    turnRight();
    moveAndCorrect(TIME_FOR_NEW_PATH);
  }
  else{
    turnAround();
  }
}

// LINE FOLLOWING 
void readSensors() { // reads all sensors value
  for (int i = 0; i < NUM_SENSORS; i++) {  
    sensorValues[i] = analogRead(sensorPins[i]); 
  }
}

// checks if line detected
bool checkLine(){
  readSensors();

  for (int i = 0; i < NUM_SENSORS; i++){
    if  (sensorValues[i] >= blackDetection){
      return true; // Line detected
    }
  }
  return false;
}

void lineFollowing()
{
  readSensors();

  bool allBlack = true;

  // Check if all sensors detect black (finish condition)
  for (int i = 0; i < NUM_SENSORS; i++) {
    if (sensorValues[i] < blackDetection) {
      allBlack = false;
      break;
    }
  }

  if (allBlack){
    stopMoving();

    pixels.setPixelColor(0, pixels.Color(255, 0, 0));   
    pixels.setPixelColor(1, pixels.Color(255, 0, 0));  
    pixels.setPixelColor(2, pixels.Color(255, 0, 0));   
    pixels.setPixelColor(3, pixels.Color(255, 0, 0));  
    pixels.show();

    holdGripper(GRIPPER_OPEN, 2000); 

    while (true){
      stopMoving(); // Stay stopped forever
    }
  }
  // Adjust movement based on sensor value
  else if (sensorValues[3] >= blackDetection && sensorValues[4] >= blackDetection){
    moveForward(180, 180);
  }else if (sensorValues[5] >= blackDetection){
    moveForward(100, 200);
  }else if (sensorValues[2] >= blackDetection){
    moveForward(160, 100);
  }else if (sensorValues[6] >= blackDetection){
    moveForward(50, 230);
  }else if (sensorValues[7] >= blackDetection){
    moveForward(0, 230);
  }else if (sensorValues[1] >= blackDetection){
    moveForward(190, 50);
  }else if (sensorValues[0] >= blackDetection){
    moveForward(230, 0);
  }
}
