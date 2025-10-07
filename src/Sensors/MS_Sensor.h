#pragma once

#include "MS5611.h"
#include <Wire.h>

struct SensorReading{
    double pressure;
    double temperature;
    double altitude;
};


class SensorData{
    private:
        // Pointer to barometer
        MS5611* barometer = nullptr;

    public: 
        void setBarometer(MS5611* b){
            barometer = b;
        }
        
        // Returns struct with relevant information
        SensorReading update();
};

struct MSSensors{
    MS5611 MSSensor = MS5611(0x77, &Wire1); // Explicitly use wire1
    bool MSstatus = false;
    SensorData Data;

    SensorReading update();
    void init();
};

