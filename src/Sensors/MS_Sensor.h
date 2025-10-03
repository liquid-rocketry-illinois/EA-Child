#pragma once

#include "MS5611.h"

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
        
        // Returns CSV: temperature,pressure,altitude
        SensorReading update();
};


struct MSSensors{
    MS5611 MSSensor = MS5611(0x77); // Default address
    bool MSstatus = false;
    SensorData Data;

    SensorReading update();
    void init();
};

