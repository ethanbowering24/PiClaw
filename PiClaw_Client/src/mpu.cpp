#include "mpu.h"
#include "I2Cdev.h"
#include "Fusion.h"

#include <thread>
#include <chrono>
#include <iostream>

#include <string>
#include <vector>
#include <algorithm>

MPU::MPU(I2Cdev &i2cDev, bool hasQMC) : i2cDev(i2cDev), mpu6050(i2cDev), qmc5883p(i2cDev), hasQMC(hasQMC)
{
    FusionBiasInitialise(&bias, SAMPLE_RATE);
    FusionAhrsInitialise(&ahrs);
    FusionAhrsSetSettings(&ahrs, &settings);
};

void MPU::Calibrate()
{
    mpu6050.CalibrateAccel();
    mpu6050.CalibrateGyro();
    mpu6050.PrintActiveOffsets();

    if (hasQMC)
    {
        // QMC self test
    }
}

bool MPU::Connect()
{
    //TODO: add device ID
    if (mpu6050.testConnection())
    {
        std::cout << "MPU6050 Connection Successful" << std::endl;
    }
    else
    {
        std::cout << "MPU6050 Connection Failed" << std::endl;
        return false;
    }

    mpu6050.initialize(); 
    mpu6050.setDLPFMode(0x03);          // set DLPF
    mpu6050.setExternalFrameSync(0x00); // disable FSYNC
    mpu6050.setRate(0x09);              // Sample Rate = Gyroscope Output Rate / (1 + SMPLRT_DIV)
                                        //      1000/(1+4) = 200Hz

    if (hasQMC)
    {
        if (qmc5883p.testConnection())
        {
            std::cout << "QMC5883P Connection Successful" << std::endl;\
        }
        else
        {
            std::cout << "QMC5883P Connection Failed" << std::endl;
            return false;
        }

        qmc5883p.initialize();
        qmc5883p.setMode(0x01); // normal mode
        qmc5883p.setODR(0x02); //100Hz
        qmc5883p.setRange(0x00); //30 gauss (?? Confirm this selection)
    }
    else
    {
        std::cout << "No QMC5883P" << std::endl;
    }
    
    return true;
}

FusionEuler MPU::ReadFusion()
{
    if (firstRead)
    {
        previousTime = std::chrono::steady_clock::now(); //TODO: move this into object construction
        firstRead = false;
    }
    
    int16_t ax, ay, az;
    int16_t gx, gy, gz;
    mpu6050.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    //TODO replace magic numbers with named values
    FusionVector gyroscope = {gx/131.0f, gy/131.0f, gz/131.0f};
    FusionVector accelerometer = {ax/16384.0f, ay/16384.0f, az/16384.0f};
    FusionVector magnetometer = FUSION_VECTOR_ZERO;
    gyroscope = FusionBiasUpdate(&bias, gyroscope);

    if (hasQMC)
    {
        int16_t mx ,my ,mz ;
        qmc5883p.getRawMagnetic(&mx, &my, &mz);
        magnetometer = {mx/1000.0f, my/1000.0f, mz/1000.0f};
        //std::cout << "magx: " << mx/1000.0f << " magy: " << my/1000.0f << " magz: " << mz/1000.0f << std::endl;
    }


    // Calculate delta time to compensate for gyroscope sample clock errors
    auto currentTime = std::chrono::steady_clock::now();
    std::chrono::duration<float> elapsed_seconds = currentTime - previousTime;
    float deltaTime = elapsed_seconds.count();
    previousTime = currentTime;

    if (hasQMC)
    {
        FusionAhrsUpdate(&ahrs, gyroscope, accelerometer, magnetometer, deltaTime);
    }
    else
    {
        FusionAhrsUpdateNoMagnetometer(&ahrs, gyroscope, accelerometer, deltaTime);
    }

    // Print AHRS outputs
    return(FusionQuaternionToEuler(FusionAhrsGetQuaternion(&ahrs)));

}

bool MPU::Initialized()
{    
    return !FusionAhrsGetFlags(&ahrs).initialising;
}