#include "MS_Sensor.h"
#include <Wire.h>

/**
 * init will attempt to initialize the MS sensor with I2C.
 * 
 */
void MSSensors::init(){
  while (!Serial);
  Serial.println();
  Serial.println(__FILE__);
  Serial.print("MS5611_LIB_VERSION: ");
  Serial.println(MS5611_LIB_VERSION); 
  Serial.println();

  Wire2.begin();
  Wire2.setClock(100000);

  delay(20);  

  if (MSSensor.begin()){
    Serial.print("MS5611 found: ");
    Serial.println(MSSensor.getAddress());
    MSstatus = true;
    MSSensor.setOversampling(OSR_ULTRA_HIGH);

    // Hook SensorData to sensor
    Data.setBarometer(&MSSensor);
  }else{
    // Keep retrying
    while(!MSSensor.begin()){ 
      Serial.println("MS5611 not found. Check connections.");
      delay(1000);
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






