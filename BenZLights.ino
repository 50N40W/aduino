/***************************************************************************/
// Copyright 2019, Howard Bishop, Gnu Lesser General Public License as listed
//  below.
// Using 2 LEDs and a SEEEDStudio ultrasonic distance sensor, it makes a pretend
// EGR and CEL light.
//  The original intent is to do a power on self test (POST), followed by a blinking EGR if the
//  distance sensor picks up a target within specified cm.  At that point the EGR Blinks for a time
// then it goes steady and the CEL light blinks.
// Finally, there's 20 seconds or so of dead time before the process starts over.
// I believe we've covered the case for the 50 day rollover of the internal milliseconds timer
// And while the odds of someone detecting a slight variation in lamp blink rate for one event
// every two months is small, I'm not entirely happy with it.
//
// BTW, the ultrasonic sensor goes to pin 7 the others are easier to find in code
//    Running the ultrasonnic on 3.3v if that helps.  Seems to work OK up to about 600cm.
// Includes source from the Radio Shack Distance sensor,
// https://github.com/RadioShackCorp/2760342-Range-Sensor/blob/master/UltrasonicRangeSensor.ino
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
//
/*****************************************************************************/
#include "Arduino.h"
#define POWERUP 0 // power on in a known default state
#define POST 1    // do a Power On Self-Test                                                                     
#define WAITING 2 // checks distance every PERIOD[2] milliseconds                                                
#define EGR 3     // will flash EGR lamp (depends on LAMPS below)                                                
#define CEL 4     // EGR Steady, Flash CEL (depends on LAMPS below)                                              
#define QUIET 5   // Give a little dead time before restart (per LAMPS ...)                                      
#define STEADY 0x01                                                                                              
#define BLINKING 0x02                                                                                            
#define IN_RANGE 100                                                                                             
#define LAMP_PERIOD 900                                                                                          
                                                                                                                 
// E lamp requires 0 bits of shift (low nibble), C lamp needs 4 (high nibble)
#define E_SHIFT 0
#define C_SHIFT 4

//                      PWR  POST  WAIT  EGR    CEL    QUIET
const long PERIOD[] = {1000, 5000,  100, 15000, 15000, 20000};
// let's say 1 is steady, 2 is blinking.
// Low nibble is EGR, upper nibble is CEL
const byte LAMPS[] =  {0x00, 0x11, 0x00, 0x02, 0x21,  0x00};

const int CEL_Pin = 13;
const int EGR_Pin = 12;
int CEL_Lamp = LOW;
int EGR_Lamp = LOW;
unsigned long prevLampTime = 0;
unsigned long prevStateTime = 0;
byte lampCommand;
byte currentState;

class Ultrasonic
{
  public:
    Ultrasonic(int pin);
    void DistanceMeasure(void);
    long microsecondsToCentimeters(void);
    long microsecondsToInches(void);
  private:
    int _pin;//pin number of Arduino that is connected with SIG pin of Ultrasonic Ranger.
    long duration;// the Pulse time received;
};
Ultrasonic::Ultrasonic(int pin)
{
  _pin = pin;
}
/*Begin the detection and get the pulse back signal*/
void Ultrasonic::DistanceMeasure(void)
{
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(_pin, HIGH);
  delayMicroseconds(5);
  digitalWrite(_pin, LOW);
  pinMode(_pin, INPUT);
  duration = pulseIn(_pin, HIGH);
}
/*The measured distance from the range 0 to 400 Centimeters*/
long Ultrasonic::microsecondsToCentimeters(void)
{
  return duration / 29 / 2;
}

Ultrasonic ultrasonic(7);
void setup()
{
  pinMode(CEL_Pin, OUTPUT);
  pinMode(EGR_Pin, OUTPUT);
  currentState = POWERUP;
  Serial.begin(9600);
}

int updateLamps(byte blinkMode, int currLamp) {
  int lampCommand;
  if (blinkMode == BLINKING) {
    lampCommand = (currLamp == LOW) ? HIGH : LOW;
  }
  else {
    lampCommand = (blinkMode & STEADY) ? HIGH : LOW;
  }
  return lampCommand;
}

void loop()
{
  unsigned long currentMillis = millis();
  // the second part of this "if" handles the 50 day overflow. Crudely.
  if ((currentMillis - prevStateTime > PERIOD[currentState]) ||
      (currentMillis < prevStateTime)) {
    prevStateTime = currentMillis;
    if (currentState == WAITING) {
      long RangeInCentimeters;
      ultrasonic.DistanceMeasure();// get the current signal time;
      RangeInCentimeters = ultrasonic.microsecondsToCentimeters();
      Serial.print(RangeInCentimeters);
      Serial.println(" ");
      if (RangeInCentimeters < IN_RANGE) {
        currentState++;
      }
    }
    else {
      if (currentState == QUIET) {
        currentState = WAITING;
      }
      else {
        currentState++;
      }
    }
    Serial.print("Current State: ");
    Serial.println(currentState);
  }
  if ((currentMillis - prevLampTime > LAMP_PERIOD) || 
      (currentMillis < prevLampTime)) {
    prevLampTime = currentMillis;
    CEL_Lamp = updateLamps(LAMPS[currentState] >> C_SHIFT, CEL_Lamp);
    EGR_Lamp = updateLamps(LAMPS[currentState] >> E_SHIFT, EGR_Lamp);
    digitalWrite(CEL_Pin, CEL_Lamp);
    digitalWrite(EGR_Pin, EGR_Lamp);
  }
}
