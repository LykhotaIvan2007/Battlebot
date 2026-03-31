// Pins controlling the left motor (backward / forward)
const int LEFTMOTORBACK = 8;
const int LEFTMOTORSTRAIGHT = 9;

// Pins controlling the right motor (backward / forward)
const int RIGHTMOTORBACK = 12;
const int RIGHTMOTORSTRAIGHT = 13; 

// Array of 8 line sensor pins (analog inputs)
const int sensorPins[8] = {A0, A1, A2, A3, A4, A5, A6, A7};

// Weight values for each sensor (used to calculate line position)
int weights[8] = {-350,-250,-150,-50,50,150,250,350};

// Proportional coefficient for line correction
float Kp = 0.35;

// Threshold to detect the line (values below are ignored)
int threshold = 500;

// Stores last known line position (used if line is lost)
int lastValue = 0;

void setup() {
  // Set motor control pins as outputs
  pinMode(LEFTMOTORBACK, OUTPUT);
  pinMode(LEFTMOTORSTRAIGHT, OUTPUT);
  pinMode(RIGHTMOTORBACK, OUTPUT);
  pinMode(RIGHTMOTORSTRAIGHT, OUTPUT);

  // Set all sensor pins as inputs
  for (int i = 0; i < 8; i++) pinMode(sensorPins[i], INPUT);

  // Ensure robot is stopped at startup
  stopMoving();
}

void loop() {
  // Read current line position
  int value = readLine();

  // If line is lost (special value returned)
  if (value == 10000) {

    // Decide spin direction based on last known position
    int spin = (lastValue >= 0) ? 120 : -120; 
  
    // Adjust motor speeds to search for the line
    int left  = constrain(140 - spin, 0, 255);
    int right = constrain(125 + spin, 0, 255);
  
    // Apply motor speeds (forward only)
    analogWrite(LEFTMOTORSTRAIGHT, left);
    digitalWrite(LEFTMOTORBACK, 0);
    analogWrite(RIGHTMOTORSTRAIGHT, right);
    digitalWrite(RIGHTMOTORBACK, 0);
  
    delay(20); // small delay for stability
    return;
  }

  // Save current position as last known position
  lastValue = value;

  // Calculate correction using proportional control 
  int correction = (int)(value * Kp);

  // Adjust motor speeds based on correction
  int leftSpeed  = 140 - correction;
  int rightSpeed = 135 + correction;

  // Limit speeds
  leftSpeed  = constrain(leftSpeed, 0, 255);
  rightSpeed = constrain(rightSpeed, 0, 255);
  
  // Apply speeds to motors (forward movement)
  analogWrite(LEFTMOTORSTRAIGHT, leftSpeed);
  digitalWrite(LEFTMOTORBACK, 0);
  analogWrite(RIGHTMOTORSTRAIGHT, rightSpeed);
  digitalWrite(RIGHTMOTORBACK, 0);

  delay(10); // small delay for smoother control
}

// Function to stop all motors
void stopMoving() {
  analogWrite(RIGHTMOTORSTRAIGHT, 0);
  analogWrite(LEFTMOTORSTRAIGHT, 0);
  analogWrite(RIGHTMOTORBACK, 0);
  analogWrite(LEFTMOTORBACK, 0);
}

// Function to calculate line position using weighted sensors
int readLine() {
  long sum = 0;     // weighted sum of sensor values
  long total = 0;   // total signal strength

  for (int i = 0; i < 8; i++) {
    // Read analog value from sensor
    int sensorValue = analogRead(sensorPins[i]);

    // Remove noise using threshold
    int howLineIsVisible = sensorValue - threshold;
    if (howLineIsVisible < 0) howLineIsVisible = 0;

    // Add weighted contribution
    sum += (long) howLineIsVisible * weights[i];
    total += howLineIsVisible;
  }

  // If line is not detected (very low signal)
  if (total < 80) return 10000;

  // Return average weighted position of the line
  return (int)(sum / total);
}