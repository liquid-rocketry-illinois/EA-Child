#include "Controls/Procedures.h"

void Procedures::MAINSETUP(){
    // All pins default to an INPUT designation
    pinMode(31, OUTPUT); // PYRO
    digitalWrite(31, LOW);

    Serial.begin(115200);

    while (!Serial){;;}

    SPI.begin();
    if (!Wire.available()) Wire.begin();
    if (!Wire2.available()) Wire2.begin();

    DataCard->init();
    DataCard->SDWrite("SD Initialization.");

    if (!sensors->InitSensors()) Serial.println("One or multiple sensors failed!");
    else Serial.println("All sensors initialized");

    Controller->Init(sensors);
    motors->init();

    initial_height = sensors->MSSensor.Data.getMSData().getX();
}

void Procedures::DRIVE_CONTROLS(){
    static unsigned long nextUpdateMicros = micros();
    const unsigned long interval = 1000000 / DATA_WRITE_FREQUENCY;

    unsigned long now = micros();
    if ((long)(now - nextUpdateMicros) >= 0) {
        nextUpdateMicros += interval;
        
        // ---- Sensor + Logging ----
        //DataCard->SDWrite(sensors->Update()); // FILL IN WITH SENSORS' DATA
        //DataCard->SDWrite(Controller->Update());
        sensors->Update();

        // ---- Frequency Monitor ----
        static unsigned long lastFreqPrint = millis();
        static unsigned int loopCount = 0;
        loopCount++;

        if (millis() - lastFreqPrint >= 1000) {
            float measuredHz = loopCount * 1000.0 / (millis() - lastFreqPrint);

            //Serial.print("Target: 10 Hz, Measured: ");
            //Serial.print(measuredHz, 2);
            //Serial.println(" Hz");

            loopCount = 0;
            lastFreqPrint = millis();
        }

        // ---- Catch-up handling ----
        // If we're very late, resync nextUpdateMicros to 'now'
        if ((long)(now - nextUpdateMicros) > (long)interval) {
            nextUpdateMicros = now + interval;
            //Serial.println("Loop lagged! Resyncing schedule.");
        }

    } else {
        // sleep until the next update window
        delayMicroseconds((unsigned long)(nextUpdateMicros - now));
    }
}

bool Procedures::EJECTION(){
    static bool aboveTargetH = false;
    static uint32_t prevmillis = 0;

    double height = sensors->MSSensor.Data.getMSData().getX() - initial_height;

    if (height > 800) aboveTargetH = true;

    if (aboveTargetH){
        if (false){ // to be replaced with velocity calc
            digitalWrite(31, HIGH);
            delay(5000);
            digitalWrite(31, LOW);
            return true;
        }
    }
    uint32_t now = millis();
    if (now - prevmillis >= 500){
        double prevheight = height;
        prevmillis = now;
    }
    return false;
}