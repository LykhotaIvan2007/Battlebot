const int LEFTMOTORBACK = 8;
const int LEFTMOTORSTRAIGHT = 9;
const int RIGHTMOTORBACK = 12;
const int RIGHTMOTORSTRAIGHT = 13; 
const int sensorPins[8] = {A0, A1, A2, A3, A4, A5, A6, A7};

int weights[8] = {-350,-250,-150,-50,50,150,250,350};
float Kp = 0.35;               
int threshold = 500;           
int lastValue = 0;             

void setup() {
  pinMode(LEFTMOTORBACK, OUTPUT);
  pinMode(LEFTMOTORSTRAIGHT, OUTPUT);
  pinMode(RIGHTMOTORBACK, OUTPUT);
  pinMode(RIGHTMOTORSTRAIGHT, OUTPUT);

  for (int i = 0; i < 8; i++) pinMode(sensorPins[i], INPUT);

  stopMoving();
}

void loop() {
  int value = readLine();

  if (value == 10000) {
    int spin = (lastValue >= 0) ? 120 : -120; 
  
    int left  = constrain(140  - spin, 0, 255);
    int right = constrain(125 + spin, 0, 255);
  
    analogWrite(LEFTMOTORSTRAIGHT, left);
    digitalWrite(LEFTMOTORBACK, 0);
    analogWrite(RIGHTMOTORSTRAIGHT, right);
    digitalWrite(RIGHTMOTORBACK, 0);
  
    delay(20);
    return;
  }

  lastValue = value;

  int correction = (int)(value * Kp);
  int leftSpeed  = 140  - correction;
  int rightSpeed = 135 + correction;

  leftSpeed  = constrain(leftSpeed, 0, 255);
  rightSpeed = constrain(rightSpeed, 0, 255);
  
  analogWrite(LEFTMOTORSTRAIGHT, leftSpeed);
  digitalWrite(LEFTMOTORBACK, 0);
  analogWrite(RIGHTMOTORSTRAIGHT, rightSpeed);
  digitalWrite(RIGHTMOTORBACK, 0);
  delay(10);
}

void stopMoving() {
  analogWrite(RIGHTMOTORSTRAIGHT,0);
  analogWrite(LEFTMOTORSTRAIGHT,0);
  analogWrite(RIGHTMOTORBACK, 0);
  analogWrite(LEFTMOTORBACK, 0);
}

int readLine() {
  long sum = 0;
  long total = 0;

  for (int i = 0; i < 8; i++) {
    int sensorValue = analogRead(sensorPins[i]);

    int howLineIsVisible = sensorValue - threshold;
    if (howLineIsVisible < 0) howLineIsVisible = 0;            

    sum += (long) howLineIsVisible * weights[i];
    total += howLineIsVisible;
  }
  if (total < 80) return 10000;   
  return (int)(sum / total);
}
