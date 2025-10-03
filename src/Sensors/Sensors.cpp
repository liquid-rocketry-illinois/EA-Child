#include "Sensors.h"

bool Sensors::InitSensors(){
    Sensors::IMUs.Init();
    //Sensors::MS.Init();
    //Sensors::Temp.Init();
    // Add init functions for other sensors
    
    return (IMUs.MainStatus && IMUs.SecStatus); // Add statuses for the other sensors
    //Add MS and Temp statuses
}

// Separating Sensors because idfk actually
void Sensors::Update(){
    Sensors::IMUs.Update();
    Sensors::MS.Update();
    //Sensors::Temp.Update();

    
}