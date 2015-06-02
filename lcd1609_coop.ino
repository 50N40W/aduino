/*
  -- In Progress - Not Working --
  Read current in amps and display work
  This involves time and voltage to calculate
  Watts.
  Project for stationary cycle at bike coop
Pins used:
0                       7    IN reset switch
1                        8   Out Test Lamp
2  Out  LCD    9
3  Out  LCD    10
4  Out  LCD    11  Out LCD
5   Out LCD    12  Out LCD
6                       13  Out Warning LED

A0 IN  Current Sense
A1 IN Voltage Sense
A2
A3
A4
A5


  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 
 by Don Fye and Howard Bishop
 
 This example code is in the public domain.
Borrowing heavily from previous work by others:
 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 */

// include the library code:
#include <LiquidCrystal.h>
#define CON_WARNLAMP_TIME  20
//#define CON_HIGH_CURRENT   35
#define CON_HIGH_CURRENT       35
#define CON_MAX_LAMP_TIME    1000
#define CON_MAX_WATTS       10000
unsigned long time = 0;
unsigned long last_time = 0;
//int testlamp = 8;
int ledwarn = 13;
int switchPin = 7;
int ledwarncount =10;
int outputWS = 0;
int buttonState;
int last_reset_switch = LOW;
long last_debouncetime = 0;
long bounceTime = 100;
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

/* --------------------------------------------------*/
//       SETUP
/* --------------------------------------------------*/
void setup() {
  // set up the LCD's number of columns and rows: 
  lcd.begin(20, 4);
  Serial.begin(9600);
  lcd.clear();
  // Print a message to the LCD.
  //lcd.print("hello, world!");
 // pinMode(testlamp, OUTPUT);
  pinMode(ledwarn, OUTPUT);
  pinMode(switchPin, INPUT);
}

/* --------------------------------------------------*/
//       Get Reset Switch
/* --------------------------------------------------*/
// Get and debounce reset switch
int get_reset_switch() {
  int reset_switch = digitalRead(switchPin);
  if (reset_switch != last_reset_switch) {
      last_debouncetime = millis();
  }
  else if ((millis() - last_debouncetime) > bounceTime) {
    last_reset_switch = reset_switch;
  }
   return last_reset_switch; 
}

/* --------------------------------------------------*/
//       Get Current
/* --------------------------------------------------*/
// Get the input current
float get_current(int reset_asserted)  {
  float outputAmp;
  // read the current and volt pin and linearize it.
  if (reset_asserted == HIGH)   {
     outputAmp = 0;
  }
 else { 
     int senseAmp = analogRead(A0);
     outputAmp = (float)(((long)523-senseAmp)*.32);
 }
 return outputAmp;
}

/* --------------------------------------------------*/
//       Set Warning Lamp
/* --------------------------------------------------*/
int set_warn_lamp (int reset_asserted, float outputAmp) {
  // drive a lamp if high.1
  digitalWrite(ledwarn,HIGH);  
  //if (0 ==2)
 // {
     if (outputAmp > CON_HIGH_CURRENT) {
       if (ledwarncount < CON_MAX_LAMP_TIME){
          ledwarncount += CON_WARNLAMP_TIME;
       }
  }
  if ((ledwarncount < CON_WARNLAMP_TIME) || (reset_asserted == HIGH)){
     digitalWrite(ledwarn, LOW);
    ledwarncount = 0;
  }
  else {
    digitalWrite(ledwarn,HIGH);  
    ledwarncount--;
  }
 // }
  return 0;
}

/* --------------------------------------------------*/
//       Get Volts
/* --------------------------------------------------*/
float get_volts(int reset_asserted) {
   float outputVolts;
   if (reset_asserted == HIGH) {
     outputVolts = 0;
   }
   else {  
      int senseVolts = analogRead(A1);
      float voltage = ((float)senseVolts) * (5.0 / 1023.0);
      float outputVolts = voltage*71.5;
   }
   return outputVolts;
}


/* --------------------------------------------------*/
//       Calculate Watts
/* --------------------------------------------------*/
float calc_watts(int reset_asserted, float current, float voltage) {
  float watts_out;
  watts_out = current * voltage;
  if ((watts_out < 0) || (reset_asserted == HIGH)) {
    watts_out = 0;
  }
  else if (watts_out > CON_MAX_WATTS) {
    watts_out = CON_MAX_WATTS;
  }
  return watts_out;
}


/* --------------------------------------------------*/
//       Time Check
/* --------------------------------------------------*/
unsigned long check_time(int reset_asserted)  {
  unsigned long outseconds = 0;
  outseconds = last_time;
  last_time = millis();
  if (!reset_asserted) {
    outseconds = last_time;
    time = abs(last_time - outseconds);
  }
  else {
    time = 0;
 } 
   return time;
}


/* --------------------------------------------------*/
//       Make it pretty and make it 
//                DISPLAYed
/* --------------------------------------------------*/
int make_output(int otimer, float outputAmp, float outputVolts, float watt)  {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):

  lcd.setCursor(0,0);
  lcd.print ("Current ");
  lcd.setCursor (8,0);
  lcd.print (outputAmp);
  //float watt = outputVolts*outputAmp;
  lcd.setCursor (0,3);
  lcd.print ("watts");
  lcd.setCursor (7,3);
  lcd.print (watt);
  outputWS = (((long)watt+outputWS));
  lcd.setCursor (15,3);
  lcd.print (outputWS);
  
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  lcd.print(millis()/1000);
   return 0;
}


/* --------------------------------------------------*/
//   Home is where the plate is.
/* --------------------------------------------------*/
void loop() {
  float current;
  float watts;
  float volts;
  long timer;
  int status1;
  int reset_asserted;
  reset_asserted = get_reset_switch();
  volts = get_volts(reset_asserted);
  current  = get_current(reset_asserted);
  current = 200;
  reset_asserted = 0;
//digitalWrite(8,HIGH);
//  digitalWrite(ledwarn,HIGH);  
  status1 = set_warn_lamp(reset_asserted, current);
  timer = check_time(reset_asserted);
  watts = calc_watts(reset_asserted, current, volts);
  status1 = make_output( timer, current, volts, watts);
  delay(50);
}
