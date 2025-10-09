#include "Sensors.h"

bool Sensors::InitSensors(){
    IMUs.Init();
    // Debug printlns
    Serial.print("Intializing MS sensor");
    Serial.print(MSSensor.MSstatus);
    
    MSSensor.init();
    // Add init functions for other sensors

    return (IMUs.MainStatus && IMUs.SecStatus && MSSensor.MSstatus); // Add statuses for the other sensors
}

String Sensors::Update(){
    String result = IMUs.Update();
    // String MSresult = MSSensor.update();  Convert to string of data
    return result;
}