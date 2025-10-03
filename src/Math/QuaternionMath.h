#include <Arduino.h>
#include <Adafruit_BNO08x.h>
#include "Vector3D.h"

// Simple header implementing neccessary quaternion calculations
namespace QuatMath{
    void quaternionToEuler(float qr, float qi, float qj, float qk, Vector3D* pyr, bool degrees = false);
    void quaternionToEulerRV(sh2_RotationVectorWAcc_t* rotational_vector, Vector3D* pyr, bool degrees = false);
    void quaternionToEulerGI(sh2_GyroIntegratedRV_t* rotational_vector, Vector3D* pyr, bool degrees = false);
};
    