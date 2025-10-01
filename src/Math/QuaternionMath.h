#include <Arduino.h>
#include <Adafruit_BNO08x.h>
#include "Vector3D.h"

// Simple header implementing neccessary quaternion calculations

void quaternionToEuler(float qr, float qi, float qj, float qk, Vector3D* ypr, bool degrees = false) {

    float sqr = sq(qr);
    float sqi = sq(qi);
    float sqj = sq(qj);
    float sqk = sq(qk);

    ypr->setX(atan2(2.0 * (qi * qj + qk * qr), (sqi - sqj - sqk + sqr)));
    ypr->setY(asin(-2.0 * (qi * qk - qj * qr) / (sqi + sqj + sqk + sqr)));
    ypr->setZ(atan2(2.0 * (qj * qk + qi * qr), (-sqi - sqj + sqk + sqr)));

    if (degrees) {
      ypr->setX(ypr->getX() * RAD_TO_DEG) ;
      ypr->setY(ypr->getX() * RAD_TO_DEG);
      ypr->setZ(ypr->getX() * RAD_TO_DEG);
    }
}

void quaternionToEulerRV(sh2_RotationVectorWAcc_t* rotational_vector, Vector3D* ypr, bool degrees = false) {
    quaternionToEuler(rotational_vector->real, rotational_vector->i, rotational_vector->j, rotational_vector->k, ypr, degrees);
}

void quaternionToEulerGI(sh2_GyroIntegratedRV_t* rotational_vector, Vector3D* ypr, bool degrees = false) {
    quaternionToEuler(rotational_vector->real, rotational_vector->i, rotational_vector->j, rotational_vector->k, ypr, degrees);
}