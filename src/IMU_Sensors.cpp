#include "Sensors/IMU_Sensors.h"
#include "SDCard.h"

GNCData::GNCData(ICM42688* secIMU, BNO08x* mainIMU)
    : Secondary(secIMU), Main(mainIMU){
        pyyr->setX(0);
        pyyr->setY(0);
        pyyr->setZ(0);
        U->setX(0);
        U->setY(0);
        U->setZ(0);
    }

GNCData::~GNCData() {}

void IMUSensors::setReports() {
    MainIMU.enableAccelerometer();
    delay(30);
    MainIMU.enableMagnetometer();
    delay(30);
    MainIMU.enableGyro();
    delay(30);
    (MainIMU.enableRotationVector(4)); // Serial.println("Output pitch yaw roll");
    delay(100);
}

void IMUSensors::Init(){
    // Manually call BNO reset and put into SPI mode
    pinMode(MAIN_IMU_PS0, OUTPUT);
    pinMode(MAIN_IMU_PS1, OUTPUT);
    pinMode(MAIN_IMU_RST, OUTPUT);

    digitalWrite(MAIN_IMU_PS0, HIGH); // PS0 and PS1 == HIGH in order to set as SPI communication
    digitalWrite(MAIN_IMU_PS1, HIGH);
    digitalWrite(MAIN_IMU_RST, LOW); // Reset BNO

    delay(100);

    // Turn BNO back on, wait 300ms to reboot
    digitalWrite(MAIN_IMU_RST, HIGH);
    delay(300);

    if (!MainIMU.beginSPI(MAIN_IMU_CS, MAIN_IMU_INT, MAIN_IMU_RST)) {
#ifdef MODE_TESTING
        Serial.println("Failed to find BNO08x chip");
#endif
        digitalWrite(MAIN_IMU_PS0, LOW);
        digitalWrite(MAIN_IMU_PS1, LOW);
    }
    else{
#ifdef MODE_TESTING
        Serial.println("BNO08x Found!");
#endif
        MainStatus = true;
        MainIMU.softReset();
        delay(500);

        setReports();
        delay(50);
        MainIMU.clearTare();
        MainIMU.tareNow();
        delay(50);

        MainIMU.saveTare();

        
    } 
    
    IMUSensors::Data = GNCData(&SecondaryIMU, &MainIMU);
    
}

String IMUSensors::Update(){
    static unsigned long beforeBNO;
    static unsigned long afterBNO;
    static Vector3D prevOmega = Vector3D(0,0,0);
    static bool quat = false;
    static bool gyro = false;
    static bool mag = false;
    static bool acc = false;

    beforeBNO = millis();
    if (MainIMU.wasReset()) {
#ifdef MODE_TESTING
        Serial.println("sensor was reset "); // COMMENT OUT WHEN DONE
#endif
        setReports();
    }

    // -------- MAIN ----------
    if (MainIMU.getSensorEvent()) {
        uint8_t sensestate = MainIMU.getSensorEventID();
        if(sensestate == SENSOR_REPORTID_ROTATION_VECTOR){
            Data.quats->i = MainIMU.getQuatI();
            Data.quats->j = MainIMU.getQuatJ();
            Data.quats->k = MainIMU.getQuatK();
            Data.quats->r = MainIMU.getQuatReal();

            Data.pyyr->setX(MainIMU.getPitch() * 180.0 / PI);
            Data.pyyr->setY(MainIMU.getYaw() * 180.0 / PI);
            Data.pyyr->setZ(MainIMU.getRoll() * 180.0 / PI);
            quat = true;
        }
        
        if (sensestate == SENSOR_REPORTID_GYROSCOPE_CALIBRATED){
            Data.omega->setX(MainIMU.getGyroX());
            Data.omega->setY(MainIMU.getGyroY());
            Data.omega->setZ(MainIMU.getGyroZ());

            static unsigned long prevMicros = 0;
            unsigned long currMicros = micros();
            if (prevMicros > 0) {
                float dt = (long)(currMicros - prevMicros) / 1.0e6f;
                if (dt > 0) {
                    // Low pass filtering to smooth data
                    Data.alpha->setX(0.2f * ((MainIMU.getGyroX() - prevOmega.getX()) / dt) + (1 - 0.2f) * Data.alpha->getX());
                    Data.alpha->setY(0.2f * ((MainIMU.getGyroY() - prevOmega.getY()) / dt) + (1 - 0.2f) * Data.alpha->getY());
                    Data.alpha->setZ(0.2f * ((MainIMU.getGyroZ() - prevOmega.getZ()) / dt) + (1 - 0.2f) * Data.alpha->getZ());
                }
            }
            prevOmega.setX(MainIMU.getGyroX());
            prevOmega.setY(MainIMU.getGyroY());
            prevOmega.setZ(MainIMU.getGyroZ());
            prevMicros = currMicros;
            gyro = true;
        }

        if (sensestate == SENSOR_REPORTID_ACCELEROMETER){
            Data.A->setX(MainIMU.getAccelX());
            Data.A->setY(MainIMU.getAccelY());
            Data.A->setZ(MainIMU.getAccelZ());
            acc = true;
        }

        if (sensestate == SENSOR_REPORTID_MAGNETIC_FIELD){
            Data.Magnet->setX(MainIMU.getMagX());
            Data.Magnet->setY(MainIMU.getMagY());
            Data.Magnet->setZ(MainIMU.getMagZ());
            mag = true;
        }
    }
    afterBNO = millis();
    gyro = false;
    quat = false;
    mag = false;
    acc = false;
   
    static char buffer[540];
    snprintf(buffer, sizeof(buffer),
        "||%lu|%.3f,%.3f,%.3f,%.3f|%.3f,%.3f,%.3f|%.3f,%.3f,%.3f|%.3f,%.3f,%.3f|%.3f,%.3f,%.3f|%.3f,%.3f,%.3f|%lu||",
        beforeBNO,
        Data.quats->i,Data.quats->j,Data.quats->k,Data.quats->r,
        Data.Magnet->getX(), Data.Magnet->getY(), Data.Magnet->getZ(),
        Data.pyyr->getX(), Data.pyyr->getY(), Data.pyyr->getZ(),
        Data.A->getX(), Data.A->getY(), Data.A->getZ(),
        Data.omega->getX(), Data.omega->getY(), Data.omega->getZ(),
        Data.alpha->getX(), Data.alpha->getY(), Data.alpha->getZ(),
        afterBNO
    );
#ifdef MODE_TESTING
    Serial.println(buffer);
    Serial.println(strlen(buffer));
#endif
    return (String)buffer; // Code is crashing here but only after a few cycles
}

String IMUSensors::Update_Tared(){
    static unsigned long beforeBNO;
    static unsigned long afterBNO;
    static Vector3D prevOmega = Vector3D(0,0,0);
    static bool quat = false;
    static bool gyro = false;
    static bool mag = false;
    static bool acc = false;

    beforeBNO = millis();
    if (MainIMU.wasReset()) {
#ifdef MODE_TESTING
        Serial.println("sensor was reset "); // COMMENT OUT WHEN DONE
#endif
        setReports();
    }

    // -------- MAIN ----------
    if (MainIMU.getSensorEvent()) {
        uint8_t sensestate = MainIMU.getSensorEventID();
        //Serial.println(sensestate);
        while (!quat && !gyro && !acc && !mag){
            if(sensestate == SENSOR_REPORTID_ROTATION_VECTOR && !quat){
                Data.quats->i = MainIMU.getQuatI();
                Data.quats->j = MainIMU.getQuatJ();
                Data.quats->k = MainIMU.getQuatK();
                Data.quats->r = MainIMU.getQuatReal();

                Data.pyyr->setX(MainIMU.getPitch() * 180.0 / PI);
                Data.pyyr->setY(MainIMU.getYaw() * 180.0 / PI);
                Data.pyyr->setZ(MainIMU.getRoll() * 180.0 / PI);
                quat == true;
            }
            
            if (sensestate == SENSOR_REPORTID_GYROSCOPE_CALIBRATED && !gyro){
                Data.omega->setX(MainIMU.getGyroX());
                Data.omega->setY(MainIMU.getGyroY());
                Data.omega->setZ(MainIMU.getGyroZ());

                static unsigned long prevMicros = 0;
                unsigned long currMicros = micros();
                if (prevMicros > 0) {
                    float dt = (long)(currMicros - prevMicros) / 1.0e6f;
                    if (dt > 0) {
                        // Low pass filtering to smooth data
                        Data.alpha->setX(0.2f * ((MainIMU.getGyroX() - prevOmega.getX()) / dt) + (1 - 0.2f) * Data.alpha->getX());
                        Data.alpha->setY(0.2f * ((MainIMU.getGyroY() - prevOmega.getY()) / dt) + (1 - 0.2f) * Data.alpha->getY());
                        Data.alpha->setZ(0.2f * ((MainIMU.getGyroZ() - prevOmega.getZ()) / dt) + (1 - 0.2f) * Data.alpha->getZ());
                    }
                }
                prevOmega.setX(MainIMU.getGyroX());
                prevOmega.setY(MainIMU.getGyroY());
                prevOmega.setZ(MainIMU.getGyroZ());
                prevMicros = currMicros;
                gyro = true;
            }

            if (sensestate == SENSOR_REPORTID_ACCELEROMETER && !acc){
                Data.A->setX(MainIMU.getAccelX());
                Data.A->setY(MainIMU.getAccelY());
                Data.A->setZ(MainIMU.getAccelZ());
                acc = true;
            }

            if (sensestate == SENSOR_REPORTID_MAGNETIC_FIELD && !mag){
                Data.Magnet->setX(MainIMU.getMagX());
                Data.Magnet->setY(MainIMU.getMagY());
                Data.Magnet->setZ(MainIMU.getMagZ());
                mag = true;
            }
        }
    }
    afterBNO = millis();
    gyro = false;
    quat = false;
    mag = false;
    acc = false;
   
    char buffer[540];
    snprintf(buffer, sizeof(buffer),
        "||%lu|%.3f,%.3f,%.3f,%.3f|%.3f,%.3f,%.3f|%.3f,%.3f,%.3f|%.3f,%.3f,%.3f|%.3f,%.3f,%.3f|%.3f,%.3f,%.3f|%lu||",
        beforeBNO,
        Data.quats->i,Data.quats->j,Data.quats->k,Data.quats->r,
        Data.Magnet->getX(), Data.Magnet->getY(), Data.Magnet->getZ(),
        Data.pyyr->getX(), Data.pyyr->getY(), Data.pyyr->getZ(),
        Data.A->getX(), Data.A->getY(), Data.A->getZ(),
        Data.omega->getX(), Data.omega->getY(), Data.omega->getZ(),
        Data.alpha->getX(), Data.alpha->getY(), Data.alpha->getZ(),
        afterBNO
    );
#ifdef MODE_TESTING
    Serial.println(buffer);
#endif
    return String(buffer);
}