#include "MS_Sensor_Header.h"
#include "ICM_Sensor.h"

class Sensors{
    private:

    //objs
    IMU

    //math for conversion
    struct SensorData {
        uint32_t timestamp;
        Ambient::AmbientData ambientData;
        AxisData magData;
        IMU::IMUData icmData;
        IMU::IMUData bmiData;
        GNSS::GNSSData gnssData;
    };
    public:
    Sensors(){}
    void Update(){}
    ~Sensors(){}
};