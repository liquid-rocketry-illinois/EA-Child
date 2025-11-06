#pragma once

#include <Arduino.h>
#include <Wire.h>
#include "MS5611.h"
#include "../Math/Vector3D.h"
#include "DEBUG_SWITCH.h"

struct SensorReading {
    double pressure;     // Pressure in mbar (or hPa)
    double temperature;  // Temperature in °C
    double altitude;     // Altitude in meters (computed using barometric formula)
};

class SensorData {
private:
    MS5611* barometer = nullptr;
    Vector3D MSData;
    double initial_height = 0;

public:
    Vector3D getMSData();

    // Assign external MS5611 object pointer
    inline void setBarometer(MS5611* b) {
        barometer = b;
    }

    inline void setInitHeight(double in_height) {
        initial_height = in_height;
    }

    // Returns struct with latest sensor data
    SensorReading update();
    SensorReading update_tared(double init_height);
};

struct MSSensors {
    // Instantiate MS5611 specifically on Wire12 (Teensy 4.1 tertiary I2C)
    MS5611 MSSensor = MS5611(0x77, &Wire2);

    bool MSstatus = false; // Will be set true if initialization succeeds
    SensorData Data;

    // Initialize barometer hardware & assign pointer
    void init();

    // Update reading through SensorData wrapper
    const char*  Update();
    const char*  Update_Tared(double init_height);
};

