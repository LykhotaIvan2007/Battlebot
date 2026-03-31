// Pins controlling the left motor (backward / forward)
const int LEFTMOTORBACK = 8;
const int LEFTMOTORSTRAIGHT = 9;

// Pins controlling the right motor (backward / forward)
const int RIGHTMOTORBACK = 12;
const int RIGHTMOTORSTRAIGHT = 13;

// Encoder pins for right and left wheels
const int R1 = 2;
const int R2 = 3;

// Pulse counters from encoders (volatile because used in interrupts)
volatile int pulsesRight = 0;
volatile int pulsesLeft = 0;

void setup() {
  // Set motor pins as outputs
  pinMode(LEFTMOTORBACK,OUTPUT);
  pinMode(LEFTMOTORSTRAIGHT,OUTPUT);
  pinMode(RIGHTMOTORBACK,OUTPUT);
  pinMode(RIGHTMOTORSTRAIGHT,OUTPUT);

  // Attach interrupts for encoder pulse counting
  attachInterrupt(digitalPinToInterrupt(R1), countRight, RISING);
  attachInterrupt(digitalPinToInterrupt(R2), countLeft, RISING);
}

void loop() {
  // Calculate number of right wheel rotations (20 pulses = 1 rotation)
  int rightWeeelRotation = pulsesRight / 20;
  
  // If between 5 and 10 rotations  move backward
  if(rightWeeelRotation >= 5 && rightWeeelRotation < 10) {
    moveBackwards(200);

  // If less than 5 rotations move forward
  } else if(rightWeeelRotation < 5) {
    moveForward(200);
  }
  
  // When exactly 10 rotations are reached
  if(rightWeeelRotation == 10) {
    stopMoving(); // stop the robot

    // Save current pulse count
    int rightWeeelRotationNow = pulsesRight;

    // Turn left until 9 additional pulses are counted
    while(pulsesRight < rightWeeelRotationNow + 9) {
      leftTurn(200);
    }
    
    stopMoving();
    delay(1000); // wait 1 second

    // Update current pulse count
    rightWeeelRotationNow = pulsesRight;
    
    // Turn right until 9 additional pulses are counted
    while(pulsesRight < rightWeeelRotationNow + 9) {
      rightTurn(200);
    }
    
    stopMoving();    
  }
}

// Function to move forward
void moveForward(int speed) {
  // Right motor forward (with speed adjustment)
  analogWrite(RIGHTMOTORSTRAIGHT, speed - 65);
  // Left motor forward (with speed adjustment)
  analogWrite(LEFTMOTORSTRAIGHT, speed + 55);
  analogWrite(RIGHTMOTORBACK, 0);
  analogWrite(LEFTMOTORBACK, 0);
}

// Function to move backward
void moveBackwards(int speed) {
  // Disable forward movement
  analogWrite(RIGHTMOTORSTRAIGHT, 0);
  analogWrite(LEFTMOTORSTRAIGHT, 0);

  // Move backward (with adjustment)
  analogWrite(RIGHTMOTORBACK, speed);
  analogWrite(LEFTMOTORBACK, speed + 55);
}

// Function to stop all movement
void stopMoving() {
  analogWrite(RIGHTMOTORSTRAIGHT, 0);
  analogWrite(LEFTMOTORSTRAIGHT, 0);
  analogWrite(RIGHTMOTORBACK, 0);
  analogWrite(LEFTMOTORBACK, 0);
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

// Interrupt handler for left wheel (increments pulse counter)
void countLeft() {
  pulsesLeft++;
}

// Interrupt handler for right wheel (increments pulse counter)
void countRight() {
  pulsesRight++;
}
