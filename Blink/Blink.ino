const int BUTTONPIN = 2;        // First button pin
const int SECONDBUTTON = 3;     // Second button pin
const int LEDPIN = 13;          // LED pin

int buttonState = 0;            // State of first button
int secondButtonState = 0;      // State of second button
unsigned long previousTime = 0; // Stores last toggle time
bool isFlash = true;            // LED state flag (ON/OFF)

void setup() {
  pinMode(LEDPIN, OUTPUT);         
  pinMode(BUTTONPIN, INPUT);       
  pinMode(SECONDBUTTON, INPUT);    
  digitalWrite(LEDPIN, HIGH);      
}

void loop() {
  buttonState = digitalRead(BUTTONPIN);        // Read first button
  secondButtonState = digitalRead(SECONDBUTTON); // Read second button
  unsigned long currenTime = millis();         // Get current time
  int interval = 1000;                         // Default blinking interval (1 second)
  
  // Determine interval based on button states
  if(buttonState == HIGH && secondButtonState == HIGH) {
    interval = 1000;   // Both not pressed normal speed
  } else if(buttonState == LOW){
    interval = 500;    // First button pressed faster blinking
  } else if(secondButtonState == LOW){
    interval = 2000;   // Second button pressed slower blinking
  }

  // Check if it's time to toggle LED
  if (currenTime - previousTime >= interval) {
    previousTime = millis(); // Update last toggle time

    // Toggle LED state
    if(isFlash){
      isFlash = false;
      digitalWrite(LEDPIN, LOW);   // Turn LED OFF
    } else {
      isFlash = true;
      digitalWrite(LEDPIN, HIGH);  // Turn LED ON
    }
  }
}
