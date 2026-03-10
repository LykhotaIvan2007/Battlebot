const int ledPinRed = 13;
const int ledPinYellow = 12;
const int ledPinGreen = 11;
const int buttonPin = 2;

int buttonState = 0;
bool isButtonClicked = false;
unsigned long previousTime = 0;
int state = 0;  //0 =red 1=green 2=yellow

void setup() {
  // put your setup code here, to run once:
  pinMode(ledPinRed, OUTPUT);
  pinMode(ledPinYellow, OUTPUT);
  pinMode(ledPinGreen, OUTPUT);
  pinMode(buttonPin,INPUT);

  digitalWrite(ledPinRed, LOW);
  digitalWrite(ledPinYellow,HIGH);
  digitalWrite(ledPinGreen,HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  buttonState = digitalRead(buttonPin);
  if(buttonState == LOW && !isButtonClicked){
    state = 0;
    previousTime = millis();
    isButtonClicked = true;
    }

    if(digitalRead(buttonPin == LOW){
      unsigned long currentTime = millis();

      if(state == 0 && currentTime - previousTime >= 1000){
        state = 1;
        previousTime = currentTime;
        digitalWrite(ledPinRed, HIGH);
        digitalWrite(ledPinYellow,HIGH);
        digitalWrite(ledPinGreen,LOW);
      }
      
      if(state == 1 && currentTime - previousTime >= 3000){
        state = 2;
        previousTime = currentTime;
        digitalWrite(ledPinRed, HIGH);
        digitalWrite(ledPinYellow,LOW);
        digitalWrite(ledPinGreen,HIGH);
      }
      if(state == 2 && currentTime - previousTime >= 1000){
        isButtonClicked = false;
        digitalWrite(ledPinRed, LOW);
        digitalWrite(ledPinYellow, HIGH);
        digitalWrite(ledPinGreen, HIGH);
      }     
    }
  
}
