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
}

boolean fl = true;

void loop(){
  distanceLeft = readDistance(TRIGPIN_LEFT, ECHOPIN_LEFT);
  distanceRight = readDistance(TRIGPIN_RIGHT, ECHOPIN_RIGHT);
  distanceFront = readDistance(TRIGPIN, ECHOPIN);

  Serial.println(distanceRight);

  physicalMaze();
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

    if (currentLeft < 8){
      moveForward(SPEED + 20, SPEED - 10);
    }
    else if (currentRight < 8){
      moveForward(SPEED - 10, SPEED + 20);
    }
    else{
      moveForward(SPEED, SPEED);
    }

    delay(20);
  }

  stopMoving();
}

void moveForward(int leftSpeed, int rightSpeed){
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
  analogWrite(LEFTMOTORSTRAIGHT, 0);
  analogWrite(LEFTMOTORBACK, SPEED);

  analogWrite(RIGHTMOTORSTRAIGHT, SPEED);
  analogWrite(RIGHTMOTORBACK, 0);

  delay(TURN_90_TIME);
  stopMoving();
}

void turnRight(){
  analogWrite(LEFTMOTORSTRAIGHT, SPEED);
  analogWrite(LEFTMOTORBACK, 0);

  analogWrite(RIGHTMOTORSTRAIGHT, 0);
  analogWrite(RIGHTMOTORBACK, SPEED + 10);

  delay(600);
  stopMoving();
}

void turnAround(){
  analogWrite(LEFTMOTORSTRAIGHT, SPEED);
  analogWrite(LEFTMOTORBACK, 0);

  analogWrite(RIGHTMOTORSTRAIGHT, 0);
  analogWrite(RIGHTMOTORBACK, SPEED);

  delay(200);
  moveBack();
  delay(400);

  analogWrite(LEFTMOTORSTRAIGHT, SPEED);
  analogWrite(LEFTMOTORBACK, 0);

  analogWrite(RIGHTMOTORSTRAIGHT, 0);
  analogWrite(RIGHTMOTORBACK, SPEED);

  delay(300);
  stopMoving();
}

void moveBack(){
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

void physicalMaze(){
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
