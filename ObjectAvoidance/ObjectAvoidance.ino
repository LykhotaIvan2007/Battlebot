// Pins controlling the left motor (backward / forward)
const int LEFTMOTORBACK = 8;
const int LEFTMOTORSTRAIGHT = 9;

// Pins controlling the right motor (backward / forward)
const int RIGHTMOTORBACK = 12;
const int RIGHTMOTORSTRAIGHT = 13;

// Pins for right and left wheels
const int R1 = 2;
const int R2 = 3;

// Ultrasonic sensor pins 
const int TRIGPIN = 10;
const int ECHOPIN = 11;

// Pulse counters from encoders (used in interrupts)
volatile int pulsesRight = 0;
volatile int pulsesLeft = 0;

void setup() {
  // Set motor control pins as outputs
  pinMode(LEFTMOTORBACK,OUTPUT);
  pinMode(LEFTMOTORSTRAIGHT,OUTPUT);
  pinMode(RIGHTMOTORBACK,OUTPUT);
  pinMode(RIGHTMOTORSTRAIGHT,OUTPUT);

  // Attach interrupts for counting encoder pulses
  attachInterrupt(digitalPinToInterrupt(R1), countRight, RISING);
  attachInterrupt(digitalPinToInterrupt(R2), countLeft, RISING);

  // Set ultrasonic sensor pins
  pinMode(TRIGPIN, OUTPUT);
  pinMode(ECHOPIN, INPUT);

  // Ensure TRIG pin starts LOW
  digitalWrite(TRIGPIN, LOW);
  
}

void loop() {
  // Read distance from ultrasonic sensor (in cm)
  float distance = readDistance();

  // If obstacle is closer than or equal to 15 cm
  if (distance <= 15) {
     stopMoving(); // stop the robot

     // Save current pulse count
     int rightCurrentPulses = pulsesRight;

     // Turn left until 13 pulses are added
     while(pulsesRight < rightCurrentPulses + 13) {
        leftTurn(200);
     }

    stopMoving();

    // Move forward for 40 pulses
    rightCurrentPulses = pulsesRight;
    while (pulsesRight < rightCurrentPulses + 40)
    {
      moveForward(200);
    }

    stopMoving();

    // Turn right for 12 pulses
    rightCurrentPulses = pulsesRight;
    while(pulsesRight < rightCurrentPulses + 12) {
      rightTurn(200);
    }

    stopMoving();

    // Move forward for 60 pulses
    rightCurrentPulses = pulsesRight;
    while (pulsesRight < rightCurrentPulses + 60)
    {
      moveForward(200);
    }

    stopMoving();

    // Turn right again for 12 pulses
    rightCurrentPulses = pulsesRight;
    while(pulsesRight < rightCurrentPulses + 12) {
      rightTurn(200);
    }

    stopMoving();

    // Move forward for 40 pulses
    rightCurrentPulses = pulsesRight;
    while (pulsesRight < rightCurrentPulses + 40)
    {
      moveForward(200);
    }

    stopMoving();

    // Final left turn for 13 pulses
    rightCurrentPulses = pulsesRight;
    while(pulsesRight < rightCurrentPulses + 13) {
      leftTurn(200);
    }

    stopMoving();
            
  } else {
    // If no obstacle move forward continuously
    moveForward(200); 
  }

}

// Function to turn left
void leftTurn (int speed) {
    analogWrite(RIGHTMOTORSTRAIGHT, speed + 20);
    analogWrite(LEFTMOTORSTRAIGHT, 0);
    analogWrite(RIGHTMOTORBACK, 0);
    analogWrite(LEFTMOTORBACK, speed);
}

// Function to turn right
void rightTurn (int speed) {
    analogWrite(RIGHTMOTORSTRAIGHT, 0);
    analogWrite(LEFTMOTORSTRAIGHT, speed + 50);
    analogWrite(RIGHTMOTORBACK, speed);
    analogWrite(LEFTMOTORBACK, 0);
}

// Function to move forward
void moveForward(int speed) {
  analogWrite(RIGHTMOTORSTRAIGHT, speed - 65);
  analogWrite(LEFTMOTORSTRAIGHT, speed + 55);
  analogWrite(RIGHTMOTORBACK, 0);
  analogWrite(LEFTMOTORBACK, 0);
}


// Function to stop all motors
void stopMoving() {
  analogWrite(RIGHTMOTORSTRAIGHT, 0);
  analogWrite(LEFTMOTORSTRAIGHT, 0);
  analogWrite(RIGHTMOTORBACK, 0);
  analogWrite(LEFTMOTORBACK, 0);
}

// Function to read distance from ultrasonic sensor
float readDistance() {
  // Send trigger pulse
  digitalWrite(TRIGPIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGPIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGPIN, LOW);

  // Measure echo pulse duration
  float duration = pulseIn(ECHOPIN, HIGH);

  // Convert time to distance (cm)
  float distance = (duration * 0.0343) / 2;

  return distance;
}

// Interrupt function for counting left wheel pulses
void countLeft() {
  pulsesLeft++;
}

// Interrupt function for counting right wheel pulses
void countRight() {
  pulsesRight++;
}