#include "I2Cdev.h"
#include "MPU6050.h"
#include "Fusion.h"

#define SAMPLE_RATE 200

class MPU
{
private:
    I2Cdev &i2cDev; //turn this into a reference so that other classes can share the i2c
    MPU6050 mpu6050;
    //int MPUOffsets[6] = {-262, 1702, 1307 , 193, 103, 74};// vert board

    /*
    const FusionAhrsSettings settings = {
        .convention = FusionConventionNwu,
        .gain = 0.5f,
        .gyroscopeRange = 0,
        .accelerationRejection = 0,
        .magneticRejection = 0,
        .recoveryTriggerPeriod = 0
    };
    */

    FusionBias bias;
    FusionAhrs ahrs;


public:
    MPU(I2Cdev &i2cDev);
    MPU(I2Cdev &i2cDev, uint8_t mpu_address);
    void Calibrate();
    bool Connect();
    void ReadFusion();


};
