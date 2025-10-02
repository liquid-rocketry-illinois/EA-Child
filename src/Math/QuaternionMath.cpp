#include "QuaternionMath.h"

void QuatMath::quaternionToEuler(float qr, float qi, float qj, float qk, Vector3D* pyr, bool degrees = false) {

    float sqr = sq(qr);
    float sqi = sq(qi);
    float sqj = sq(qj);
    float sqk = sq(qk);

    pyr->setX(asin(-2.0 * (qi * qk - qj * qr) / (sqi + sqj + sqk + sqr)));
    pyr->setY(atan2(2.0 * (qi * qj + qk * qr), (sqi - sqj - sqk + sqr)));
    pyr->setZ(atan2(2.0 * (qj * qk + qi * qr), (-sqi - sqj + sqk + sqr)));

    if (degrees) {
      pyr->setX(pyr->getX() * RAD_TO_DEG);
      pyr->setY(pyr->getX() * RAD_TO_DEG);
      pyr->setZ(pyr->getX() * RAD_TO_DEG);
    }
}

void QuatMath::quaternionToEulerRV(sh2_RotationVectorWAcc_t* rotational_vector, Vector3D* pyr, bool degrees = false) {
    QuatMath::quaternionToEuler(rotational_vector->real, rotational_vector->i, rotational_vector->j, rotational_vector->k, pyr, degrees);
}

void QuatMath::quaternionToEulerGI(sh2_GyroIntegratedRV_t* rotational_vector, Vector3D* pyr, bool degrees = false) {
    QuatMath::quaternionToEuler(rotational_vector->real, rotational_vector->i, rotational_vector->j, rotational_vector->k, pyr, degrees);
}