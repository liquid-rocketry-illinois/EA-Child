#include "MS5611.h"
#include "MS_Sensor.h"


MS5611 ms5611(0x77);

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
  if (ms5611.begin())
  {
    Serial.print("MS5611 found: ");
    Serial.println(ms5611.getAddress());
    MSstatus = true;
  }
  else
  {
    while(!ms5611.begin()){ 
      Serial.println("MS5611 not found. Check connections.");
    }
  }
  Serial.println();
}

/**
 * SensorData returns a CSV string in the form temperature, pressure, altitude.
 */
String SensorData::update(){
  ms5611.read();
  String temperature = String(ms5611.getTemperature());
  String pressure = String(ms5611.getPressure());
  String altitude = String(ms5611.getAltitude());
  return temperature + "," + pressure + "," + altitude;
}






