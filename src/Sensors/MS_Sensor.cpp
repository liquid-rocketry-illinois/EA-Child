#include "MS_Sensor.h"
#include <Wire.h>

/**
 * init will attempt to initialize the MS sensor with I2C.
 * 
 */
void MSSensors::init(){
    Wire2.setClock(100000); // Only need to set clk speed, main.cpp inits everything else

    if (MSSensor.begin()){
      MSstatus = true;
      MSSensor.setOversampling(OSR_ULTRA_HIGH);

      // Hook SensorData to sensor
      Data.setBarometer(&MSSensor);
      Serial.print("MS5611 initialised on address: "); 
      Serial.println(MSSensor.getAddress());
    }
    else Serial.println("MS5611 not found or unable to initialize!");
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

  barometer -> read(0);
  delay(10);
  float temp = barometer -> getTemperature();
  float pres = barometer -> getPressure();

  const float calibrationOffset = 27.7;
  float calibratedPressure = pres + calibrationOffset;
  
  reading.temperature = temp;
  reading.pressure = calibratedPressure;
  //reading.altitude = barometer -> getAltitude();

  // Manually calc the altitude because the formula is going wild
  float pressureRatio = calibratedPressure / 1013.25;
  reading.altitude = 44307.69396 * (1 - pow(pressureRatio, 0.190284));

  return reading;
}






