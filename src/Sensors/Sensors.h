#pragma once

#include "IMU_Sensors.h"
#include "MS_Sensor.h"

struct Sensors{
    IMUSensors IMUs;

    bool InitSensors();
    String Update();
    Vector3D* getPYR();
};