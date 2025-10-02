#include "Sensors.h"

bool Sensors::InitSensors(){
    Sensors::IMUs.Init();
    // Add init functions for other sensors

    return (IMUs.MainStatus && IMUs.SecStatus); // Add statuses for the other sensors
}

// Separating Sensors because idfk actually
void Sensors::Update(){
    Sensors::IMUs.Update();
}