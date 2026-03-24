#include <Adafruit_NeoPixel.h>

const int SPEED = 160;
const int RIGHT_MOTOR_SPEED_DERCREMENTATION = 20;

int TURN_90_TIME = 450;
int TURN_180_TIME = 800;

const int EMPTY_WAY_DISTANCE = 30;
const int STOP_DISTANCE = 15;

const int TIME_WHEN_WAY_DETECTED = 250;
const int TIME_FOR_NEW_PATH = 400;

const int LEFTMOTORBACK = 6;
const int LEFTMOTORSTRAIGHT = 9;
const int RIGHTMOTORBACK = 10;
const int RIGHTMOTORSTRAIGHT = 11;

const int ECHOPIN_RIGHT = 2;
const int TRIGPIN_RIGHT = 3;
const int ECHOPIN = 4;
const int TRIGPIN = 5;
const int ECHOPIN_LEFT = 7;
const int TRIGPIN_LEFT = 8;

const int SERVO = 12;
const int GRIPPER_OPEN = 1600;
const int GRIPPER_CLOSE = 990;

const int LINE_SENSOR1 = A0;
const int LINE_SENSOR2 = A1;
const int LINE_SENSOR3 = A2;
const int LINE_SENSOR4 = A3;
const int LINE_SENSOR5 = A4;
const int LINE_SENSOR6 = A5;
const int LINE_SENSOR7 = A6;
const int LINE_SENSOR8 = A7;

const int sensorPins[8] = {
  LINE_SENSOR1, LINE_SENSOR2, LINE_SENSOR3, LINE_SENSOR4,
  LINE_SENSOR5, LINE_SENSOR6, LINE_SENSOR7, LINE_SENSOR8
};

int sensorValues[8];

const int NUM_SENSORS = 8;

int blackDetection = 900;

long distanceFront, distanceRight, distanceLeft;

Adafruit_NeoPixel pixels(4, 13, NEO_GRB + NEO_KHZ800);

void setup(){
  Serial.begin(9600);
  pinMode(LEFTMOTORBACK, OUTPUT);
  pinMode(LEFTMOTORSTRAIGHT, OUTPUT);
  pinMode(RIGHTMOTORBACK, OUTPUT);
  pinMode(RIGHTMOTORSTRAIGHT, OUTPUT);
  pinMode(TRIGPIN_RIGHT, OUTPUT);
  pinMode(ECHOPIN_RIGHT, INPUT);
  pinMode(TRIGPIN, OUTPUT);
  pinMode(ECHOPIN, INPUT);
  pinMode(TRIGPIN_LEFT, OUTPUT);
  pinMode(ECHOPIN_LEFT, INPUT);
  pinMode(SERVO, OUTPUT);
  holdGripper(GRIPPER_OPEN, 1000);
  for (int i = 14; i < 22; i++){
    pinMode(i, INPUT);
  }
  pixels.begin();      
  pixels.clear();      
  pixels.show();       
}

boolean parkingMode = true;
boolean mode = true; // true = maze || false = line

void loop(){
  gripper(0);
  distanceLeft = readDistance(TRIGPIN_LEFT, ECHOPIN_LEFT);
  distanceRight = readDistance(TRIGPIN_RIGHT, ECHOPIN_RIGHT);
  distanceFront = readDistance(TRIGPIN, ECHOPIN);
  if(distanceFront < 30 && parkingMode == true){
    //delay(1000);
    while (distanceLeft < 10) {
      moveForward(SPEED, SPEED);
      distanceLeft = readDistance(TRIGPIN_LEFT, ECHOPIN_LEFT);
    }
    delay (750);
    stopMoving();
    holdGripper(GRIPPER_CLOSE, 1000);
    turnLeft();
    turnLeft();
    moveForward(SPEED, SPEED);
    delay(700);
    moveAndCorrect(3000);
    parkingMode = false;
  }
  if (!parkingMode){
    if (checkLine()){
      lineFollowing();
    }else{
      maze();
    }
  }
  
  //pixels.setPixelColor(0, pixels.Color(255, 0, 0));   // зеленый 0 - левый нижний
  //pixels.setPixelColor(1, pixels.Color(255, 255, 0)); // жёлтый  1 - правый нижний 
  //pixels.setPixelColor(2, pixels.Color(0, 0, 255));   // синий 2 - правый верхний
  //pixels.setPixelColor(3, pixels.Color(0, 255, 0));  // крассный 3 - левый верхний
  //pixels.show();
}

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
    timer = millis() + 20;
  }
}

void moveAndCorrect(int duration){
  unsigned long startTime = millis();
  while (millis() - startTime < duration){
    long currentLeft = readDistance(TRIGPIN_LEFT, ECHOPIN_LEFT);
    long currentRight = readDistance(TRIGPIN_RIGHT, ECHOPIN_RIGHT);
    if (readDistance(TRIGPIN, ECHOPIN) < STOP_DISTANCE){
      stopMoving();
      break;
    }
    if (currentLeft < 9){
      moveForward(SPEED + 30, SPEED - 10);
    }
    else if (currentRight < 9){
      moveForward(SPEED - 10, SPEED + 30);
    }
    else{
      moveForward(SPEED, SPEED);
    }
    delay(20);
  }
  stopMoving();
}

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

void stopMoving(){
  analogWrite(RIGHTMOTORSTRAIGHT, 0);
  analogWrite(LEFTMOTORSTRAIGHT, 0);
  analogWrite(RIGHTMOTORBACK, 0);
  analogWrite(LEFTMOTORBACK, 0);
}

void turnLeft(){
  pixels.setPixelColor(0, pixels.Color(255, 255, 0));
  pixels.setPixelColor(1, pixels.Color(0, 0, 0));
  pixels.setPixelColor(2, pixels.Color(0, 0, 0));
  pixels.setPixelColor(3, pixels.Color(255, 255, 0)); 
  pixels.show();
  analogWrite(LEFTMOTORSTRAIGHT, 0);
  analogWrite(LEFTMOTORBACK, SPEED + 20);
  analogWrite(RIGHTMOTORSTRAIGHT, SPEED);
  analogWrite(RIGHTMOTORBACK, 0);
  delay(TURN_90_TIME);
  stopMoving();
}

void turnRight(){
  pixels.setPixelColor(0, pixels.Color(0, 0, 0));   // зеленый 0 - левый нижний
  pixels.setPixelColor(1, pixels.Color(255, 255, 0)); // жёлтый  1 - правый нижний 
  pixels.setPixelColor(2, pixels.Color(255, 255, 0));   // синий 2 - правый верхний
  pixels.setPixelColor(3, pixels.Color(0, 0, 0));  // крассный 3 - левый верхний
  pixels.show();
  analogWrite(LEFTMOTORSTRAIGHT, SPEED);
  analogWrite(LEFTMOTORBACK, 0);
  analogWrite(RIGHTMOTORSTRAIGHT, 0);
  analogWrite(RIGHTMOTORBACK, SPEED + 10);
  delay(600);
  stopMoving();
}

void turnAround(){
  moveBack();
  delay(500);
  analogWrite(LEFTMOTORSTRAIGHT, SPEED+20);
  analogWrite(LEFTMOTORBACK, 0);
  analogWrite(RIGHTMOTORSTRAIGHT, 0);
  analogWrite(RIGHTMOTORBACK, SPEED+20);
  delay(700);
  stopMoving();
}

void moveBack(){
  pixels.setPixelColor(0, pixels.Color(0, 255, 0));   
  pixels.setPixelColor(1, pixels.Color(0, 255, 0)); 
  pixels.setPixelColor(2, pixels.Color(0, 0, 0));   
  pixels.setPixelColor(3, pixels.Color(0, 0, 0));  
  pixels.show();
  analogWrite(LEFTMOTORSTRAIGHT, 0);
  analogWrite(LEFTMOTORBACK, SPEED);
  analogWrite(RIGHTMOTORSTRAIGHT, 0);
  analogWrite(RIGHTMOTORBACK, SPEED - RIGHT_MOTOR_SPEED_DERCREMENTATION);
  delay(350);
  stopMoving();
}

long readDistance(int trig, int echo){
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long duration = pulseIn(echo, HIGH, 6000);
  if (duration == 0){
    return 1000;
  }
  return duration * 0.0343 / 2;
}

void maze(){
  stopMoving();

  if (distanceFront < 9){
    moveBack();
    return;
  }

  if (distanceLeft > EMPTY_WAY_DISTANCE){
    moveAndCorrect(TIME_WHEN_WAY_DETECTED);
    stopMoving();

    turnLeft();
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

void readSensors() {
  for (int i = 0; i < NUM_SENSORS; i++) {
    sensorValues[i] = analogRead(sensorPins[i]);
  }
}

bool checkLine(){
  readSensors();
  for (int i = 0; i < NUM_SENSORS; i++){
    if  (sensorValues[i] >= blackDetection){
      return true;
    }
  }
  return false;
}

void lineFollowing()
{
  readSensors();
  bool allBlack = true;
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
      stopMoving();
    }
  }
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
