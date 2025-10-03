#include "ICM42688.h"
#include "SparkFun_BNO08x_Arduino_Library.h"
#include "..\Math\Vector3D.h"
#include "..\Math\QuaternionMath.h"

#define MAIN_IMU_CS 10
#define MAIN_IMU_MOSI 11
#define MAIN_IMU_MISO 12
#define MAIN_IMU_SCK 13
#define MAIN_IMU_INT 9
#define MAIN_IMU_RST 5
#define MAIN_IMU_I2C_ADDR 0x4B // default 0x4B
#define MAIN_IMU_PS1 4
#define MAIN_IMU_PS0 3

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
    double pdt;
    double ydt;
    double rdt;
    double pdtt;
    double ydtt;
    double rdtt;

    // Just pointers, no allocation here
    ICM42688* Secondary;
    BNO08x* Main;

public:
    Vector3D* pyr = new Vector3D(pitch, yaw, roll);
    Vector3D* pyrdt = new Vector3D(pdt, ydt, rdt);
    Vector3D* pyrdtt = new Vector3D(pdtt, ydtt, rdtt);
    Vector3D* U   = new Vector3D(vX, vY, vZ);

    GNCData(ICM42688* secIMU, BNO08x* mainIMU);

    Vector3D CalculateVelocity();
    Vector3D CalculateAcceleration();

    ~GNCData();
};

struct IMUSensors {
    BNO08x MainIMU;
    ICM42688 SecondaryIMU = ICM42688(Wire, SECONDARY_IMU_I2C_ADDR);
    GNCData Data = GNCData(&SecondaryIMU, &MainIMU);
    sh2_SensorValue_t sensorValue;
    sh2_SensorId_t reportType = SH2_ARVR_STABILIZED_RV;
    long reportIntervalUs = 5000;
    bool MainStatus = false;
    bool SecStatus = false;

    void setReports();
    void Init();
    void Update();
};