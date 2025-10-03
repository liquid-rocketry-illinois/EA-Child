#pragma once

#include "IMU_Sensors.h"
//#include "MS_Sensor.h"
//#include "SHIT 40.h"
struct Sensors{
    IMUSensors IMUs;
    //MSSensors MS;
    //Temperature Sensors Temp;
    
    bool InitSensors();
    void Update();
};