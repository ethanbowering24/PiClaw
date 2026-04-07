#pragma once
#include "I2Cdev.h"
#include "mpu.h"
#include "Fusion.h"
#include "packet.h"
#include "adc.h"
#include <iostream>

class Arm
{
    private:
        I2Cdev i2c1;
        I2Cdev i2c3;
        I2Cdev i2c4;
        MPU wrist;
        MPU forearm;
        MPU upArm;
        ADC adc;
        float yawOffset = 0;


    public:
        Arm();
        bool Connect();
        void Calibrate();
        Packet Read();
        void Initialize();
};