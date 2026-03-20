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
  
  distanceLeft = readDistance(TRIGPIN_LEFT, ECHOPIN_LEFT);
  distanceRight = readDistance(TRIGPIN_RIGHT, ECHOPIN_RIGHT);
  distanceFront = readDistance(TRIGPIN, ECHOPIN);
  Serial.println(distanceFront);
  if(distanceFront < 30 && parkingMode == true){
    holdGripper(GRIPPER_OPEN, 1000);
    //delay(1000);
    moveForward(SPEED, SPEED);
    delay(2050);
    stopMoving();
    holdGripper(GRIPPER_CLOSE, 1000);
    turnLeft();
    turnLeft();
    moveForward(SPEED, SPEED);
    delay(4000);
    parkingMode = false;
  }
  if (!parkingMode){
    if (checkLine()){
      gripper(0);
      lineFollowing();
    }else{
      gripper(0);
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
  analogWrite(LEFTMOTORBACK, SPEED);

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

  delay(400);
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

bool checkLine(){
  int s1 = analogRead(LINE_SENSOR1);
  int s2 = analogRead(LINE_SENSOR2);
  int s3 = analogRead(LINE_SENSOR3);
  int s4 = analogRead(LINE_SENSOR4);
  int s5 = analogRead(LINE_SENSOR5);
  int s6 = analogRead(LINE_SENSOR6);
  int s7 = analogRead(LINE_SENSOR7);
  int s8 = analogRead(LINE_SENSOR8);

  if (s1 >= 950 || s2 >= 950 || s3 >= 950 || s4 >= 950 || s5 >= 950 || s6 >= 950 || s7 >= 950 || s8 >= 950)
  {
    return true;
  }
  return false;
}

void lineFollowing()
{
  int s1 = analogRead(LINE_SENSOR1);
  int s2 = analogRead(LINE_SENSOR2);
  int s3 = analogRead(LINE_SENSOR3);
  int s4 = analogRead(LINE_SENSOR4);
  int s5 = analogRead(LINE_SENSOR5);
  int s6 = analogRead(LINE_SENSOR6);
  int s7 = analogRead(LINE_SENSOR7);
  int s8 = analogRead(LINE_SENSOR8);

  if (s1 >= blackDetection && s2 >= blackDetection && s3 >= blackDetection && s4 >= blackDetection && s5 >= blackDetection && s6 >= blackDetection && s7 >= blackDetection && s8 >= blackDetection)
  {
    stopMoving();
    pixels.setPixelColor(0, pixels.Color(255, 0, 0));   
    pixels.setPixelColor(1, pixels.Color(255, 0, 0));  
    pixels.setPixelColor(2, pixels.Color(255, 0, 0));   
    pixels.setPixelColor(3, pixels.Color(255, 0, 0));  
    pixels.show();
    holdGripper(GRIPPER_OPEN, 2000); 
    delay(10000);
  }
  else if (s4 >= blackDetection && s5 >= blackDetection){
    analogWrite(LEFTMOTORSTRAIGHT, 180);
    analogWrite(LEFTMOTORBACK, 0);

    analogWrite(RIGHTMOTORSTRAIGHT, 180);
    analogWrite(RIGHTMOTORBACK, 0);
  }else if (s6 >= blackDetection){
    analogWrite(LEFTMOTORSTRAIGHT, 100);
    analogWrite(LEFTMOTORBACK, 0);

    analogWrite(RIGHTMOTORSTRAIGHT, 200);
    analogWrite(RIGHTMOTORBACK, 0);
  }else if (s3 >= blackDetection){
    analogWrite(LEFTMOTORSTRAIGHT, 160);
    analogWrite(LEFTMOTORBACK, 0);

    analogWrite(RIGHTMOTORSTRAIGHT, 100);
    analogWrite(RIGHTMOTORBACK, 0);
  }else if (s7 >= blackDetection){
    analogWrite(LEFTMOTORSTRAIGHT, 50);
    analogWrite(LEFTMOTORBACK, 0);

    analogWrite(RIGHTMOTORSTRAIGHT, 230);
    analogWrite(RIGHTMOTORBACK, 0);
  }else if (s8 >= blackDetection){
    analogWrite(LEFTMOTORSTRAIGHT, 0);
    analogWrite(LEFTMOTORBACK, 0);

    analogWrite(RIGHTMOTORSTRAIGHT, 230);
    analogWrite(RIGHTMOTORBACK, 0);
  }else if (s2 >= blackDetection){
    analogWrite(LEFTMOTORSTRAIGHT, 190);
    analogWrite(LEFTMOTORBACK, 0);

    analogWrite(RIGHTMOTORSTRAIGHT, 50);
    analogWrite(RIGHTMOTORBACK, 0);
  }else if (s1 >= blackDetection){
    analogWrite(LEFTMOTORSTRAIGHT, 230);
    analogWrite(LEFTMOTORBACK, 0);

    analogWrite(RIGHTMOTORSTRAIGHT, 0);
    analogWrite(RIGHTMOTORBACK, 0);
  }
}
