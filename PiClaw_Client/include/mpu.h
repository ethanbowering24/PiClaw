#include "MPU6050.h"
#include "Fusion.h"

#define SAMPLE_RATE 200

class MPU
{
private:
    MPU6050 mpu6050;
    int MPUOffsets[6] = {-262, 1702, 1307 , 193, 103, 74};// vert board

    const FusionAhrsSettings settings = {
        .convention = FusionConventionNwu,
        .gain = 0.5f,
        .gyroscopeRange = 250.0f, /* replace with actual gyroscope range */
        .accelerationRejection = 0,
        .magneticRejection = 0,
        .recoveryTriggerPeriod = 0
    };

    FusionBias bias;
    FusionAhrs ahrs;


public:
    MPU();
    MPU(uint8_t mpu_address);
    bool Connect();
    void ReadFusion();


};
