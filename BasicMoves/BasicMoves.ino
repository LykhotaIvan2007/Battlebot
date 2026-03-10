const int LEFTMOTORBACK = 8;
const int LEFTMOTORSTRAIGHT = 9;
const int RIGHTMOTORBACK = 12;
const int RIGHTMOTORSTRAIGHT = 13;
const int R1 = 2;
const int R2 = 3;
const int PULSESFOR90DEEGREROTATION = 20;

volatile int pulsesRight = 0;
volatile int pulsesLeft = 0;
bool stateForLeftTurn = true;
bool stateForRightTurn = true;

void setup() {
  pinMode(LEFTMOTORBACK,OUTPUT);
  pinMode(LEFTMOTORSTRAIGHT,OUTPUT);
  pinMode(RIGHTMOTORBACK,OUTPUT);
  pinMode(RIGHTMOTORSTRAIGHT,OUTPUT);
  attachInterrupt(digitalPinToInterrupt(R1), countRight, RISING);
  attachInterrupt(digitalPinToInterrupt(R2), countLeft, RISING);
}

void loop() {
  int rightWeeelRotation = pulsesRight / 20;
  
  if(rightWeeelRotation >= 5 && rightWeeelRotation < 10) {
    moveBackwards(200);
  }else if(rightWeeelRotation <5) {
    moveForward(200);
  }
  
  if(rightWeeelRotation == 10) {
    stopMoving();
    int rightWeeelRotationNow = pulsesRight;
    while(pulsesRight < rightWeeelRotationNow+9) {
      leftTurn(200);
    }
    
    stopMoving();
    delay(1000);
    rightWeeelRotationNow = pulsesRight;
    
    while(pulsesRight < rightWeeelRotationNow+9) {
      rightTurn(200);
    }
    
    stopMoving();    
  }
}

// function for mooving forward
void moveForward(int speed) {
  analogWrite(RIGHTMOTORSTRAIGHT,speed-65);
  analogWrite(LEFTMOTORSTRAIGHT,speed+55);
  analogWrite(RIGHTMOTORBACK,0);
  analogWrite(LEFTMOTORBACK,0);
}

// function for mooving backwards
void moveBackwards(int speed) {
  analogWrite(RIGHTMOTORSTRAIGHT,0);
  analogWrite(LEFTMOTORSTRAIGHT, 0);
  analogWrite(RIGHTMOTORBACK,speed);
  analogWrite(LEFTMOTORBACK, speed+55);
}

// function for stop mooving
void stopMoving() {
  analogWrite(RIGHTMOTORSTRAIGHT,0);
  analogWrite(LEFTMOTORSTRAIGHT,0);
  analogWrite(RIGHTMOTORBACK, 0);
  analogWrite(LEFTMOTORBACK, 0);
}

// function for moving left
void leftTurn (int speed) {
    analogWrite(RIGHTMOTORSTRAIGHT,speed+20);
    analogWrite(LEFTMOTORSTRAIGHT,0);
    analogWrite(RIGHTMOTORBACK, 0);
    analogWrite(LEFTMOTORBACK, speed);
}

// function for moving right
void rightTurn (int speed) {
    analogWrite(RIGHTMOTORSTRAIGHT,0);
    analogWrite(LEFTMOTORSTRAIGHT,speed+50);
    analogWrite(RIGHTMOTORBACK, speed);
    analogWrite(LEFTMOTORBACK, 0);
}

//function for counting how much pulses left wheel made
void countLeft() {
  pulsesLeft++;
}

//function for counting how much pulses right wheel made
void countRight() {
  pulsesRight++;
}
