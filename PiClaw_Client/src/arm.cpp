#include "arm.h"
#include "I2Cdev.h"
#include "mpu.h"
#include "Fusion.h"

#include <iostream>
#include <chrono>
#include <thread>

Arm::Arm() : i2c1("/dev/i2c-1"), i2c3("/dev/i2c-3"), wrist(i2c1, false), forearm(i2c3, false)
{

}

bool Arm::Connect()
{
    bool ret = true;
    if (!wrist.Connect())
    {
        std::cout << "Wrist connection failed" << std::endl;
        ret = false;
    }
    if (!forearm.Connect())
    {
        std::cout << "Forearm connection failed" << std::endl;
        ret = false;
    }
    return ret;
}

void Arm::Calibrate()
{
    std::cout << "Wrist Calibration: " << std::endl;
    wrist.Calibrate();

    std::cout << "Forearm Calibration: " << std::endl;
    forearm.Calibrate();
}

void Arm::Read()
{
    auto next_loop_time = std::chrono::steady_clock::now();
    while (true)
    {
        FusionEuler wristEuler;
        wrist.ReadFusion(wristEuler);

        FusionEuler forearmEuler;
        forearm.ReadFusion(forearmEuler);

        std::cout << "Wrist:" << std::endl;
        printf("Roll %0.1f, Pitch %0.1f, Yaw %0.1f\n",
               wristEuler.angle.roll, wristEuler.angle.pitch, wristEuler.angle.yaw);      
        std::cout << "Forearm:" << std::endl;    
        printf("Roll %0.1f, Pitch %0.1f, Yaw %0.1f\n",
               forearmEuler.angle.roll, forearmEuler.angle.pitch, forearmEuler.angle.yaw);   

        next_loop_time += std::chrono::milliseconds(5);
        std::this_thread::sleep_until(next_loop_time);
    }
    
}