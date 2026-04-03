#pragma once

#include "I2Cdev.h"
#include "MPU6050.h"
#include "QMC5883P.h"
#include "Fusion.h"
#include <chrono>

#define SAMPLE_RATE 100

class MPU
{
private:
    I2Cdev &i2cDev;
    MPU6050 mpu6050;
    QMC5883P qmc5883p;
    bool hasQMC;

    
    const FusionAhrsSettings settings = {
        .convention = FusionConventionNwu,
        .gain = 5,
        .gyroscopeRange = 0,
        .accelerationRejection = 0,
        .magneticRejection = 0,
        .recoveryTriggerPeriod = 0
    };
    

    FusionBias bias;
    FusionAhrs ahrs;

    bool firstRead = true;
    std::chrono::_V2::steady_clock::time_point previousTime;


public:
    MPU(I2Cdev &i2cDev, bool hasQMC=true);
    void Calibrate();
    bool Connect();
    FusionEuler ReadFusion();

};
