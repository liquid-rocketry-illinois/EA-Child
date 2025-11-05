#include "Sensors/MS_Sensor.h"

// Standard sea-level pressure in millibars
static constexpr double P0 = 1013.25;
#define MS_I2C_EN_PIN 29 // enable i2c pin for p0

// --- SensorData::update() ---
SensorReading SensorData::update() {
    SensorReading reading = {0, 0, 0};

    if (!barometer) {
        return reading; // return zeros if barometer not assigned
    }

    // Request raw measurements & compute compensation
    barometer->read(); // Performs temperature and pressure conversion

    reading.temperature = barometer->getTemperature(); // °C
    reading.pressure = barometer->getPressure();       // mbar / hPa

    // Compute altitude using the barometric formula
    // h = 44330 * (1 - (P / P0)^(0.1903))
    reading.altitude = 44330.0 * (1.0 - pow(reading.pressure / P0, 0.1903));

    MSData.setX(reading.altitude);
    MSData.setY(reading.pressure);
    MSData.setZ(reading.temperature);
    return reading;
}


Vector3D SensorData::getMSData(){ // Vector3D(altitude,pressure,temperature)
    return MSData;
}

// --- MSSensors::init() ---
void MSSensors::init() {
    // Initialize sensor hardware
    pinMode(MS_I2C_EN_PIN, OUTPUT);

    digitalWrite(MS_I2C_EN_PIN, HIGH); // HIGH in order to set as I2C communication

    MSstatus = MSSensor.begin();
    if (MSstatus) {
        Data.setBarometer(&MSSensor);
    }
}

// --- MSSensors::update() ---
String MSSensors::Update() {
    if (!MSstatus) {
        return String(); // Return nothing if sensor not initialized
    }
    SensorReading data = Data.update();

    char buffer[50];
    snprintf(buffer, sizeof(buffer),
        "||%.3f,%.3f,%.3f||",
        data.altitude, data.pressure, data.temperature
    );

    Serial.println(buffer);
    return (String)buffer;
}
