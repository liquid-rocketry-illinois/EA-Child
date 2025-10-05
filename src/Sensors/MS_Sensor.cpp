#define Wire Wire2   // Trick library to use Wire2 instead of Wire
#include "MS5611.h"
#undef Wire
#include "MS_Sensor.h"
#include <Wire.h>

/**
 * init will attempt to initialize the MS sensor with I2C.
 * 
 */
void MSSensors::init(){
  Serial.begin(115200);
  while (!Serial);
  Serial.println();
  Serial.println(__FILE__);
  Serial.print("MS5611_LIB_VERSION: ");
  Serial.println(MS5611_LIB_VERSION); 
  Serial.println();

  Wire2.begin();

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

/**
 * update returns a SensorReading struct with .temperature, .pressure
 * and .altitude. 
 * 
 */
SensorReading MSSensors::update(){
  SensorReading r = Data.update();
  if(MSstatus){
    Serial.print("Temp: ");                   
    Serial.print(r.temperature);              
    Serial.print(" °C, Pressure: ");
    Serial.print(r.pressure); 
    Serial.print(" mbar, Altitude: ");
    Serial.print(r.altitude);
    Serial.println(" m");
  }
  return r;
}

/**
 * SensorData returns a SensorReading struct with temperature, pressure and altitude. 
 */
SensorReading SensorData::update(){
  SensorReading reading{};
  
  // To avoid hitting a nullptr
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






