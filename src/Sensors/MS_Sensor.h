#pragma once

#include "MS5611.h"

#define MS5611_SCL_PIN 24
#define MS5611_SDA_PIN 25

class SensorData{
    private:
        double pressure;
        double temperature;
        double altitude;

        // Pointer to barometer
        MS5611* barometer = nullptr;

    public: 
        void setBarometer(MS5611* b){
            barometer = b;
        }
        
        // Returns CSV: temperature,pressure,altitude
        String update();
};


struct MSSensors{
    MS5611 MSSensor = MS5611(0x77); // Default address
    bool MSstatus = false;
    SensorData Data;

    String update();
    void init();
};

