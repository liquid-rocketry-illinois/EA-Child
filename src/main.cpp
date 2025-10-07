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
#include "Controls\Motors.h"

#define DATA_WRITE_FREQUENCY 100 // Hertz

SDCard* DataCard; 
Sensors* sensors;
Servos* motors;

void setup() {
    // All pins default to an INPUT designation
    pinMode(31, OUTPUT); // PYRO ONE
    pinMode(32, OUTPUT); // PYRO TWO
    digitalWrite(31, LOW);
    digitalWrite(32, LOW); // Initialize as low signal to represent 'off'. 
                           // In case the write value isn't already low (this shouldn't happen though)
    Serial.begin(115200);
    while (!Serial) {
        ; // wait for serial port to connect. remove line after testing
    }

    if (!Wire.available()) Wire.begin();
    if (!Wire1.available()) Wire1.begin();
    if (!Wire2.available()) Wire2.begin();

    // Now construct the objects
    DataCard = new SDCard();
    sensors  = new Sensors;
    motors = new Servos;

    DataCard->init();
    DataCard->SDWrite("SD Initialization.");
    if (!sensors->InitSensors()) Serial.println("One or multiple sensors failed!");
    else Serial.println("All sensors initialized");
    motors->init();
}

void loop() {
    static unsigned long nextUpdateMicros = micros();
    const unsigned long interval = 1000000 / DATA_WRITE_FREQUENCY;
    int loops = 0;
    sensors->Update();
    
    //DataCard->SDWrite(); // FILL IN WITH SENSORS' DATA

    unsigned long now = micros();
    if ((long)(now - nextUpdateMicros) >= 0) {
        nextUpdateMicros += interval;  // lock to 100 Hz schedule

        // ---- Sensor + Logging ----
        motors->testMotors();
        DataCard->SDWrite(sensors->Update()); // FILL IN WITH SENSORS' DATA
        if (loops >= 100000){
            while(1){;} // Stop loop
        }
/*
        // ---- Frequency Monitor ----
        static unsigned long lastFreqPrint = millis();
        static unsigned int loopCount = 0;
        loopCount++;

        if (millis() - lastFreqPrint >= 1000) {
            float measuredHz = loopCount * 1000.0 / (millis() - lastFreqPrint);

            Serial.print("Target: 10 Hz, Measured: ");
            Serial.print(measuredHz, 2);
            Serial.println(" Hz");

            loopCount = 0;
            lastFreqPrint = millis();
        }

        // ---- Catch-up handling ----
        // If we're very late, resync nextUpdateMicros to 'now'
        if ((long)(now - nextUpdateMicros) > (long)interval) {
            nextUpdateMicros = now + interval;
            Serial.println("Loop lagged! Resyncing schedule.");
        }
*/
    } else {
        // sleep until the next update window
        delayMicroseconds((unsigned long)(nextUpdateMicros - now));
    }
    loops++;
}
