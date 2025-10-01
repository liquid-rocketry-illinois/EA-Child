#include "ICM42688.h"
#include "Adafruit_BNO08x.h"
#include "..\Math\Vector3D.h"

#define MAIN_IMU_CS 10
#define MAIN_IMU_MOSI 11
#define MAIN_IMU_MISO 12
#define MAIN_IMU_SCK 13
#define MAIN_IMU_RST 9 // INTERRUPT PIN

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

    GNCData(ICM42688* secIMU, Adafruit_BNO08x* mainIMU)
        : Secondary(secIMU), Main(mainIMU) {
        pyr->setX(0);
        pyr->setY(0);
        pyr->setZ(0);
        U->setX(0);
        U->setY(0);
        U->setZ(0);
    }

    Vector3D CalculateVelocity();
    Vector3D CalculateAcceleration();
    void Update();

    ~GNCData();
};

struct IMUSensors {
    Adafruit_BNO08x IMUSensors::MainIMU = Adafruit_BNO08x(MAIN_IMU_RST);
    ICM42688 IMUSensors::SecondaryIMU = ICM42688(Wire, SECONDARY_IMU_I2C_ADDR);
    GNCData IMUSensors::Data = GNCData(&SecondaryIMU, &MainIMU);
    void Init();
};