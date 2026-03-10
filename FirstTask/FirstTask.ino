const int BUTTONPIN = 2;
const int SECONDBUTTON = 3;
const int LEDPIN = 13;

int buttonState = 0;
int secondButtonState = 0;
unsigned long previousTime = 0;
bool isFlash = true;

void setup() {
  // put your setup code here, to run once:
  pinMode(LEDPIN, OUTPUT);
  pinMode(BUTTONPIN, INPUT);
  pinMode(SECONDBUTTON, INPUT);
  digitalWrite(LEDPIN, HIGH);

}

void loop() {
  // put your main code here, to run repeatedly:
  buttonState = digitalRead(BUTTONPIN);
  secondButtonState = digitalRead(SECONDBUTTON);
  unsigned long currenTime = millis();
  int interval = 1000;
  
    if(buttonState == HIGH && secondButtonState == HIGH) {
      interval = 1000; 
      }else if(buttonState == LOW){
        interval = 500;
        }else if(secondButtonState == LOW){
          interval = 2000;
          }
          if (currenTime - previousTime >=interval) {
            previousTime = millis();
            if(isFlash){
              isFlash = false;
              digitalWrite(LEDPIN, LOW);
            }else{
              isFlash = true;
              digitalWrite(LEDPIN, HIGH);
            }
          }
   
}
