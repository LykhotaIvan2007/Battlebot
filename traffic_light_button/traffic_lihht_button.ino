const int ledPinRed = 13;      // Pin for red LED
const int ledPinYellow = 12;   // Pin for yellow LED
const int ledPinGreen = 11;    // Pin for green LED
const int buttonPin = 2;       // Pin for button

int buttonState = 0;           // Variable to store button state
bool isButtonClicked = false;  // Flag to track if button was clicked
unsigned long previousTime = 0; // Stores last time reference
int state = 0;  // 0 = red, 1 = green, 2 = yellow

void setup() {
  pinMode(ledPinRed, OUTPUT);     
  pinMode(ledPinYellow, OUTPUT);  
  pinMode(ledPinGreen, OUTPUT);   
  pinMode(buttonPin, INPUT);      

  digitalWrite(ledPinRed, LOW);      
  digitalWrite(ledPinYellow, HIGH);  
  digitalWrite(ledPinGreen, HIGH);   
}

void loop() {
  buttonState = digitalRead(buttonPin); // Read button state

  // Check if button is pressed (LOW) and not already handled
  if(buttonState == LOW && !isButtonClicked){
    state = 0;                     // Reset to initial state (red)
    previousTime = millis();       // Store current time
    isButtonClicked = true;        // Mark button as handled
  }

  // Continue sequence while button is held
  if(digitalRead(buttonPin)== LOW){
    unsigned long currentTime = millis(); // Get current time

    // After 1 second in state 0  switch to state 1 (green)
    if(state == 0 && currentTime - previousTime >= 1000){
      state = 1;
      previousTime = currentTime;
      digitalWrite(ledPinRed, HIGH);     // Red OFF (assuming active LOW logic)
      digitalWrite(ledPinYellow, HIGH);  // Yellow OFF
      digitalWrite(ledPinGreen, LOW);    // Green ON
    }

    // After 3 seconds in state 1  switch to state 2 (yellow)
    if(state == 1 && currentTime - previousTime >= 3000){
      state = 2;
      previousTime = currentTime;
      digitalWrite(ledPinRed, HIGH);     // Red OFF
      digitalWrite(ledPinYellow, LOW);   // Yellow ON
      digitalWrite(ledPinGreen, HIGH);   // Green OFF
    }

    // After 1 second in state 2  reset system
    if(state == 2 && currentTime - previousTime >= 1000){
      isButtonClicked = false;           // Allow button to be pressed again
      digitalWrite(ledPinRed, LOW);      // Red ON
      digitalWrite(ledPinYellow, HIGH);  // Yellow OFF
      digitalWrite(ledPinGreen, HIGH);   // Green OFF
    }     
  }
}
