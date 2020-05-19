#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <SimpleDHT.h>

const int photoRPin = 0;
const int ledPin = LED_BUILTIN;
const int pinDHT11 = 2;
const long PHOTO_PERIOD = 20;
const long PERIOD = 500;
const long DHT_PERIOD = 1500;
const float LIGHT_FC = 0.94;

int ledState = LOW;
unsigned long previousMillis = 0;
unsigned long prevDhtMillis = 0;
unsigned long photoMillis = 0;
int lightLevel = 500;


Adafruit_BMP085 bmp;   
SimpleDHT11 dht11(pinDHT11);

void setup(void) 
{
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
  Serial.println("BMP, Photocell & DHT 11 Sensor experiment"); Serial.println("");
  
  /* Initialise the sensor */
  if(!bmp.begin())
  {
    Serial.print("BMP 180 not detected");
    while(1);
  }
}

void loop(void) 
{
  byte dht_tmptr = 0;
  byte dht_humidity = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - prevDhtMillis > DHT_PERIOD) {
    prevDhtMillis = currentMillis;
    int err = SimpleDHTErrSuccess;
    if ((err = dht11.read(&dht_tmptr, &dht_humidity, NULL)) != SimpleDHTErrSuccess) {
      Serial.print("Read DHT11 failed, err="); 
      Serial.println(err);
    }
  }
  if (currentMillis - photoMillis >= PHOTO_PERIOD) {
    photoMillis = currentMillis;
    int rawLightLevel = analogRead(photoRPin);
    lightLevel = (int)(LIGHT_FC * (float)lightLevel + (1-LIGHT_FC)*(rawLightLevel));
  }
  if (currentMillis - previousMillis >= PERIOD) {
    previousMillis = currentMillis;
    digitalWrite(ledPin, ledState);
    ledState = (ledState == LOW)? HIGH : LOW;

    Serial.print(currentMillis);
    Serial.print(", ");
    Serial.print(bmp.readTemperature());
    Serial.print(", ");
    Serial.print(dht_tmptr);
    Serial.print(", ");
    Serial.print(lightLevel);    
    Serial.print(", ");
    Serial.println(bmp.readPressure());
  }
}
