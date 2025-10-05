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
    uint32_t previous = 0;

    // -------- MAIN ----------
    if (MainIMU.getSensorEvent()) {
        // is it the correct sensor data we want?
        if (MainIMU.getSensorEventID() == SENSOR_REPORTID_ROTATION_VECTOR) {
            Data.pyr->setX((MainIMU.getPitch()) * 180.0 / PI);
            Data.pyr->setY((MainIMU.getYaw()) * 180.0 / PI);
            Data.pyr->setZ((MainIMU.getRoll()) * 180.0 / PI);
            
            double prevPitch = Data.pyr->getX();
            double prevYaw = Data.pyr->getY();
            double prevRoll = Data.pyr->getZ();
            if (Data.pyr->getX() != 0.0 && Data.pyr->getY() != 0.0 && Data.pyr->getZ() != 0.0) previous = millis();

            Data.pyrdt->setX(MainIMU.getGyroX());
            Data.pyrdt->setY(MainIMU.getGyroY());
            Data.pyrdt->setZ(MainIMU.getGyroZ());

            Serial.print(Data.pyr->getX());              Serial.print("\t");
            Serial.print(Data.pyr->getY());              Serial.print("\t");
            Serial.print(Data.pyr->getZ());              Serial.print("\t");Serial.print("\t");

            // NEED THIS TO BE VELOCITY BUT IT ISN'T :(
            Serial.print(Data.pyrdt->getX());              Serial.print("\t");
            Serial.print(Data.pyrdt->getY());              Serial.print("\t");
            Serial.println(Data.pyrdt->getZ());
            //Serial.println("Data printed for BNO.");
        }
    }
    else Serial.println("Sensor data incorrect!");

    // -------- SECONDARY ---------

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
    //else Serial.println("Secondary IMU Data Fail!");
}