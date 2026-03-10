#define SERVO 10
#define GRIPPER_OPEN 1600
#define GRIPPER_CLOSE 990

const int LEFTMOTORBACK = 8;
const int LEFTMOTORSTRAIGHT = 9;
const int RIGHTMOTORBACK = 12;
const int RIGHTMOTORSTRAIGHT = 13;

const int R1 = 2;
const int R2 = 3;

const int PULSES_FOR_25CM = 40;

volatile int pulsesRight = 0;
volatile int pulsesLeft = 0;

boolean first = true;
boolean second = false;
boolean third = false;
unsigned long startTime = millis();

void setup() {

pinMode(SERVO, OUTPUT);

pinMode(LEFTMOTORBACK, OUTPUT);
pinMode(LEFTMOTORSTRAIGHT, OUTPUT);
pinMode(RIGHTMOTORBACK, OUTPUT);
pinMode(RIGHTMOTORSTRAIGHT, OUTPUT);

attachInterrupt(digitalPinToInterrupt(R1), countRight, RISING);
attachInterrupt(digitalPinToInterrupt(R2), countLeft, RISING);

stopMoving();
}

void loop() {

// open gripper
holdGripper(GRIPPER_OPEN,1000);

if (first){
    holdGripper(GRIPPER_CLOSE, 2000);
    first = false;
    second = true;
    startTime = millis();
  }

  if (second){
    holdGripper(GRIPPER_OPEN, 2000);
    second = false;
    third = true;
    startTime = millis();
  }

if (third){
  // move 25 cm
  moveDistance(PULSES_FOR_25CM);
  
  stopMoving();
  
  
  // grab cone
  holdGripper(GRIPPER_CLOSE,1000);
  
  // move another 25 cm
  moveDistance(PULSES_FOR_25CM);
  
  stopMoving();
  
  while(1) {
    gripper(0);
  }
}


}

void moveDistance(int pulsesNeeded){

int start = pulsesRight;

while(pulsesRight < start + pulsesNeeded){

moveForward(200);  
gripper(0);   // keep servo signal alive

}

}

void moveForward(int speed){
analogWrite(RIGHTMOTORSTRAIGHT,speed-65);
analogWrite(LEFTMOTORSTRAIGHT,speed+55);
analogWrite(RIGHTMOTORBACK,0);
analogWrite(LEFTMOTORBACK,0);
}

void stopMoving(){
analogWrite(RIGHTMOTORSTRAIGHT,0);
analogWrite(LEFTMOTORSTRAIGHT,0);
analogWrite(RIGHTMOTORBACK,0);
analogWrite(LEFTMOTORBACK,0);
}

void countLeft(){
pulsesLeft++;
}

void countRight(){
pulsesRight++;
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
