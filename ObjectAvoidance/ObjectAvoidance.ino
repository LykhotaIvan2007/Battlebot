const int LEFTMOTORBACK = 8;
const int LEFTMOTORSTRAIGHT = 9;
const int RIGHTMOTORBACK = 12;
const int RIGHTMOTORSTRAIGHT = 13;
const int R1 = 2;
const int R2 = 3;
const int TRIGPIN = 10;
const int ECHOPIN = 11;

volatile int pulsesRight = 0;
volatile int pulsesLeft = 0;
unsigned long previousTime = 0;


void setup() {
  // put your setup code here, to run once:
  pinMode(LEFTMOTORBACK,OUTPUT);
  pinMode(LEFTMOTORSTRAIGHT,OUTPUT);
  pinMode(RIGHTMOTORBACK,OUTPUT);
  pinMode(RIGHTMOTORSTRAIGHT,OUTPUT);
  attachInterrupt(digitalPinToInterrupt(R1), countRight, RISING);
  attachInterrupt(digitalPinToInterrupt(R2), countLeft, RISING);
  pinMode(TRIGPIN, OUTPUT);
  pinMode(ECHOPIN, INPUT);
  digitalWrite(TRIGPIN, LOW);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  
  float distance = readDistance();

  if (distance <= 15) {
     stopMoving();
     int rightCurrentPulses = pulsesRight;
      while(pulsesRight < rightCurrentPulses + 13) {
        leftTurn(200);
      }
    stopMoving();
    rightCurrentPulses = pulsesRight;
    while (pulsesRight < rightCurrentPulses+40)
    {
      moveForward(200);
    }
    stopMoving();
    rightCurrentPulses = pulsesRight;
    while(pulsesRight < rightCurrentPulses + 12) {
      rightTurn(200);
    }
    stopMoving();
    rightCurrentPulses = pulsesRight;
    while (pulsesRight < rightCurrentPulses+60)
    {
      moveForward(200);
    }
    stopMoving();
    rightCurrentPulses = pulsesRight;
      while(pulsesRight < rightCurrentPulses + 12) {
        rightTurn(200);
      }
      stopMoving();
      rightCurrentPulses = pulsesRight;
    while (pulsesRight < rightCurrentPulses+40)
    {
      moveForward(200);
    }
    stopMoving();

    rightCurrentPulses = pulsesRight;
    while(pulsesRight < rightCurrentPulses + 13) {
      leftTurn(200);
    }
    stopMoving();
            
  } else {
    moveForward(200); 
  }


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

// function for mooving forward
void moveForward(int speed) {
  analogWrite(RIGHTMOTORSTRAIGHT,speed-65);
  analogWrite(LEFTMOTORSTRAIGHT,speed+55);
  analogWrite(RIGHTMOTORBACK,0);
  analogWrite(LEFTMOTORBACK,0);
}


// function for stop mooving
void stopMoving() {
  analogWrite(RIGHTMOTORSTRAIGHT,0);
  analogWrite(LEFTMOTORSTRAIGHT,0);
  analogWrite(RIGHTMOTORBACK, 0);
  analogWrite(LEFTMOTORBACK, 0);
}

float readDistance() {
  digitalWrite(TRIGPIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGPIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGPIN, LOW);
  float duration = pulseIn(ECHOPIN, HIGH);
  float distance = (duration * 0.0343) / 2;
  return distance;
}

//function for counting how much pulses left wheel made
void countLeft() {
  pulsesLeft++;
}

//function for counting how much pulses right wheel made
void countRight() {
  pulsesRight++;
}
