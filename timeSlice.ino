#include "pitches.h"

/*  
  This example code is in the public domain.

  Created 17 Aug 2017
  by Howard Bishop
*/
int thisNote = 0;
const int LED1 = 13;
//const int LED2 = 8;
unsigned long prevF1;
unsigned long prevF2;
const unsigned long F1 = 2000;
unsigned long F2 = 5000;
int L1State = LOW;
int F2State = 0;
//const OFFSET1 = 20;
//const OFFSET2 = 25;

// notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

/*****************   SETUP ***************/
void setup() {
  prevF1 = millis() + 20;
  //prevF2 = millis() + 15;
  
  pinMode(LED1, OUTPUT);
  //pinMode(LED2, OUTPUT);
}

/******************END SETUP*************/

// the loop function runs over and over again forever
void loop() {

  unsigned long thisFrame = millis();
  if ((thisFrame - prevF1) >= F1) {
    // frame1, period: F1
    prevF1 = thisFrame;
    L1State = (L1State + 1) & 0x01;
    digitalWrite(LED1, L1State);   
  }
  
  if ((thisFrame - prevF2) >= F2) {
    // frame2, period: F2
    prevF2 = thisFrame;
    F2State = (F2State + 1) & 0x01;
    if (F2State == 0) {
      if (thisNote == 0) {
        F2 = 2000;
      }
      else {
        F2 = 1000 / noteDurations[thisNote];
      }
      noTone(8);       
    }
    else
    {
    // iterate over the notes of the melody:
      F2 = 1000 / noteDurations[thisNote];
      tone(8, melody[thisNote], noteDurations);
      thisNote++;
      if (thisNote >= 8) {
        thisNote = 0;
      }
    }
  }
