#include "ICM42688.h"
#include "Adafruit_BNO08x.h"
#include "..\Math\Vector3D.h"
#include "..\Math\QuaternionMath.h"

#define MAIN_IMU_CS 10
#define MAIN_IMU_MOSI 11
#define MAIN_IMU_MISO 12
#define MAIN_IMU_SCK 13
#define MAIN_IMU_INT 9
#define MAIN_IMU_RST 5

#define SEC_IMU_SCL 19
#define SEC_IMU_SDA 18
#define SECONDARY_IMU_I2C_ADDR 0x68 // 104

class GNCData {
private:
    double pitch;
    double yaw;
    double roll;
    double vX;
    double vY;
    double vZ;

    // Just pointers, no allocation here
    ICM42688* Secondary;
    Adafruit_BNO08x* Main;

public:
    Vector3D* pyr = new Vector3D(pitch, yaw, roll);
    Vector3D* U   = new Vector3D(vX, vY, vZ);

    GNCData(ICM42688* secIMU, Adafruit_BNO08x* mainIMU);

    Vector3D CalculateVelocity();
    Vector3D CalculateAcceleration();

    ~GNCData();
};

struct IMUSensors {
    Adafruit_BNO08x MainIMU = Adafruit_BNO08x(MAIN_IMU_RST); // SPI needs Reset pin defn, for i2c pin defn is -1
    ICM42688 SecondaryIMU = ICM42688(Wire, SECONDARY_IMU_I2C_ADDR);
    GNCData Data = GNCData(&SecondaryIMU, &MainIMU);
    sh2_SensorValue_t sensorValue;
    sh2_SensorId_t reportType = SH2_ARVR_STABILIZED_RV;
    long reportIntervalUs = 5000;
    bool MainStatus = false;
    bool SecStatus = false;

    void setReports(sh2_SensorId_t reportType, long report_interval);
    void Init();
    void Update();
};