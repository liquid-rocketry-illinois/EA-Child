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
  Serial.println();
}

String MSSensors::update(){
  return Data.update();
}

/**
 * Update returns a CSV string in the form temperature, pressure, altitude.
 */
String SensorData::update(){
  barometer -> read();
  String temperature = String(barometer->getTemperature());
  String pressure = String(barometer->getPressure());
  String altitude = String(barometer->getAltitude());
  return temperature + "," + pressure + "," + altitude;
}






