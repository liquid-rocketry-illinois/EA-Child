#include "ControlsSystem.h"

void CTRLS::Init(Sensors* sensors){
    this->Data = sensors;
}

String CTRLS::Update(){
    Data->Update();

    // CONTROLLER CODE

    

    // END CONTROLLER CODE

    return (String)(12); // Return actuation amount
}

MatrixXd Controls::R_BW_from_q(float qw, float qx, float qy, float qz){
    float norm = sqrt(qw*qw + qx*qx + qy*qy + qz*qz);
    if (norm < 1e-12) norm = 1e-12;
    float s = 1.0 / norm;

    qw *= s;
    qx *= s;
    qy *= s;
    qz *= s;

    float xx = qx*qx;
    float yy = qy*qy;
    float zz = qz*qz;

    float wx = qw*qx;
    float wy = qw*qy;
    float wz = qw*qz;

    float xy = qx*qy;
    float xz = qx*qz;
    float yz = qy*qz;

    MatrixXd R(3,3);
    R << 1-2*(yy+zz),   2*(xy+wz),   2*(xz-wy),
         2*(xy-wz),     1-2*(xx+zz), 2*(yz+wx),
         2*(xz+wy),     2*(yz-wx),   1-2*(xx+yy);
    return R;
}

// TODO
Controls::ABK_Matrices Controls::getABK(MatrixXd stateVector){

}

Controls::Controls( float t_motor_burnout = 1.971, 
                    float t_estimated_apogee = 13.571,
                    float t_launch_rail_clearance = 0.164,
                    float prop_mass = 0.355,
                    float L_ne = 1.17,
                    float dt = 0.01,
                    MatrixXd Ks = MatrixXd(2, 2),
                    MatrixXd L = MatrixXd(2, 2),
                    MatrixXd x0 = MatrixXd(2, 2),
                    MatrixXd u0 = MatrixXd(2, 2) ){
    this->t_motor_burnout = t_motor_burnout; // seconds
    this->t_estimated_apogee = t_estimated_apogee; // seconds
    this->t_launch_rail_clearance = t_launch_rail_clearance; // seconds
    this->prop_mass = prop_mass; // kg
    this->L_ne = L_ne; // m

    this->A = MatrixXd();
    this->B = MatrixXd();
    this->C = MatrixXd();
    //this->f_preburnout;
    //this->f_postburnout;
    this->Ks = Ks;
    this->L = L;
    //this->vars;
    //this->f_params;
    //this->f_subs;
    this->dt = dt;
    this->x0 = x0;
    this->u0 = u0;
    //this->t_sym;
    //this->T;
    //this->g;

    // Logging
    this->states = MatrixXd();
    this->inputs = MatrixXd();
    this->As = MatrixXd();
    this->Bs = MatrixXd();
}