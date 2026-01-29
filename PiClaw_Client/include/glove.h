#include "mpu.h"


class Glove
{
private:
    MPU mpu;
public:
    Glove(/* args */);
    Glove(uint8_t mpu_address);
};

Glove::Glove(/* args */):mpu()
{
}

Glove::Glove(uint8_t mpu_address):mpu(mpu_address)
{

}


