#include "Sensors/Sensors.h"

bool Sensors::InitSensors(){
    IMUs.Init();
    MSSensor.init();

    return (IMUs.MainStatus /* && IMUs.SecStatus */ && MSSensor.MSstatus); // Add statuses for the other sensors
}

String Sensors::Update(){
    String result = IMUs.Update();
    String MSresult = MSSensor.Update();
    return result + MSresult;
}

String Sensors::Update_Tared(double init_height){
    String result = IMUs.Update_Tared();
    
    String MSresult = MSSensor.Update_Tared(init_height);
    return result + MSresult;
}