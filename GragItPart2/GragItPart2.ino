// Servo control pin
#define SERVO 10

// Servo pulse values for gripper positions (in microseconds)
#define GRIPPER_OPEN 1600
#define GRIPPER_CLOSE 990

// Motor control pins (left and right motors)
const int LEFTMOTORBACK = 8;
const int LEFTMOTORSTRAIGHT = 9;
const int RIGHTMOTORBACK = 12;
const int RIGHTMOTORSTRAIGHT = 13;

// Encoder pins for right and left wheels
const int R1 = 2;
const int R2 = 3;

// Number of encoder pulses corresponding to ~25 cm movement
const int PULSES_FOR_25CM = 40;

// Pulse counters (updated inside interrupts)
volatile int pulsesRight = 0;
volatile int pulsesLeft = 0;

// State flags for sequence control
boolean first = true;
boolean second = false;
boolean third = false;

// Timer variable (used but not essential in current logic)
unsigned long startTime = millis();

void setup() {

  // Set servo pin as output
  pinMode(SERVO, OUTPUT);

  // Set motor pins as outputs
  pinMode(LEFTMOTORBACK, OUTPUT);
  pinMode(LEFTMOTORSTRAIGHT, OUTPUT);
  pinMode(RIGHTMOTORBACK, OUTPUT);
  pinMode(RIGHTMOTORSTRAIGHT, OUTPUT);

  // Attach interrupts for encoder pulse counting
  attachInterrupt(digitalPinToInterrupt(R1), countRight, RISING);
  attachInterrupt(digitalPinToInterrupt(R2), countLeft, RISING);

  // Ensure robot is stopped at startup
  stopMoving();
}

void loop() {

  // Open gripper at the beginning
  holdGripper(GRIPPER_OPEN, 1000);

  // First stage: close gripper
  if (first){
    holdGripper(GRIPPER_CLOSE, 2000);
    first = false;
    second = true;
    startTime = millis();
  }

  // Second stage: open gripper again
  if (second){
    holdGripper(GRIPPER_OPEN, 2000);
    second = false;
    third = true;
    startTime = millis();
  }

  // Third stage: main task sequence
  if (third){

    // Move forward ~25 cm
    moveDistance(PULSES_FOR_25CM);
    stopMoving();
  
    // Close gripper (grab object / cone)
    holdGripper(GRIPPER_CLOSE, 1000);
  
    // Move forward another ~25 cm
    moveDistance(PULSES_FOR_25CM);
    stopMoving();
  
    // Infinite loop to keep servo signal active
    while(1) {
      gripper(0);
    }
  }
}

// Function to move forward a specific distance (based on encoder pulses)
void moveDistance(int pulsesNeeded){

  // Save starting pulse count
  int start = pulsesRight;

  // Move until required number of pulses is reached
  while(pulsesRight < start + pulsesNeeded){

    moveForward(200);  
    gripper(0);   // keep servo signal alive during movement

  }
}

// Function to move forward
void moveForward(int speed){
  // Adjust speeds slightly for motor calibration
  analogWrite(RIGHTMOTORSTRAIGHT, speed - 65);
  analogWrite(LEFTMOTORSTRAIGHT, speed + 55);

  // Disable backward movement
  analogWrite(RIGHTMOTORBACK, 0);
  analogWrite(LEFTMOTORBACK, 0);
}

// Function to stop all motors
void stopMoving(){
  analogWrite(RIGHTMOTORSTRAIGHT, 0);
  analogWrite(LEFTMOTORSTRAIGHT, 0);
  analogWrite(RIGHTMOTORBACK, 0);
  analogWrite(LEFTMOTORBACK, 0);
}

// Interrupt function: counts left wheel pulses
void countLeft(){
  pulsesLeft++;
}

// Interrupt function: counts right wheel pulses
void countRight(){
  pulsesRight++;
}

// Function to hold gripper position for a specific time
void holdGripper(int pulse, int timeMs){

  unsigned long start = millis();

  // Keep sending servo signal for the specified duration
  while(millis() - start < timeMs){
    gripper(pulse);
  }
}

// Low-level servo control function (manual PWM generation)
void gripper(int newPulse){

  static unsigned long timer; // controls update timing
  static int pulse;           // stores current pulse width

  // Update every ~20 ms (standard servo refresh rate)
  if(millis() > timer){

    // If a new pulse value is provided, update it
    if(newPulse > 0){  
      pulse = newPulse;  
    }  

    // Generate PWM signal manually
    digitalWrite(SERVO, HIGH);  
    delayMicroseconds(pulse);  
    digitalWrite(SERVO, LOW);  

    // Schedule next update
    timer = millis() + 20;
  }
}