#pragma once

#include <ArduinoEigen.h>
#include <iostream>
#include "../Sensors/Sensors.h"
#include <Arduino.h>

using Eigen::MatrixXd;
using Eigen::Matrix;

struct CTRLS {
    Sensors* Data;

    void Init(Sensors* sensor);
    String Update();
};

class Controls{
    public: 

    Controls(float t_motor_burnout, 
             float t_estimated_apogee,
             float t_launch_rail_clearance,
             float prop_mass,
             float L_ne,
             float dt,
             MatrixXd Ks,
             MatrixXd L,
             MatrixXd x0,
             MatrixXd u0);
    ~Controls();

    struct ABK_Matrices {
        Matrix<double,10,10> A;
        Matrix<double,10,1>  B;
        Matrix<double,1,10>  K;
    };

    MatrixXd R_BW_from_q(float qw, float qx, float qy, float qz);
    ABK_Matrices getABK(MatrixXd stateVector);

    float t_motor_burnout; // seconds
    float t_estimated_apogee; // seconds
    float t_launch_rail_clearance; // seconds
    float prop_mass; // kg
    float L_ne; // m
    const String csv_path = "Flight_Data.txt"; 
    MatrixXd A;
    MatrixXd B;
    MatrixXd C;
    MatrixXd f_preburnout;
    MatrixXd f_postburnout;
    MatrixXd Ks;
    MatrixXd L;
    //float[###] vars; // Need number of vars
    MatrixXd f_params;
    MatrixXd f_subs;
    float dt;

    MatrixXd x0;
    MatrixXd u0;
    //float t_sym; // Symbolic
    nullptr_t T; // What will this be
    nullptr_t g; // What will this be

    // Logging (Output matrices?)
    MatrixXd states;
    MatrixXd inputs;
    MatrixXd As;
    MatrixXd Bs;
};