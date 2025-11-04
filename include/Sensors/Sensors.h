#pragma once

#include "IMU_Sensors.h"
#include "MS_Sensor.h"

struct Sensors{
    IMUSensors IMUs;
    MSSensors MSSensor;

    bool InitSensors();
    String Update();
};