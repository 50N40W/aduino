/*
  Change the pwm period to one of three different patterns
  with the transition at a specific time.
 This example code is in the public domain.

 Developed as an extension of the fade example.

 */
const int LOWPWM = 3;
const int MIDPWM = 127;
const int HIPWM = 245;
unsigned long prevTimeMillis = 0;
const long TRANSITION_1 = 5000;
const long TRANSITION_2 = 10000;
int fadeValue = 3;
int ledPin = 9;    // LED connected to digital pin 9

void setup() {
    // set it up for the first case
    prevTimeMillis = millis();
    analogWrite(ledPin, fadeValue);
}

void loop() {
  unsigned long currTimeMillis = millis();
  if ((currTimeMillis - prevTimeMillis > TRANSITION_2)&&
      (fadeValue < HIPWM)){
      fadeValue = HIPWM;
      analogWrite(ledPin, fadeValue);
  }
  else if ((currTimeMillis - prevTimeMillis > TRANSITION_1) &&
           (fadeValue < MIDPWM)) {
      fadeValue = MIDPWM;
      analogWrite(ledPin, fadeValue); 
  }
}

