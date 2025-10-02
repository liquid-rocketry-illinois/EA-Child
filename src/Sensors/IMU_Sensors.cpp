#include "IMU_Sensors.h"

GNCData::GNCData(ICM42688* secIMU, Adafruit_BNO08x* mainIMU)
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

void IMUSensors::setReports(sh2_SensorId_t reportType, long report_interval) {
  Serial.println("Setting desired reports");
  if (! MainIMU.enableReport(reportType, (uint32_t)report_interval)) {
    Serial.println("Could not enable stabilized remote vector");
  }
}

void IMUSensors::Init(){
    if (!Wire.available()) Wire.begin();
    delay(100);
    if (!SecondaryIMU.begin()){
        Serial.println("Failed to find ICM42688 chip");
    }
    else{
        Serial.println("ICM42688 Found!");
        SecStatus = true;
    }
    if (!MainIMU.begin_SPI(MAIN_IMU_CS, MAIN_IMU_INT, &SPI, 0x01)) { // SENSOR ID = 0x01; decimal 1
        Serial.println("Failed to find BNO08x chip");
    }
    else{
        Serial.println("BNO08x Found!");
        MainStatus = true;
    } 

    
// BNO (Main):
    setReports(reportType, reportIntervalUs);

// ICM (Secondary):
    // setting the accelerometer full scale range to +/-8G
    SecondaryIMU.setAccelRange(ICM42688::ACCEL_RANGE_8G);
    // setting the gyroscope full scale range to +/-500 deg/s
    SecondaryIMU.setGyroRange(ICM42688::GYRO_RANGE_500DPS);
    // setting DLPF bandwidth to 20 Hz
    SecondaryIMU.setDlpfBandwidth(ICM42688::DLPF_BANDWIDTH_20HZ);

    IMUSensors::Data = GNCData(&SecondaryIMU, &MainIMU);
}

void IMUSensors::Update(){
    if (MainIMU.wasReset()) {
    Serial.print("sensor was reset "); // COMMENT OUT WHEN DONE
    IMUSensors::setReports(reportType, reportIntervalUs);
    }

    // -------- MAIN ----------
  
    if (MainIMU.getSensorEvent(&sensorValue)) {
        // in this demo only one report type will be received depending on FAST_MODE define (above)
        switch (sensorValue.sensorId) {
        case SH2_ARVR_STABILIZED_RV: // This is used rn
            QuatMath::quaternionToEulerRV(&sensorValue.un.arvrStabilizedRV, Data.pyr, true);
        case SH2_GYRO_INTEGRATED_RV:
            // faster (more noise?)
            QuatMath::quaternionToEulerGI(&sensorValue.un.gyroIntegratedRV, Data.pyr, true);
            break;
        }
        static long last = 0;
        long now = micros();
        Serial.print(now - last);             Serial.print("\t"); // COMMENT OUT WHEN DONE
        last = now;

        // COMMENT OUT WHEN DONE
        Serial.print(sensorValue.status);     Serial.print("\t");  // This is accuracy in the range of 0 to 3
        Serial.print(Data.pyr->getX());                Serial.print("\t");
        Serial.print(Data.pyr->getY());              Serial.print("\t");
        Serial.println(Data.pyr->getZ());
    }

    // -------- SECONDARY ---------

    SecondaryIMU.readSensor();
    
    Serial.print(SecondaryIMU.getAccelX_mss(),6);
    Serial.print("\t");
    Serial.print(SecondaryIMU.getAccelY_mss(),6);
    Serial.print("\t");
    Serial.print(SecondaryIMU.getAccelZ_mss(),6);
    Serial.print("\t");
    Serial.print(SecondaryIMU.getGyroX_rads(),6);
    Serial.print("\t");
    Serial.print(SecondaryIMU.getGyroY_rads(),6);
    Serial.print("\t");
    Serial.print(SecondaryIMU.getGyroZ_rads(),6);
    Serial.print("\t");
    Serial.println(SecondaryIMU.getTemperature_C(),6);
}