/*
  SD card datalogger

  This example shows how to log data from three analog sensors
  to an SD card using the SD library.

  The circuit:
   analog sensors on analog ins 0, 1, and 2
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

  created  24 Nov 2010
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

*/

// These are the four libraries for the four utilized sensors
#include <MS5611.h>
#include <SensirionI2cSht4x.h>

#include "SDCard.h"
#include "Sensors\Sensors.h"

SDCard* DataCard; 
Sensors* sensors;

void setup() {
    Serial.begin(115200);
    while (!Serial) {
        ; // wait for serial port to connect. remove line when testing
    }

    if (!Wire.available()) Wire.begin();
    if (!Wire1.available()) Wire1.begin();
    if (!Wire2.available()) Wire2.begin();

    // Now construct the objects
    DataCard = new SDCard();
    sensors  = new Sensors();

    DataCard->SDWrite("SD Initialization.");
    if (!sensors->InitSensors()) Serial.println("One or multiple sensors failed!");
    else Serial.println("All sensors initialized");

    // All pins default to an INPUT designation
    pinMode(31, OUTPUT); // PYRO ONE
    pinMode(32, OUTPUT); // PYRO TWO
    digitalWrite(31, LOW);
    digitalWrite(32, LOW); //Initialize as low signal to represent 'off'. In case the write value isn't already low (this shouldn't happen though)
}

void loop() {
    sensors->Update();
    //DataCard->SDWrite(); // FILL IN WITH SENSORS' DATA

    /*
    //EJECTION TEST
    Serial.println("BEGIN EJECTION TEST.");
    delay(20000);
    Serial.println("ON!");
    digitalWrite(31, LOW);
    digitalWrite(32, LOW);
    delay(15000);
    Serial.println("OFF!");
    digitalWrite(31, HIGH);
    digitalWrite(32, HIGH);
    while(1){;;}
    */
}
