#include "Sensors/Sensors.h"

bool Sensors::InitSensors(){
    IMUs.Init();
    MSSensor.init();

    return (IMUs.MainStatus /* && IMUs.SecStatus */ && MSSensor.MSstatus); // Add statuses for the other sensors
}

const char* Sensors::Update() {
    static char buffer[700]; // adjust size as needed

    const char* imu = IMUs.Update();       // must return const char*
    const char* ms  = MSSensor.Update();   // must return const char*

    snprintf(buffer, sizeof(buffer), "%s%s", imu, ms);

    return buffer;
}

String Sensors::Update_Tared(double init_height){
    String result = IMUs.Update_Tared();
    Serial.println("successfully returned IMUdata");
    String MSresult = MSSensor.Update_Tared(init_height);
    return result + MSresult;
}