#include "Controls/Procedures.h"

void Procedures::MAINSETUP(){
    // All pins default to an INPUT designation
    pinMode(31, OUTPUT); // PYRO
    digitalWrite(31, LOW);
#ifdef MODE_TESTING
    Serial.begin(115200);
    while (!Serial){;;}
#endif
    SPI.begin();
    if (!Wire.available()) Wire.begin();
    if (!Wire2.available()) Wire2.begin();

    DataCard->init();
    DataCard->SDWrite("SD Initialization.\n");

    if (!sensors->InitSensors()){ DataCard->SDWrite("One or multiple sensors failed!\n");
#ifdef MODE_TESTING
    Serial.println("One or multiple sensors failed!");
#endif
    }
#ifdef MODE_TESTING
    else Serial.println("All sensors initialized");
#endif
    Controller->Init(sensors);
    motors->init();

    const uint32_t init_millis = millis();
    while(millis() - init_millis < 180000){ // Delay for calibration : 3min
        sensors->Update();
#ifdef MODE_TESTING
        Serial.println("Calibrating...");
#endif
    }

    // Initial positions. Tared positions 
    sensors->IMUs.Data.pyyr_init = sensors->IMUs.Data.pyyr_o;
    sensors->IMUs.Data.omega_init = sensors->IMUs.Data.omega_o;
    sensors->IMUs.Data.alpha_init = sensors->IMUs.Data.alpha_o;
    sensors->IMUs.Data.U_init = sensors->IMUs.Data.U_o;
    sensors->IMUs.Data.A_init = sensors->IMUs.Data.A_o;
    sensors->IMUs.Data.Magnet_init = sensors->IMUs.Data.Magnet_o;
    sensors->IMUs.Data.quats_init = sensors->IMUs.Data.quats_o;

    initial_height = sensors->MSSensor.Data.getMSData().getX();
    DataCard->SDWrite(" Initial Height: ");
    DataCard->SDWrite((String)initial_height);

    

    char buffer[540];
    snprintf(buffer, sizeof(buffer),
        "||%.3f,%.3f,%.3f,%.3f|%.3f,%.3f,%.3f|%.3f,%.3f,%.3f|%.3f,%.3f,%.3f|%.3f,%.3f,%.3f|%.3f,%.3f,%.3f||",
        sensors->IMUs.Data.quats_init.i,sensors->IMUs.Data.quats_init.j,sensors->IMUs.Data.quats_init.k,sensors->IMUs.Data.quats_init.r,
        sensors->IMUs.Data.Magnet_init.getX(), sensors->IMUs.Data.Magnet_init.getY(), sensors->IMUs.Data.Magnet_init.getZ(),
        sensors->IMUs.Data.pyyr_init.getX(), sensors->IMUs.Data.pyyr_init.getY(), sensors->IMUs.Data.pyyr_init.getZ(),
        sensors->IMUs.Data.A_init.getX(), sensors->IMUs.Data.A_init.getY(), sensors->IMUs.Data.A_init.getZ(),
        sensors->IMUs.Data.omega_init.getX(), sensors->IMUs.Data.omega_init.getY(), sensors->IMUs.Data.omega_init.getZ(),
        sensors->IMUs.Data.alpha_init.getX(), sensors->IMUs.Data.alpha_init.getY(), sensors->IMUs.Data.alpha_init.getZ()
    );

    DataCard->SDWrite(" Initial IMU data: ");
    DataCard->SDWrite((String)buffer);

#ifdef MODE_TESTING
    Serial.print("\nInitial IMU data: ");
    Serial.println((String)buffer);
    Serial.print("Initial Height: ");
    Serial.println(initial_height);
#endif
}

void Procedures::DRIVE_CONTROLS(){
    static unsigned long nextUpdateMicros = micros();
    const unsigned long interval = 1000000 / DATA_WRITE_FREQUENCY;

    unsigned long now = micros();
    if ((long)(now - nextUpdateMicros) >= 0) {
        nextUpdateMicros += interval;
        
        // ---- Sensor + Logging ----
        DataCard->SDWrite(sensors->Update_Tared(initial_height));
        DataCard->SDWrite(Controller->Update());

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
    static uint32_t prevmillis = millis();
    static uint32_t prevtime = millis();
    static double prevHeight = 0;
    static float velocity = 0;

    double height = sensors->MSSensor.Data.getMSData().getX() - initial_height;

    if (millis() - prevtime >= 100){
        velocity = (height - prevHeight) / 100;
        prevHeight = height;
        prevtime = millis();
    }

    if (velocity < 10 && height > 800){
        digitalWrite(31, HIGH);
#ifdef MODE_TESTING
        Serial.println("EJECT\nEJECT\nEJECT\nEJECT\nEJECT");
#endif
        static uint32_t pm = millis();
        if (millis() - pm >= 5000) digitalWrite(31, LOW);
        return true;
    }
    return false;
}