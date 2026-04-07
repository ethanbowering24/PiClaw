#include "arm.h"
#include "I2Cdev.h"
#include "mpu.h"
#include "Fusion.h"
#include "packet.h"
#include "ADS1115.h"

#include <iostream>
#include <chrono>
#include <thread>

Arm::Arm() : i2c1("/dev/i2c-1"), i2c3("/dev/i2c-3"), i2c4("/dev/i2c-4"), wrist(i2c1, false), forearm(i2c3, true), upArm(i2c4, false), adc(i2c4)
{

}

bool Arm::Connect()
{
    if (!wrist.Connect())
    {
        std::cerr << "Wrist connection failed" << std::endl;
        return false;
    }
    if (!forearm.Connect())
    {
        std::cerr << "Forearm connection failed" << std::endl;
        return false;
    }
    if (!upArm.Connect())
    {
        std::cerr << "UpArm connection failed" << std::endl;
        return false;
    }
    if (!adc.Connect())
    {
        std::cerr << "ADC connection failed" << std::endl;
        return false;
    }
    
    return true;
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
    readEuler(upArm, UPARM_ROLL, UPARM_PITCH, UPARM_YAW);
    packet.values[CLAW] = adc.Read();
    packet.values[FOREARM_YAW] -= yawOffset;
    return packet;
    
}

void Arm::Initialize()
{
    
    auto readEuler = [&](auto& sensor, int rollIdx, int pitchIdx, int yawIdx, Packet packet)
    {
        FusionEuler euler = sensor.ReadFusion();
        packet.values[rollIdx] = euler.angle.roll;
        packet.values[pitchIdx] = euler.angle.pitch;
        packet.values[yawIdx] = euler.angle.yaw;
    };

    float tmp;
    
    do
    {
        Packet packet;
        auto next_loop_time = std::chrono::steady_clock::now();

        readEuler(wrist, WRIST_ROLL, WRIST_PITCH, WRIST_YAW, packet);
        readEuler(forearm, FOREARM_ROLL, FOREARM_PITCH, FOREARM_YAW, packet);
        readEuler(upArm, UPARM_ROLL, UPARM_PITCH, UPARM_YAW, packet);

        tmp = packet.values[FOREARM_YAW];

        next_loop_time += std::chrono::milliseconds(10);
        std::this_thread::sleep_until(next_loop_time);
        
    } while (!wrist.Initialized() && !forearm.Initialized() && !upArm.Initialized());

    yawOffset = tmp;

    return;

}