#include "Sensors.h"

bool Sensors::InitSensors(){
    Sensors::IMUs.Init();
    Sensors::MSSensor.init();
    // Add init functions for other sensors

    return (IMUs.MainStatus && IMUs.SecStatus && MSSensor.MSstatus); // Add statuses for the other sensors
}

// Separating Sensors because idfk actually
void Sensors::Update(){
    Sensors::IMUs.Update();
    Sensors::MSSensor.update();
}