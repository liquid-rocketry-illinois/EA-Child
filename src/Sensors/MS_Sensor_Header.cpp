#include "MS5611.h"
#include "MS_Sensor_Header.h"


MS5611 ms5611(0x77);


void setup()
{
  Serial.begin(115200);
  while (!Serial);
  Serial.println();
  Serial.println(__FILE__);
  Serial.print("MS5611_LIB_VERSION: ");
  Serial.println(MS5611_LIB_VERSION); 
  Serial.println();

  Wire.begin();
  if (ms5611.begin() == true)
  {
    Serial.print("MS5611 found: ");
    Serial.println(ms5611.getAddress());
  }
  else
  {
    Serial.println("MS5611 not found. halt.");
    while (1);
  }
  Serial.println();
}

String something(){

}



