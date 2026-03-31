#include "arm.h"
#include "I2Cdev.h"
#include "mpu.h"
#include "Fusion.h"
#include "packet.h"

#include <iostream>
#include <chrono>
#include <thread>

Arm::Arm() : i2c1("/dev/i2c-1"), i2c3("/dev/i2c-3"), i2c4("/dev/i2c-4"), wrist(i2c1, false), forearm(i2c3, false), upArm(i2c4, false)
{

}

bool Arm::Connect()
{
    bool ret = true;
    if (!wrist.Connect())
    {
        std::cerr << "Wrist connection failed" << std::endl;
        ret = false;
    }
    if (!forearm.Connect())
    {
        std::cerr << "Forearm connection failed" << std::endl;
        ret = false;
    }
    if (!upArm.Connect())
    {
        std::cerr << "UpArm connection failed" << std::endl;
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

    std::cout << "UpArm Calibration: " << std::endl;
    upArm.Calibrate();
}

Packet Arm::Read()
{
    Packet packet;

    auto readEuler = [&](auto& sensor, int rollIdx, int pitchIdx, int yawIdx)
    {
        FusionEuler euler = sensor.ReadFusion();
        packet.values[rollIdx] = euler.angle.roll;
        packet.values[pitchIdx] = euler.angle.pitch;
        packet.values[yawIdx] = euler.angle.yaw;
    };

    readEuler(wrist, WRIST_ROLL, WRIST_PITCH, WRIST_YAW);
    readEuler(forearm, FOREARM_ROLL, FOREARM_PITCH, FOREARM_YAW);

    return packet;
    
}