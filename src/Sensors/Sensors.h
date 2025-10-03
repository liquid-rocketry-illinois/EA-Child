#pragma once

#include "IMU_Sensors.h"

struct Sensors{
    IMUSensors IMUs;

    bool InitSensors();
    void Update();
};