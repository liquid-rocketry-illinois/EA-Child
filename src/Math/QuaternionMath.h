#include <Arduino.h>
#include <SparkFun_BNO08x_Arduino_Library.h>
#include "Vector3D.h"

// Simple header implementing neccessary quaternion calculations
namespace QuatMath{
    void quaternionToEuler(float qr, float qi, float qj, float qk, Vector3D* pyr, bool degrees = false);
    void quaternionToEulerRV(sh2_RotationVectorWAcc_t* rotational_vector, Vector3D* pyr, bool degrees = false);
    void quaternionToEulerGI(sh2_GyroIntegratedRV_t* rotational_vector, Vector3D* pyr, bool degrees = false);
};
    