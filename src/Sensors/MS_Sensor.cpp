#include "MS5611.h"
#include "MS_Sensor.h"
#include <Wire.h>

void MSSensors::init()
{
  Serial.begin(115200);
  while (!Serial);
  Serial.println();
  Serial.println(__FILE__);
  Serial.print("MS5611_LIB_VERSION: ");
  Serial.println(MS5611_LIB_VERSION); 
  Serial.println();

  Wire.begin();

  if (MSSensor.begin())
  {
    Serial.print("MS5611 found: ");
    Serial.println(MSSensor.getAddress());
    MSstatus = true;
    // Hook SensorData to sensor
    Data.setBarometer(&MSSensor);
  }else{
    // Keep retrying
    while(!MSSensor.begin()){ 
      Serial.println("MS5611 not found. Check connections.");
      delay(2000);
    }
    MSstatus = true;
    Data.setBarometer(&MSSensor);
  }
  Serial.println("MS5611 initialised.");
}

SensorReading MSSensors::update(){
  return Data.update();
}

/**
 * SensorData returns a SensorReading struct with temperature, pressure and altitude. 
 */
SensorReading SensorData::update(){
  barometer -> read();
  SensorReading reading{};
  
  if(!barometer){
    reading.temperature = NAN;
    reading.pressure = NAN;
    reading.altitude = NAN;
    return reading;
  }

  barometer -> read();

  reading.temperature = barometer -> getTemperature();
  reading.pressure = barometer -> getPressure();
  reading.altitude = barometer -> getAltitude();

  return reading;
}






