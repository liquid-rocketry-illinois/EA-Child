#pragma once

#include "IMU_Sensors.h"
#include "MS_Sensor.h"
#include "DEBUG_SWITCH.h"
struct Sensors{
    IMUSensors IMUs;
    MSSensors MSSensor;

    bool InitSensors();
    const char* Update();
    String Update_Tared(double init_height);
};