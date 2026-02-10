#include "mpu.h"
#include "I2Cdev.h"
#include "Fusion.h"
#include "time.h" //move to chrono

#include <iostream>

MPU::MPU():mpu6050()
{

};

MPU::MPU(uint8_t mpu_address):mpu6050(mpu_address)
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

    FusionBiasInitialise(&bias, SAMPLE_RATE);
    FusionAhrsInitialise(&ahrs);
    FusionAhrsSetSettings(&ahrs, &settings);
    return true;
}

void MPU::ReadFusion()
{
    struct timespec remaining, request = { 0, 1000000000L / SAMPLE_RATE };
    clock_t previousTimestamp;

    while (true)
    {
        const clock_t timestamp = clock();
        int16_t ax, ay, az;
        int16_t gx, gy, gz;
        mpu6050.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    
        FusionVector gyroscope = {gx/131.0f, gy/131.0f, gz/131.0f};
        FusionVector accelerometer = {ax/16384.0f, ay/16384.0f, az/16384.0f};
        //FusionVector magnetometer = {1.0f, 0.0f, 0.0f};
    
        gyroscope = FusionModelInertial(gyroscope, gyroscopeMisalignment, gyroscopeSensitivity, gyroscopeOffset);
        accelerometer = FusionModelInertial(accelerometer, accelerometerMisalignment, accelerometerSensitivity, accelerometerOffset);
        //magnetometer = FusionModelMagnetic(magnetometer, softIronMatrix, hardIronOffset);
    
        gyroscope = FusionBiasUpdate(&bias, gyroscope);
    
        // Calculate delta time to compensate for gyroscope sample clock errors
        const float deltaTime = (float) (timestamp - previousTimestamp) / (float) CLOCKS_PER_SEC;
        previousTimestamp = timestamp;

        //FusionAhrsUpdate(&ahrs, gyroscope, accelerometer, magnetometer, deltaTime);
        FusionAhrsUpdateNoMagnetometer(&ahrs, gyroscope, accelerometer, deltaTime);

        // Print AHRS outputs
        const FusionEuler euler = FusionQuaternionToEuler(FusionAhrsGetQuaternion(&ahrs));

        const FusionVector earth = FusionAhrsGetEarthAcceleration(&ahrs);

        printf("Roll %0.1f, Pitch %0.1f, Yaw %0.1f, X %0.1f, Y %0.1f, Z %0.1f\n",
               euler.angle.roll, euler.angle.pitch, euler.angle.yaw,
               earth.axis.x, earth.axis.y, earth.axis.z);

        nanosleep(&request, &remaining);
        
    }

}

int main()
{

    //wiringPiSetupPinType(WPI_PIN_PHYS);
    //pinMode(26, INPUT);
    I2Cdev::initialize("/dev/i2c-1");

    MPU mpu;

    mpu.Connect();
    mpu.ReadFusion();

}

