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
        MS5611* barometer;

    public: 
        String update();
};


struct MSSensors{
    //Declares the address (or something)
    MS5611 MSSensor = MS5611(MS5611_SDA_PIN); 
    bool MSstatus;
    SensorData Data;

    String update();
    void init();

};

