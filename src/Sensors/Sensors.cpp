#include "Sensors.h"

bool Sensors::InitSensors(){
    Sensors::IMUs.Init();
    // Debug printlns
    Serial.print("Intializing MS sensor");
    Serial.print(Sensors::MSSensor.MSstatus);
    
    Sensors::MSSensor.init();
    // Add init functions for other sensors

    return (IMUs.MainStatus && IMUs.SecStatus && MSSensor.MSstatus); // Add statuses for the other sensors
}

// Separating Sensors because idfk actually
void Sensors::Update(){
    Sensors::IMUs.Update();
    Sensors::MSSensor.update();
}