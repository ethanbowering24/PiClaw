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
};

/*
MPU::MPU(I2Cdev &i2cDev, uint8_t mpu_address) : i2cDev(i2cDev), mpu6050(i2cDev, mpu_address)
{

};
*/

void MPU::Calibrate()
{
    mpu6050.CalibrateAccel();
    mpu6050.CalibrateGyro();
    mpu6050.PrintActiveOffsets();
    // save to a file

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
    mpu6050.setRate(0x04);              // Sample Rate = Gyroscope Output Rate / (1 + SMPLRT_DIV)
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
        qmc5883p.setODR(0x03); //200Hz
        qmc5883p.setRange(0x00); //30 gauss (?? Confirm this selection)
    }
    else
    {
        std::cout << "No QMC5883P" << std::endl;
    }
    
    
    
    
/*
    std::cout << "RATE: " << static_cast<int>(mpu6050.getRate()) << std::endl;
    std::cout << "FSYNC: " << static_cast<int>(mpu6050.getExternalFrameSync()) << std::endl;
    std::cout << "DLPF MODE: " << static_cast<int>(mpu6050.getDLPFMode()) << std::endl;
    std::cout << "CLK: " << static_cast<int>(mpu6050.getClockSource()) << std::endl;
    std::cout << "Gscale: " << static_cast<int>(mpu6050.getFullScaleGyroRange()) << std::endl;
    std::cout << "Ascale: " << static_cast<int>(mpu6050.getFullScaleAccelRange()) << std::endl;
*/
    return true;
}

void MPU::ReadFusionOld()
{

    FusionBiasInitialise(&bias, SAMPLE_RATE);
    FusionAhrsInitialise(&ahrs);
    //FusionAhrsSetSettings(&ahrs, &settings);

    auto next_loop_time = std::chrono::steady_clock::now();
    previousTime = std::chrono::steady_clock::now();

    while (true)
    {
        const clock_t timestamp = clock();
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
        const FusionEuler euler = FusionQuaternionToEuler(FusionAhrsGetQuaternion(&ahrs));

        const FusionVector earth = FusionAhrsGetEarthAcceleration(&ahrs);

        printf("Roll %0.1f, Pitch %0.1f, Yaw %0.1f, X %0.1f, Y %0.1f, Z %0.1f\n",
               euler.angle.roll, euler.angle.pitch, euler.angle.yaw,
               earth.axis.x, earth.axis.y, earth.axis.z);

        /*
        FusionAhrsFlags flags = FusionAhrsGetFlags(&ahrs);
        if (flags.accelerationRecovery)
        {
            std::cout << "ACCELERATION RECOVERY" << std::endl;
        }
        if (flags.angularRateRecovery)
        {
            std::cout << "ANGULAR RECOVERY" << std::endl;
        }
        if (flags.initialising)
        {
            std::cout << "INITIALIZING" << std::endl;
        }
        if (flags.magneticRecovery)
        {
            std::cout << "MAGNETIC RECOVERY" << std::endl;
        }   
        */
        
        // 5ms for 200Hz
        next_loop_time += std::chrono::milliseconds(5);
        std::this_thread::sleep_until(next_loop_time);
    }

}

void MPU::ReadFusion(FusionEuler& euler)
{
    if (firstRead)
    {
        previousTime = std::chrono::steady_clock::now();
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
    euler = FusionQuaternionToEuler(FusionAhrsGetQuaternion(&ahrs));


    /*
    const FusionVector earth = FusionAhrsGetEarthAcceleration(&ahrs);

    printf("Roll %0.1f, Pitch %0.1f, Yaw %0.1f, X %0.1f, Y %0.1f, Z %0.1f\n",
            euler.angle.roll, euler.angle.pitch, euler.angle.yaw,
            earth.axis.x, earth.axis.y, earth.axis.z);
    */

    /*
    FusionAhrsFlags flags = FusionAhrsGetFlags(&ahrs);
    if (flags.accelerationRecovery)
    {
        std::cout << "ACCELERATION RECOVERY" << std::endl;
    }
    if (flags.angularRateRecovery)
    {
        std::cout << "ANGULAR RECOVERY" << std::endl;
    }
    if (flags.initialising)
    {
        std::cout << "INITIALIZING" << std::endl;
    }
    if (flags.magneticRecovery)
    {
        std::cout << "MAGNETIC RECOVERY" << std::endl;
    }   
    */
    

}

/*
int main(int argc, char* argv[])
{

    I2Cdev i2c1("/dev/i2c-1");
    I2Cdev i2c3("/dev/i2c-3");
    I2Cdev i2c4("/dev/i2c-4");


    MPU wrist(i2c1);
    MPU forearm(i2c3);
    MPU upper(i2c4);

    wrist.Connect();

    //wiringPiSetupPinType(WPI_PIN_PHYS);
    //pinMode(26, INPUT);
    std::vector<std::string> args(argv, argv + argc);
    if (std::find(args.begin(), args.end(), "--calibrate") != args.end()) {
        wrist.Calibrate();
        return 0;
    }
    
    wrist.ReadFusion();
    return 0;

}
*/
