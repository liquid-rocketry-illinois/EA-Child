#include "Sensors/Sensors.h"

bool Sensors::InitSensors(){
    IMUs.Init();
    
    MSSensor.init();
    // Debug printlns
    Serial.print("MS sensor status: ");
    Serial.println(MSSensor.MSstatus);

    return (IMUs.MainStatus /* && IMUs.SecStatus */ && MSSensor.MSstatus); // Add statuses for the other sensors
}

String Sensors::Update(){
    String result = IMUs.Update();
    String MSresult = MSSensor.Update();
    return result + MSresult;
}