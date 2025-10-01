#include "IMU_Sensors.h"

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

void IMUSensors::Init(){
    Wire.begin();
    SecondaryIMU.begin();
    MainIMU.begin_I2C(); // or begin_SPI depending on your wiring
}