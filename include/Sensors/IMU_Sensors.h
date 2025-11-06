#include "ICM42688.h"
#include "SparkFun_BNO08x_Arduino_Library.h"
#include "../Math/Vector3D.h"
#include "../Math/QuaternionMath.h"
#include "DEBUG_SWITCH.h"

#define MAIN_IMU_CS 10
#define MAIN_IMU_MOSI 11
#define MAIN_IMU_MISO 12
#define MAIN_IMU_SCK 13
#define MAIN_IMU_INT 9
#define MAIN_IMU_RST 5
#define MAIN_IMU_PS1 4
#define MAIN_IMU_PS0 3

#define SEC_IMU_SCL 19
#define SEC_IMU_SDA 18
#define SECONDARY_IMU_I2C_ADDR 0x68 // 104

struct Quaternions{ // Quaternion data
    float i;
    float j;
    float k;
    float r;
}; 

class GNCData {
private:
    double pitch;
    double yaw;
    double roll;
    double vX;
    double vY;
    double vZ;
    double aX;
    double aY;
    double aZ;
    double mX;
    double mY;
    double mZ;
    double omegaX;
    double omegaY;
    double omegaZ;
    double alphaX;
    double alphaY;
    double alphaZ;

    // Just pointers, no allocation here
    ICM42688* Secondary;
    BNO08x* Main;

public:
    Vector3D pyyr_init;
    Vector3D omega_init;
    Vector3D alpha_init;
    Vector3D U_init; // Velocity
    Vector3D A_init; // Acceleration
    Vector3D Magnet_init; // Magnetometer data
    Quaternions quats_init;

    Vector3D pyyr_o;
    Vector3D omega_o;
    Vector3D alpha_o;
    Vector3D U_o; // Velocity
    Vector3D A_o; // Acceleration
    Vector3D Magnet_o; // Magnetometer data
    Quaternions quats_o;

    Vector3D* pyyr = &pyyr_o;
    Vector3D* omega = &omega_o;
    Vector3D* alpha = &alpha_o;
    Vector3D* U   = &U_o; // Velocity
    Vector3D* A   = &A_o; // Acceleration
    Vector3D* Magnet = &Magnet_o; // Magnetometer data
    Quaternions* quats = &quats_o;

    GNCData(ICM42688* secIMU, BNO08x* mainIMU);

    ~GNCData();
};

struct IMUSensors {
    BNO08x MainIMU;
    ICM42688 SecondaryIMU = ICM42688(Wire, SECONDARY_IMU_I2C_ADDR);
    GNCData Data = GNCData(&SecondaryIMU, &MainIMU);
    Quaternions* Quats = Data.quats;
    sh2_SensorValue_t sensorValue;
    sh2_SensorId_t reportType = SH2_ARVR_STABILIZED_RV;
    long reportIntervalUs = 5000;
    bool MainStatus = false;
    bool SecStatus = false;

    void setReports();
    void Init();
    Vector3D* directPYR(Vector3D* accel, Vector3D* gyro, Vector3D* magn, double dt);
    const char*  Update();
    const char*  Update_Tared();
};