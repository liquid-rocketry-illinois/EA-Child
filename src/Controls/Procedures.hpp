#pragma once

#include "Motors.h"
#include "../Sensors/Sensors.h"
#include "../SDCard.h"
#include "ControlsSystem.h"
#include <Arduino.h>

#define DATA_WRITE_FREQUENCY 10 // Hertz

namespace Procedures{
    SDCard* DataCard; 
    Sensors* sensors;
    Servos* motors;
    Controls* Controller;

    void ___MAINSETUP(){
        // All pins default to an INPUT designation
        pinMode(31, OUTPUT); // PYRO ONE
        pinMode(32, OUTPUT); // PYRO TWO
        digitalWrite(31, LOW);
        digitalWrite(32, LOW); // Initialize as low signal to represent 'off'. 
                            // In case the write value isn't already low (this shouldn't happen though)

        Serial.begin(115200);
        //while (!Serial) {
        //    ; // wait for serial port to connect. remove line after testing
        //}
        

        if (!Wire.available()) Wire.begin();
        if (!Wire1.available()) Wire1.begin();
        if (!Wire2.available()) Wire2.begin();

        // Now construct the objects
        DataCard = new SDCard();
        sensors  = new Sensors;
        motors = new Servos;
        Controller = new Controls;

        DataCard->init();
        DataCard->SDWrite("SD Initialization.");
        if (!sensors->InitSensors()) Serial.println("One or multiple sensors failed!");
        else {
            Serial.println("All sensors initialized");
            Controller->Init(sensors);
        }
        motors->init();
        }

    void ___MAIN(){
        static unsigned long nextUpdateMicros = micros();
        const unsigned long interval = 1000000 / DATA_WRITE_FREQUENCY;

        unsigned long now = micros();
        if ((long)(now - nextUpdateMicros) >= 0) {
            nextUpdateMicros += interval;
            
            // ---- Sensor + Logging ----
            DataCard->SDWrite(sensors->Update()); // FILL IN WITH SENSORS' DATA
            DataCard->SDWrite(Controller->Update());

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
    }
};