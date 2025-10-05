#include "IMU_Sensors.h"

GNCData::GNCData(ICM42688* secIMU, BNO08x* mainIMU)
    : Secondary(secIMU), Main(mainIMU){
        pyr->setX(0);
        pyr->setY(0);
        pyr->setZ(0);
        U->setX(0);
        U->setY(0);
        U->setZ(0);
    }

Vector3D GNCData::CalculateVelocity(){
    return Vector3D();
}

Vector3D GNCData::CalculateAcceleration(){
    return Vector3D();
}

GNCData::~GNCData() {
    delete pyr;
    delete U;
}

void IMUSensors::setReports() {
  Serial.println("Setting desired reports");
  if (MainIMU.enableRotationVector() == true) {
    Serial.println(F("Rotation vector enabled"));
    Serial.println(F("Output in form roll, pitch, yaw"));
  } else {
    Serial.println("Could not enable rotation vector");
  }
}

void IMUSensors::Init(){
    pinMode(MAIN_IMU_PS0, OUTPUT);
    pinMode(MAIN_IMU_PS1, OUTPUT);

    digitalWrite(MAIN_IMU_PS0, HIGH);
    digitalWrite(MAIN_IMU_PS1, HIGH);

    if (!SecondaryIMU.begin()){
        Serial.println("Failed to find ICM42688 chip");
    }
    else{
        Serial.println("ICM42688 Found!");
        SecStatus = true;

        // setting the accelerometer full scale range to +/-8G
        SecondaryIMU.setAccelRange(ICM42688::ACCEL_RANGE_8G);
        // setting the gyroscope full scale range to +/-500 deg/s
        SecondaryIMU.setGyroRange(ICM42688::GYRO_RANGE_500DPS);
        // setting DLPF bandwidth to 20 Hz
        SecondaryIMU.setDlpfBandwidth(ICM42688::DLPF_BANDWIDTH_20HZ);

        SecondaryIMU.calibrateAccel();
        SecondaryIMU.calibrateGyro();
    }
    if (!MainIMU.beginSPI(MAIN_IMU_CS, MAIN_IMU_INT, MAIN_IMU_RST)) {
        Serial.println("Failed to find BNO08x chip");
        digitalWrite(MAIN_IMU_PS0, LOW);
        digitalWrite(MAIN_IMU_PS1, LOW);
    }
    else{
        Serial.println("BNO08x Found!");
        MainStatus = true;

        MainIMU.clearTare();
        MainIMU.tareNow();

        MainIMU.enableAccelerometer();
        MainIMU.enableMagnetometer();
        MainIMU.enableGyro();
        setReports();
    } 
    
    IMUSensors::Data = GNCData(&SecondaryIMU, &MainIMU);
    
}

void IMUSensors::Update(){
    if (MainIMU.wasReset()) {
        //Serial.print("sensor was reset "); // COMMENT OUT WHEN DONE
        setReports();
    }
/*
    static bool Calibrated = false;
    static const uint16_t first_millis = (uint16_t)millis();

    if (((millis() - first_millis) > 2000) && (Calibrated == false)){
        MainIMU.tareNow();
        if (MainIMU.clearTare()) {
            delayMicroseconds(100000);
            MainIMU.tareNow();
            MainIMU.saveTare();
        }
        Calibrated = true;
    }
*/

    // -------- MAIN ----------
    if (MainIMU.getSensorEvent()) {
        // is it the correct sensor data we want?
        if (MainIMU.getSensorEventID() == SENSOR_REPORTID_ROTATION_VECTOR) {
            Data.pyr->setX((MainIMU.getPitch()) * 180.0 / PI);
            Data.pyr->setY((MainIMU.getYaw()) * 180.0 / PI);
            Data.pyr->setZ((MainIMU.getRoll()) * 180.0 / PI);

            Data.omega->setX(MainIMU.getGyroX());
            Data.omega->setY(MainIMU.getGyroY());
            Data.omega->setZ(MainIMU.getGyroZ());

            static Vector3D prevOmega(0,0,0);
            static unsigned long prevMicros = micros();
            unsigned long currMicros = micros();
            float dt = (currMicros - prevMicros) / 1.0e6; // convert µs to seconds
            if (dt > 0) {
                Data.alpha->setX((Data.omega->getX() - prevOmega.getX()) / dt);
                Data.alpha->setY((Data.omega->getY() - prevOmega.getY()) / dt);
                Data.alpha->setZ((Data.omega->getZ() - prevOmega.getZ()) / dt);
            }
            prevOmega = *Data.omega;
            prevMicros = currMicros;

            Data.A->setX(MainIMU.getAccelX());
            Data.A->setY(MainIMU.getAccelY());
            Data.A->setZ(MainIMU.getAccelZ());

            Data.Magnet->setX(MainIMU.getMagX());
            Data.Magnet->setY(MainIMU.getMagY());
            Data.Magnet->setZ(MainIMU.getMagZ());

            Serial.print(millis());                         Serial.print(" ");Serial.print(" . ");

            Serial.print(Data.Magnet->getX());              Serial.print(" ");
            Serial.print(Data.Magnet->getY());              Serial.print(" ");
            Serial.print(Data.Magnet->getZ());              Serial.print(" ");Serial.print(" . ");

            Serial.print(Data.pyr->getX());              Serial.print(" ");
            Serial.print(Data.pyr->getY());              Serial.print(" ");
            Serial.print(Data.pyr->getZ());              Serial.print(" ");Serial.print(" . ");

            Serial.print(Data.A->getX());              Serial.print(" ");
            Serial.print(Data.A->getY());              Serial.print(" ");
            Serial.print(Data.A->getZ());              Serial.print(" ");Serial.print(" . ");

            Serial.print(Data.omega->getX());              Serial.print(" ");
            Serial.print(Data.omega->getY());              Serial.print(" ");
            Serial.print(Data.omega->getZ());              Serial.print(" ");Serial.print(" . ");

            Serial.print(Data.alpha->getX());              Serial.print(" ");
            Serial.print(Data.alpha->getY());              Serial.print(" ");
            Serial.print(Data.alpha->getZ());              Serial.print(" ");Serial.print(" . ");
            Serial.println(millis());
            //Serial.println("Data printed for BNO.");
        }
    }
    else Serial.println("Sensor data incorrect!");

    // -------- SECONDARY ---------
/*
    if (SecondaryIMU.readSensor() == 1){
        Serial.print(SecondaryIMU.getAccelX_mss(),3);
        Serial.print("\t");
        Serial.print(SecondaryIMU.getAccelY_mss(),3);
        Serial.print("\t");
        Serial.print(SecondaryIMU.getAccelZ_mss(),3);
        Serial.print("\t");
        Serial.print(SecondaryIMU.getGyroX_rads(),3);
        Serial.print("\t");
        Serial.print(SecondaryIMU.getGyroY_rads(),3);
        Serial.print("\t");
        Serial.print(SecondaryIMU.getGyroZ_rads(),3);
        Serial.print("\t");
        Serial.println(SecondaryIMU.getTemperature_C(),2);
    }
    else Serial.println("Secondary IMU Data Fail!");
    */
}