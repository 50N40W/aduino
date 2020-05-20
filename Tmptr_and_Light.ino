#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <SimpleDHT.h>
#include <SPI.h>
#include <SD.h>

const int chipSelect = 4;
const int photoRPin = 0;
const int ledPin = LED_BUILTIN;
const int pinDHT11 = 2;
const long PHOTO_PERIOD = 200;
const long PERIOD = 1000;
const long DHT_PERIOD = 1500;
const float LIGHT_FC = 0.94;

int ledState = LOW;
unsigned long previousMillis = 0;
unsigned long prevDhtMillis = 0;
unsigned long photoMillis = 0;
int lightLevel = 500;
byte dht_tmptr = 0;
byte dht_humidity = 0;


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
  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    //while (1);
  }
}

void loop(void) 
{
  unsigned long currentMillis = millis();
  if (currentMillis - prevDhtMillis > DHT_PERIOD) {
    prevDhtMillis = currentMillis;
    int err = SimpleDHTErrSuccess;
    ledState = (ledState == LOW)? HIGH : LOW;
    digitalWrite(ledPin, ledState);
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
    String dataString = String(currentMillis) + ", ";
    dataString += String(dht_humidity) + ", " + String(dht_tmptr) + ", ";
    dataString += String(bmp.readTemperature()) + ", " + String(bmp.readPressure()) + ", ";
    dataString += String(lightLevel);

    Serial.print(dataString);

    File dataFile = SD.open("datalog.txt", FILE_WRITE);
    if (dataFile) {
      dataFile.println(dataString);
      dataFile.close();
      Serial.println("");
    }
    else {
      Serial.println(", unable to open file on SD card  ");
    }
  }
