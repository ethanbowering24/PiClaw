#include "mpu.h"
#include "I2Cdev.h"
#include "Fusion.h"

#include <thread>
#include <chrono>
#include <iostream>

MPU::MPU(const char* devPath) : i2cDev(devPath), mpu6050(i2cDev)
{

};

MPU::MPU(const char* devPath, uint8_t mpu_address) : i2cDev(devPath), mpu6050(i2cDev, mpu_address)
{

};

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
    std::cout <<"init"<< std::endl;
    //struct timespec remaining, request = { 2, 0 };
    //nanosleep(&request, &remaining);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    mpu6050.CalibrateAccel();
    //delay
    mpu6050.CalibrateGyro();
    //delay
    mpu6050.PrintActiveOffsets();
    
    mpu6050.setDLPFMode(0x03); // set DLPF
    mpu6050.setExternalFrameSync(0x00); // disable FSYNC

    //Sample Rate = Gyroscope Output Rate / (1 + SMPLRT_DIV)
    //  1000/(1+4) = 200Hz
    mpu6050.setRate(0x04);


    std::cout << "RATE: " << static_cast<int>(mpu6050.getRate()) << std::endl;
    std::cout << "FSYNC: " << static_cast<int>(mpu6050.getExternalFrameSync()) << std::endl;
    std::cout << "DLPF MODE: " << static_cast<int>(mpu6050.getDLPFMode()) << std::endl;
    std::cout << "CLK: " << static_cast<int>(mpu6050.getClockSource()) << std::endl;
    std::cout << "Gscale: " << static_cast<int>(mpu6050.getFullScaleGyroRange()) << std::endl;
    std::cout << "Ascale: " << static_cast<int>(mpu6050.getFullScaleAccelRange()) << std::endl;


    FusionBiasInitialise(&bias, SAMPLE_RATE);
    FusionAhrsInitialise(&ahrs);
    FusionAhrsSetSettings(&ahrs, &settings);
    return true;
}

void MPU::ReadFusion()
{
    auto next_loop_time = std::chrono::steady_clock::now();
    auto previousTime = std::chrono::steady_clock::now();

    while (true)
    {
        const clock_t timestamp = clock();
        int16_t ax, ay, az;
        int16_t gx, gy, gz;
        mpu6050.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    
        FusionVector gyroscope = {gx/131.0f, gy/131.0f, gz/131.0f};
        FusionVector accelerometer = {ax/16384.0f, ay/16384.0f, az/16384.0f};
        //FusionVector magnetometer = {1.0f, 0.0f, 0.0f};
        
        gyroscope = FusionBiasUpdate(&bias, gyroscope);
    
        // Calculate delta time to compensate for gyroscope sample clock errors
        auto currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<float> elapsed_seconds = currentTime - previousTime;
        float deltaTime = elapsed_seconds.count();
        previousTime = currentTime;

        //FusionAhrsUpdate(&ahrs, gyroscope, accelerometer, magnetometer, deltaTime);
        FusionAhrsUpdateNoMagnetometer(&ahrs, gyroscope, accelerometer, deltaTime);

        // Print AHRS outputs
        const FusionEuler euler = FusionQuaternionToEuler(FusionAhrsGetQuaternion(&ahrs));

        const FusionVector earth = FusionAhrsGetEarthAcceleration(&ahrs);

        printf("Roll %0.1f, Pitch %0.1f, Yaw %0.1f, X %0.1f, Y %0.1f, Z %0.1f\n",
               euler.angle.roll, euler.angle.pitch, euler.angle.yaw,
               earth.axis.x, earth.axis.y, earth.axis.z);


        // 5ms for 200Hz
        next_loop_time += std::chrono::milliseconds(5);
        std::this_thread::sleep_until(next_loop_time);
    }

}

int main()
{

    //wiringPiSetupPinType(WPI_PIN_PHYS);
    //pinMode(26, INPUT);


    MPU mpu("/dev/i2c-1");

    mpu.Connect();
    mpu.ReadFusion();

}

